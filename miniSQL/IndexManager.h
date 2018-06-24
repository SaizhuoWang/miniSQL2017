#pragma once

class IndexManager
{
private:
	string getTreeFilename(const char *indexName);

public:

	/*
	Find key in index.
	@param const char *indexName: The given index name.
	@param const char *key: The given key.
	@return int: The record id.
	*/
	int Find(const char *indexName, const char *key);

	/*
	Insert key into index.
	@param const char *indexName: The given index name.
	@param const char *key: The given key.
	@param int value: The given value.
	@return bool: true if success.
	*/
	bool Insert(const char *indexName, const char *key, int value);

	/*
	Remove key from index.
	@param const char *indexName: The given index name.
	@param const char *key: The given key.
	@return bool: true if success.
	*/
	bool Remove(const char *indexName, const char *key);

	/*
	Create new index.
	@param const char *indexName: The given index name.
	@return bool: true if success.
	*/
	bool CreateIndex(const char *indexName);

	/*
	Drop index.
	@param const char *indexName: The given index name.
	@return bool: true if success.
	*/
	bool DropIndex(const char *indexName);
};
