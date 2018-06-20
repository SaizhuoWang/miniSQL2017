#include "stdafx.h"
#include "CatalogManager.h"

#pragma warning(disable : 4996)

Byte * CatalogManager::ReadIndex(Index & idx, Byte * ptr)
{
	idx.table_name = (char*)ptr;
	ptr += strlen((char*)ptr) + 1;
	idx.name = (char*)ptr;
	ptr += strlen((char*)ptr) + 1;
	ptr = ReadAttribute(idx.attr, ptr);
	return ptr;
}

Byte* CatalogManager::ReadAttribute(Attribute &attr, Byte* ptr)
{
	string name = (char*)ptr;
	ptr += strlen((char*)ptr) + 1;
	int type = *(int*)ptr;
	ptr += sizeof(int);
	bool unique = *(bool*)ptr;
	ptr += sizeof(bool);
	bool primary = *(bool*)ptr;
	ptr += sizeof(bool);
	attr.name = name;
	attr.type = type;
	attr.unique = unique;
	attr.primary = primary;
	return ptr;
}

Byte * CatalogManager::WriteAttribute(Byte * dest, const Attribute & src)
{
	memcpy(dest, src.name.c_str(), src.name.size() + 1);
	dest += src.name.size() + 1;
	memcpy(dest, &src.type, sizeof(int));
	dest += sizeof(int);
	memcpy(dest, &src.unique, sizeof(bool));
	dest += sizeof(bool);
	memcpy(dest, &src.primary, sizeof(bool));
	dest += sizeof(bool);
	return dest;
}

Byte * CatalogManager::WriteIndex(Byte * dest, const Index & src)
{
	Attribute attr = src.attr;
	memcpy(dest, src.table_name.c_str(), src.table_name.size() + 1);
	dest += src.table_name.size() + 1;
	memcpy(dest, src.name.c_str(), src.name.size() + 1);
	dest += src.name.size() + 1;
	memcpy(dest, attr.name.c_str(), attr.name.size() + 1);
	dest += attr.name.size() + 1;
	memcpy(dest, &attr.type, sizeof(int));
	dest += sizeof(int);
	memcpy(dest, &attr.unique, sizeof(bool));
	dest += sizeof(bool);
	memcpy(dest, &attr.primary, sizeof(bool));
	dest += sizeof(bool);
	return dest;
}

void CatalogManager::WriteTableCatalog(const Table & t)
{
	Block b;
	string filename(t.name + ".tlog");
	bm.CreateFile(filename);
	Byte* b_ptr = b.content;
	//Link the table catalog filename and offset to the block.
	string f_name(t.name + ".tlog");
	strcpy(b.file_name, f_name.c_str());
	b.tag = 0;
	//Table_name
	memcpy(b_ptr, t.name.c_str(), t.name.size() + 1);
	b_ptr += t.name.size() + 1;
	//Attr_cnt,pk_idx,idx_cnt
	int attr_cnt = t.attributes.size(), pk_idx = t.primary_key_index, idx_cnt = t.indices.size();
	memcpy(b_ptr, &attr_cnt, sizeof(int));
	b_ptr += sizeof(int);
	memcpy(b_ptr, &pk_idx, sizeof(int));
	b_ptr += sizeof(int);
	memcpy(b_ptr, &idx_cnt, sizeof(int));
	b_ptr += sizeof(int);
	//Attributes
	for (int i = 0; i < attr_cnt; i++)
	{
		b_ptr = WriteAttribute(b_ptr, t.attributes[i]);
	}
	//Indices
	for (int i = 0; i < idx_cnt; i++)
	{
		b_ptr = WriteIndex(b_ptr, t.indices[i]);
	}
	b.byte_used = b_ptr - b.content;
	bm.WriteBlock(b);
}

void CatalogManager::ReadTableCatalog(Table & t,Block & b)
{
	Byte* ptr = b.content;
	t.name = (char*)ptr;
	ptr += strlen((char*)ptr) + 1;
	int atr_cnt, pk_idx, idx_cnt;
	atr_cnt = *(int*)ptr;
	ptr += sizeof(int);
	pk_idx= *(int*)ptr;
	ptr += sizeof(int);
	idx_cnt= *(int*)ptr;
	ptr += sizeof(int);
	t.primary_key_index = pk_idx;
	for (int i = 0; i < atr_cnt; i++)
	{
		Attribute a;
		ptr = ReadAttribute(a, ptr);
		t.attributes.push_back(a);
	}
	for (int i = 0; i < idx_cnt; i++)
	{
		Index idx;
		ptr = ReadIndex(idx, ptr);
		t.indices.push_back(idx);
	}
}

