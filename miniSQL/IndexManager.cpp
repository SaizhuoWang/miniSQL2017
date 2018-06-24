#include "stdafx.h"
#include "utils.h"
#include "BPTree.h"
#include "IndexManager.h"
#pragma warning(disable : 4996)
string IndexManager::getTreeFilename(const char *indexName) {
	string s = string(indexName) + ".index";
	return s;
}

int IndexManager::Find(const char *indexName, const char *key)
{
	BPTree *tree = new BPTree(getTreeFilename(indexName).c_str());
	int ret = tree->Find(key);
	delete tree;
	return ret;
}

bool IndexManager::Insert(const char *indexName, const char *key, int value)
{
	BPTree *tree = new BPTree(getTreeFilename(indexName).c_str());
	if (!tree->Add(key, value))
	{
		delete tree;
		cerr << "Insert key error: Duplicated key in index `" << indexName << "`." << endl;
		return false;
	}
	delete tree;
	return true;
}

bool IndexManager::Remove(const char *indexName, const char *key)
{
	BPTree *tree = new BPTree(getTreeFilename(indexName).c_str());
	if (!tree->Remove(key))
	{
		delete tree;
		cerr << "Remove key error: Cannot find key in index `" << indexName << "`." << endl;
		return false;
	}
	delete tree;
	return true;
}

bool IndexManager::CreateIndex(const char *indexName)
{
	CatalogManager *catalogManager = Utils::GetCatalogManager();
	Index *index = catalogManager->get_index(indexName);
	if (index == NULL)
		return false;
	int keyLength = Utils::GetTypeSize(index->attr->type);
	return BPTree::CreateFile(getTreeFilename(indexName).c_str(), keyLength);
}

bool IndexManager::DropIndex(const char * indexName)
{
	BufferManager *bufferManager = Utils::GetBufferManager();
	bufferManager->DeleteFile(getTreeFilename(indexName));
	return true;
}
