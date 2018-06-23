#include "stdafx.h"
#include "utils.h"
#include "BPTreeNode.h"

BPTreeNode::BPTreeNode(const char * filename, int id, int keyLength) :filename(filename), id(id), keyLength(keyLength)
{
	BufferManager *bm = Utils::GetBufferManager();
	Block* block = bm->FetchBlock(filename, id);
	int *data = reinterpret_cast<int *>(block->content);

	size = data[0];
	keys.push_back(NULL);
	pointers.push_back(data[1]);
	leaf = pointers[0] < 0;
	blockRemoved = false;

	int j = 8;
	for (int i = 1; i <= size; i++)
	{
		char *key = new char[keyLength];
		memcpy(key, block->content + j, keyLength);
		keys.push_back(key);
		pointers.push_back(*reinterpret_cast<int*>(block->content + j + keyLength));
		j += keyLength + 4;
	}
}

BPTreeNode::BPTreeNode(const char *filename, int id, int keyLength, bool isLeaf, int firstPointer) :filename(filename), id(id), keyLength(keyLength), leaf(isLeaf)
{
	size = 0;
	keys.push_back(NULL);
	pointers.push_back(firstPointer);
	dirty = true;
	blockRemoved = false;
}

BPTreeNode::~BPTreeNode()
{
	if (dirty && !blockRemoved)
	{
		BufferManager* bm = Utils::GetBufferManager();
		Block* block = bm->FetchBlock(filename, id);
		Byte *data = block->content;

		memcpy(data, &size, 4);

		memcpy(data + 4, &pointers[0], 4);

		int j = 8;
		for (int i = 1; i <= size; i++)
		{
			memcpy(data + j, keys[i], keyLength);
			memcpy(data + j + keyLength, &pointers[i], 4);
			j += keyLength + 4;
		}

		block->dirty = true;
	}

	for (char *key : keys)
		if (key != NULL)
			delete[] key;
}

size_t BPTreeNode::FindPosition(const char *key) const
{
	vector<char *>::const_iterator it, first = keys.begin();
	ptrdiff_t count, step;
	count = keys.end() - (keys.begin() + 1);

	while (count > 0) {
		it = first;
		step = count / 2;
		std::advance(it, step);
		if (memcmp(key, *it, keyLength) >= 0) {
			first = ++it;
			count -= step + 1;
		}
		else
			count = step;
	}
	return first - (keys.begin() + 1);
}

bool BPTreeNode::IsLeaf() const
{
	return leaf;
}

const char *BPTreeNode::GetKey(int pos) const
{
	if (pos > size || pos <= 0)
	{
		cerr << "Get key error: Position " << pos << " out of range." << endl;
		return NULL;
	}
	return keys[pos];
}

int BPTreeNode::GetPointer(int pos) const
{
	if (pos > size || pos <= 0)
	{
		cerr << "Get pointer error: Position " << pos << " out of range." << endl;
		return NULL;
	}
	return pointers[pos];
}

void BPTreeNode::Insert(int pos, const char * key, int pointer)
{
	if (pos > size || pos < 0)
	{
		cerr << "Insert node error: Position" << pos << " out of range." << endl;
		return;
	}

	dirty = true;
	char *k = new char[keyLength];
	memcpy(k, key, keyLength);
	if (pos == size)
	{
		keys.push_back(k);
		pointers.push_back(pointer);
	}
	else
	{
		keys.insert(keys.begin() + pos + 1, k);
		pointers.insert(pointers.begin() + pos + 1, pointer);
	}
	size++;
}

int BPTreeNode::GetSize() const
{
	return size;
}

BPTreeNode * BPTreeNode::Split(int newId, char *newKey)
{
	dirty = true;

	int pos = size / 2 + (leaf ? 0 : 1);
	memcpy(newKey, keys[size / 2 + 1], keyLength);
	BPTreeNode* node = new BPTreeNode(filename.c_str(), newId, keyLength, leaf, leaf ? -1 : pointers[pos]);

	// Copy former half of keys-pointers to new node
	for (pos++; pos <= size; pos++)
		node->Insert(node->GetSize(), keys[pos], pointers[pos]);

	size /= 2;
	keys.resize(size + 1);
	pointers.resize(size + 1);

	return node;
}

void BPTreeNode::Remove(int pos)
{
	if (pos > size || pos <= 0)
	{
		cerr << "Remove node error: Position" << pos << " out of range." << endl;
		return;
	}

	dirty = true;
	if (pos == size)
	{
		keys.pop_back();
		pointers.pop_back();
	}
	else
	{
		keys.erase(keys.begin() + pos);
		pointers.erase(pointers.begin() + pos);
	}
	size--;
}

void BPTreeNode::SetRemoved()
{
	blockRemoved = true;
}

const char *BPTreeNode::Borrow(BPTreeNode *sibling, bool leftSibling, const char *parentKey)
{
	dirty = true;

	if (leftSibling)
	{
		// Borrow from left sibling
		int sibSize = sibling->GetSize();
		const char *sibKey = sibling->GetKey(sibSize);
		int sibPtr = sibling->GetPointer(sibSize);
		sibling->Remove(sibSize);

		if (leaf)
			Insert(0, sibKey, sibPtr);
		else
		{
			int ptr = pointers[0];
			pointers[0] = sibPtr;
			Insert(0, parentKey, ptr);
		}

		return sibKey;
	}
	else
	{
		// Borrow from right sibling
		const char* sibKey = sibling->GetKey(1);
		int sibPtr0 = sibling->GetPointer(0);
		int sibPtr1 = sibling->GetPointer(1);
		sibling->Remove(1);

		if (leaf)
		{
			Insert(size, sibKey, sibPtr1);
			return sibling->GetKey(1);
		}
		else
		{
			sibling->SetPointer(0, sibPtr1);
			Insert(size, parentKey, sibPtr0);
			return sibKey;
		}
	}
}

void BPTreeNode::MergeRight(BPTreeNode *sibling, const char * parentKey)
{
	dirty = true;
	int sibSize = sibling->GetSize();
	if (!leaf)
		Insert(size, parentKey, sibling->GetPointer(0));
	for (int i = 1; i <= sibSize; i++)
		Insert(size, sibling->GetKey(i), sibling->GetPointer(i));
}

void BPTreeNode::SetKey(int pos, const char * key)
{
	if (pos > size || pos <= 0)
	{
		cerr << "Set key error: Position" << pos << " out of range." << endl;
		return;
	}

	dirty = true;
	memcpy(keys[pos], key, keyLength);
}

void BPTreeNode::SetPointer(int pos, int pointer)
{
	if (pos > size || pos < 0)
	{
		cerr << "Set pointer error: Position" << pos << " out of range." << endl;
		return;
	}

	dirty = true;
	pointers[pos] = pointer;
}