void CatalogManager::WriteIndexCatalog(const Index & idx)
{
	Block b;
	string filename(idx.name + ".ilog");
	bm.CreateFile(filename);
	strcpy(b.file_name, filename.c_str());
	b.tag = 0;
	Byte* b_ptr = b.content;
	b_ptr = WriteIndex(b_ptr, idx);
	b.byte_used = b_ptr - b.content;
	bm.WriteBlock(b);
}

bool CatalogManager::create_table(const string & table_name, const vector<Attribute>& attr, int primary_key_index)
{
	if (table_records.find(table_name) != table_records.end())
	{
		return false;
	}
	Table tbl(table_name, primary_key_index, attr);//Create a table object.
	Index idx(table_name, table_name, tbl.attributes[primary_key_index]);//Automatically create index on pk.
	tbl.indices.push_back(idx);//Push the automatically create index on primary key attribute to the index vector.
	index_records.insert(pair<string, int>(table_name, tbl.indices.size() - 1));//Register this index.
	//Write the category of the index into a unique file.
	WriteIndexCatalog(idx);//Write the index catalog back to the buffer.
	int pos = tables.size();
	tables.push_back(tbl);//Push the table object into table vector.
	table_records.insert(std::pair<string, int>(table_name, pos));//Register the table.
	WriteTableCatalog(tbl);//Write the table back to the buffer.
	return true;
}

bool CatalogManager::create_index(const string & iname, const string & tname, const string & aname)
{
	if (!find_table(tname))//Wrong table name.
		return false;
	if (table_records.find(tname) != table_records.end())//If the table is in the buffer.
	{
		Table cur_table = tables[table_records[tname]];
		int atr_idx = FindAttribute(cur_table, aname);
		if (atr_idx == -1)
			return false;
		Index idx(iname, tname, cur_table.attributes[atr_idx]);
		cur_table.indices.push_back(idx);
		WriteIndexCatalog(idx);
		WriteTableCatalog(cur_table);
	}
	else//The table is not in the buffer.
	{
		Table cur_table;
		Block b = bm.FetchBlock(tname + ".tlog", 0);
		ReadTableCatalog(cur_table, b);
		int pos = tables.size();
		tables.push_back(cur_table);
		table_records.insert(pair<string, int>(tname, pos));//Register the table
		int atr_idx = FindAttribute(cur_table, aname);
		if (atr_idx == -1)
			return false;
		Index idx(iname, tname, cur_table.attributes[atr_idx]);
		cur_table.indices.push_back(idx);
		WriteIndexCatalog(idx);
		WriteTableCatalog(cur_table);
	}
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

int CatalogManager::FindAttribute(const Table & t,const string &aname)
{
	for (int i = 0; i < (int)t.attributes.size(); i++)
	{
		if (t.attributes[i].name == aname) return i;
	}
	return -1;
}

vector<Attribute>& CatalogManager::attr(const string & tname)
{
	if (!find_table(tname))//Wrong table name.
		throw CMException("Creating index error: Table does not exist!");
	if (table_records.find(tname) != table_records.end())//If the table is in the buffer.
	{
		Table cur_table = tables[table_records[tname]];
		return tables[table_records[tname]].attributes;
	}
	else//The table is not in the buffer.
	{
		Table cur_table;
		Block b = bm.FetchBlock(tname + ".tlog", 0);
		ReadTableCatalog(cur_table, b);
		int pos = tables.size();
		tables.push_back(cur_table);
		table_records.insert(pair<string, int>(tname, pos));//Register the table
		return tables[pos].attributes;
	}
}

vector<Index>& CatalogManager::idx(const string & tname)
{
	if (!find_table(tname))//Wrong table name.
		throw CMException("Creating index error: Table does not exist!");
	if (table_records.find(tname) != table_records.end())//If the table is in the buffer.
	{
		return tables[table_records[tname]].indices;
	}
	else//The table is not in the buffer.
	{
		Table cur_table;
		Block b = bm.FetchBlock(tname + ".tlog", 0);
		ReadTableCatalog(cur_table, b);
		int pos = tables.size();
		tables.push_back(cur_table);
		table_records.insert(pair<string, int>(tname, pos));//Register the table
		return tables[pos].indices;
	}
}

bool CatalogManager::drop_table(const string & table_name)
{
	if (!find_table(table_name))//Wrong table name.
		return false;

	return true;
}

bool CatalogManager::drop_index(const string & index_name)
{
	Block idx_blk=bm.FetchBlock(index_name + ".ilog", 0);
	Index idx;
	ReadIndex(idx, idx_blk.content);
	
	return false;
}