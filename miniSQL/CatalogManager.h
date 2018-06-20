#pragma once
#include "stdafx.h"
#include "mDef.h"

class CMException
{
private:
	string err_message;
public:
	CMException();
	~CMException() {};
	CMException(string message) :err_message(message) {};
};

class CatalogManager
{
private:
	BufferManager & bm;
	vector<Table> tables;
	vector<Index> indices;
	map<string, int> table_records, index_records;
	Byte* ReadAttribute(Attribute &attr,Byte* ptr);
	Byte* ReadIndex(Index &idx, Byte* ptr);//Reading from ptr to obj dest
	Byte* WriteAttribute(Byte* dest, const Attribute& src);
	Byte* WriteIndex(Byte* dest, const Index& src);//Write from obj to dest ptr
	void WriteTableCatalog(const Table& t);
	void ReadTableCatalog(Table &t,Block &b);//R&W an table object to the buffer
	void WriteIndexCatalog(const Index &idx);
	int FindAttribute(const Table &t,const string &aname);
public:
	/*
		This constructor takes in a buffer manager and link it with the cm;
		@param BufferManager& bfm: The buffer manager to be linked.
	*/
	CatalogManager(BufferManager& bfm) :bm(bfm) {};
	CatalogManager();
	~CatalogManager() {};

	/*
		This function checks if a table of a given name exists.
		@param const string& table_name: The given table name.
		@return bool: true if exists, false otherwise.
	*/
	bool find_table(const string& table_name);
	/*
		The same usage as above, except that this is for index.
		@param const string& index_name: The given table name.
		@return bool: true if exists, false otherwise.
	*/
	bool find_index(const string& index_name);
	/*
		This function fetches the attribute vector of a table with given table name.
		It will throw exception if it finds that the table does not exist or some other error.
		@param const string& tname: The given table name.
		@return value: The pointer to the attribute vector.
	*/
	vector<Attribute>& attr(const string& tname);
	/*
		This function fetches the index vector of a table with given table name.
		It will throw exception if it finds that the table does not exist or some other error.
		@param const string& tname: The given table name.
		@return value: The pointer to the index vector.
	*/
	vector<Index>& idx(const string &tname);
	/*
		Drop a table with given table name
	*/
	bool drop_table(const string& table_name);
	/*
	Drop an index with given index name*/
	bool drop_index(const string& index_name);
	/*
		This function interface takes in the essential information to build a table and create a table.
	*/
	bool create_table(const string& table_name, const vector<Attribute>& attr, int primary_key_index);
	/*
		This function interface takes in the essential information to build an index and create an index.
	*/
	bool create_index(const string& iname, const string& tname, const string& aname);
};