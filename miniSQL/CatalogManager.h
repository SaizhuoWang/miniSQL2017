#pragma once
#include "stdafx.h"
#include "mDef.h"

class CMException
{
private:
	string err_message;
public:
	CMException();
	~CMException();
	CMException(string message) :err_message(message) {};
};

class CatalogManager
{
private:
	BufferManager & bm;
	vector<Table> tables;
	vector<Index> indices;
	Byte* WriteAttribute(Byte* dest, const Attribute& a);
public:
	CatalogManager(BufferManager& bfm) :bm(bfm) {};
	CatalogManager();
	~CatalogManager() {};
	bool find_table(const string& table_name);
	bool find_index(const string& index_name);
	int attr_check(const vector<Attribute>& attr, const string& table);
	bool drop_table(const string& table_name);
	bool drop_index(const string& index_name);
	bool create_table(const string& table_name, const vector<Attribute>& attr, int primary_key_index);
	bool create_index(const string& iname, const string& tname, const string& aname);
};