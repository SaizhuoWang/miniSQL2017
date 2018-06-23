#pragma once
class BPTreeNode
{
private:
	string filename;
	int id;
	int size;
	int keyLength;
	bool leaf;
	bool dirty;
	bool blockRemoved;
	vector<char *> keys;
	vector<int> pointers;

public:
	BPTreeNode(const char* filename, int id, int keyLength);
	BPTreeNode(const char* filename, int id, int keyLength, bool leaf, int firstPointer);
	~BPTreeNode();

	int GetSize() const;
	bool IsLeaf() const;
	void SetRemoved();

	const char *GetKey(int pos) const;
	void SetKey(int pos, const char* key);
	int GetPointer(int pos) const;
	void SetPointer(int pos, int pointer);

	size_t FindPosition(const char *key) const;
	void Insert(int pos, const char* key, int pointer);
	void Remove(int pos);
	BPTreeNode *Split(int newId, char *newKey);
	const char* Borrow(BPTreeNode* sibling, bool leftSibling, const char* parentKey);
	void MergeRight(BPTreeNode* sibling, const char* parentKey);
};
