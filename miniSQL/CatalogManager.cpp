#include "stdafx.h"
#include "CatalogManager.h"

#pragma warning(disable : 4996)

Byte * CatalogManager::WriteAttribute(Byte * dest, const Attribute & a)
{

	return nullptr;
}

bool CatalogManager::create_table(const string & table_name, const vector<Attribute>& attr, int primary_key_index)
{
	string filename(table_name + ".tlog");
	bm.CreateFile(filename);
	Block temp;
	strcpy(temp.file_name, filename.c_str());
	temp.tag = 0;
	Byte* ptr = temp.content;
	memcpy(ptr, table_name.c_str(), strlen(table_name.c_str())+1);
	ptr += strlen(table_name.c_str()) + 1;
	int attr_cnt = attr.size();
	memcpy(ptr, &attr_cnt, sizeof(int));
	ptr += sizeof(int);
	memcpy(ptr, &primary_key_index, sizeof(int));
	ptr += sizeof(int);
	for (auto &atr : attr)
	{
		memcpy(ptr, &atr.type, sizeof atr.type);
		ptr += sizeof atr.type;
		memcpy(ptr, atr.name.c_str(), strlen(atr.name.c_str()) + 1);
		ptr += SEGMENT_LENGTH;
		memcpy(ptr, &atr.is_unique, sizeof(bool));
		ptr += sizeof(bool);
		memcpy(ptr, &atr.length, sizeof atr.length);
		ptr += sizeof atr.length;
	}
	bm.WriteBlock(temp);
	return true;
}

bool CatalogManager::create_index(const string & iname, const string & tname, const string & aname)
{
	string filename(iname + ".ilog");
	Block temp;
	bm.CreateFile(filename);
	strcpy(temp.file_name, filename.c_str());
	temp.tag = 0;
	Byte* ptr = temp.content;
	memcpy(ptr, iname.c_str(), strlen(iname.c_str()) + 1);
	ptr += SEGMENT_LENGTH;
	memcpy(ptr, tname.c_str(), strlen(tname.c_str()) + 1);
	ptr += SEGMENT_LENGTH;
	memcpy(ptr, aname.c_str(), strlen(aname.c_str()) + 1);
	ptr += SEGMENT_LENGTH;
	return true;
}

bool CatalogManager::find_table(const string & table_name)
{
	string filename(table_name + ".tlog");
	FILE* fp = fopen(filename.c_str(), "r+");
	if (!fp) 
		return false;
	else return true;
}

bool CatalogManager::find_index(const string & index_name)
{
	string filename(index_name + ".ilog");
	FILE* fp = fopen(filename.c_str(), "r+");
	if (!fp)
		return false;
	else return true;
}