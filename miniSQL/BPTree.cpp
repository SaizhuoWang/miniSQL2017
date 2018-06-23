#include "stdafx.h"
#include "BPTree.h"
#include "utils.h"
#include "BPTreeNode.h"

#pragma warning(disable : 4996)
// Returns -1 when not found.
int BPTree::find(int id)
{
	if (id < 0) {
		return -1;
	}
	BPTreeNode *node = new BPTreeNode(filename, id, keyLength);
	int pos = node->FindPosition(key);

	int s = -1;
	if (node->IsLeaf())
	{
		if (pos > 0)
		{
			const char* k = node->GetKey(pos);
			if (memcmp(key, k, keyLength) == 0)
				s = node->GetPointer(pos);
		}
	}
	else
	{
		s = find(node->GetPointer(pos));
	}
	delete node;
	return s;
}

// Returns 1 to add new node, and -1 when failed.
int BPTree::add(int id)
{
	if (id < 0) {
		return 1;
	}
	BPTreeNode *node = new BPTreeNode(filename, id, keyLength);
	int pos = node->FindPosition(key);

	int s = node->IsLeaf() ? 1 : add(node->GetPointer(pos));

	if (node->IsLeaf() && pos > 0)
	{
		const char *k = node->GetKey(pos);
		if (memcmp(key, k, keyLength) == 0)
			s = -1;
	}

	if (s == 1)
	{
		node->Insert(pos, key, value);
		if (node->GetSize() >= order)
		{
			int newId = getFirstEmpty();
			BPTreeNode *newNode = node->Split(newId, key);
			value = newId;
			delete newNode;
		}
	}

	delete node;
	return s;
}

// Returns -1 when failed, 1 to remove, 2 to change key
int BPTree::remove(int id, int siblingId, bool isLeftSibling, const char *parentKey)
{
	if (id < 0) {
		return false;
	}
	BPTreeNode* node = new BPTreeNode(filename, id, keyLength);
	BPTreeNode* sibling = NULL;
	if (id != root)
		sibling = new BPTreeNode(filename, siblingId, keyLength);
	int pos = node->FindPosition(key);

	int res;
	if (node->IsLeaf())
		res = -1;
	else
	{
		int nxtId = node->GetPointer(pos);
		int nxtSib = node->GetPointer(pos > 0 ? pos - 1 : pos + 1);
		const char* nxtParentKey = node->GetKey(pos > 0 ? pos : pos + 1);
		res = remove(nxtId, nxtSib, pos > 0, nxtParentKey);
	}

	if (node->IsLeaf())
	{
		if (pos > 0)
		{
			const char* k = node->GetKey(pos);
			if (memcmp(key, k, keyLength) == 0)
				res = 1;
		}
	}

	int s = 0;
	if (res == -1)
		s = -1;
	else if (res == 1)
	{
		node->Remove(pos > 0 ? pos : pos + 1);

		if (id == root)
		{
			if (node->GetSize() == 0)
			{
				root = node->GetPointer(0);
				removeBlock(id);
				node->SetRemoved();
			}
		}
		else
		{
			int limit = (order + 1) / 2 - 1;
			if (node->GetSize() < limit)
			{
				if (sibling->GetSize() > limit)
				{
					const char* k = node->Borrow(sibling, isLeftSibling, parentKey);
					memcpy(key, k, keyLength);
					s = 2;
				}
				else
				{
					if (isLeftSibling)
					{
						sibling->MergeRight(node, parentKey);
						removeBlock(id);
						node->SetRemoved();
					}
					else
					{
						node->MergeRight(sibling, parentKey);
						removeBlock(siblingId);
						sibling->SetRemoved();
					}
					s = 1;
				}
			}
		}
	}
	else if (res == 2)
		node->SetKey(pos > 0 ? pos : pos + 1, key);

	delete node;
	if (sibling != NULL)
		delete sibling;
	return s;
}

int BPTree::getFirstEmpty()
{
	if (firstEmpty < 0)
		return ++nodeCount;

	int s = firstEmpty;
	BufferManager *bm = Utils::GetBufferManager();
	Block *block = bm->FetchBlock(filename, firstEmpty);
	firstEmpty = *(reinterpret_cast<int*>(block->content));
	return s;
}

void BPTree::removeBlock(int id)
{
	BufferManager *bm = Utils::GetBufferManager();
	Block *block = bm->FetchBlock(filename, id);
	memcpy(block->content, &firstEmpty, 4);
	firstEmpty = id;
}

void BPTree::updateHeader()
{
	BufferManager *bm = Utils::GetBufferManager();
	Block *block = bm->FetchBlock(filename, 0);

	memcpy(block->content + 8, &nodeCount, 4);
	memcpy(block->content + 12, &root, 4);
	memcpy(block->content + 16, &firstEmpty, 4);

	block->dirty = true;
}

bool BPTree::CreateFile(const char *filename, int keyLength)
{
	BufferManager *bm = Utils::GetBufferManager();
	FILE* file = fopen(filename, "wb");
	int header[] = { (BLOCK_SIZE - 8) / (keyLength + 4) + 1, keyLength, 0, -1, -1 };
	fwrite(header, 4, 5, file);
	fclose(file);
	return true;
}

BPTree::BPTree(const char *filename) :filename(filename)
{
	BufferManager *bm = Utils::GetBufferManager();
	Block *header = bm->FetchBlock(filename, 0);

	int *headerArray = reinterpret_cast<int *>(header->content);
	order = headerArray[0];
	keyLength = headerArray[1];
	nodeCount = headerArray[2];
	root = headerArray[3];
	firstEmpty = headerArray[4];

	key = new char[keyLength];
}


BPTree::~BPTree()
{
	delete[] key;
}

int BPTree::Find(const char *key)
{
	memcpy(this->key, key, keyLength);
	return find(root);
}

bool BPTree::Add(const char *key, int value)
{
	memcpy(this->key, key, keyLength);
	this->value = value;
	int t = add(root);

	if (t == 1)
	{
		int newRoot = getFirstEmpty();
		BPTreeNode* node = new BPTreeNode(filename, newRoot, keyLength, root < 0, root < 0 ? -1 : root);
		node->Insert(0, key, value);
		delete node;
		root = newRoot;
	}
	updateHeader();

	return t != -1;
}

bool BPTree::Remove(const char *key)
{
	memcpy(this->key, key, keyLength);
	int t = remove(root, 0, true, NULL);
	updateHeader();
	return t != -1;
}
