#pragma once
#include<vector>

class BPTree
{
private:
	const char *filename;
	// Order of the tree
	int order;

	// Length of the key
	int keyLength;

	// Total number of the nodes
	int nodeCount;

	// Block id of the root
	int root;

	// First empty block id in the file
	int firstEmpty;

	char *key;
	int value;

	int find(int id);
	int add(int id);
	int remove(int id, int siblingId, bool hasLeftSibling, const char *parentKey);
	int getFirstEmpty();
	void removeBlock(int id);
	void updateHeader();

public:
	// Create a file for a B+ Tree
	static bool CreateFile(const char *filename, int keyLength);

	BPTree(const char *filename);
	~BPTree();

	// Find the value of certain key
	int Find(const char *key);

	// Add a node
	bool Add(const char *key, int value);

	// Remove a node
	bool Remove(const char *key);
};

