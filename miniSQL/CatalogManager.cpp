#include "stdafx.h"
#include "utils.h"
#include "Widget.h"
#pragma warning(disable : 4996)

void CatalogManager::SetBM(BufferManager * bm)
{
	this->bm = bm;
}

CatalogManager::CatalogManager()
{
	bm = NULL;
	this->indices = new map<string, Index*>;
	this->tables = new map<string, Table*>;
	this->bm = Utils::GetBufferManager();
}

CatalogManager::CatalogManager(BufferManager & bfm)
{
	bm = &bfm;
	this->indices = new map<string, Index*>;
	this->tables = new map<string, Table*>;
}

CatalogManager::~CatalogManager()
{
	for (auto &i : *this->tables)
	{
		delete i.second;
	}
	for (auto &i : *this->indices)
	{
		delete i.second;
	}
	delete tables;
	delete indices;
}

Byte * CatalogManager::ReadIndexFromBytePtr(Index *idx, Byte * ptr)
{
	idx->table_name = (char*)ptr;
	ptr += strlen((char*)ptr) + 1;
	idx->name = (char*)ptr;
	ptr += strlen((char*)ptr) + 1;
	ptr = ReadAttributeFromBytePtr(idx->attr, ptr);
	return ptr;
}

Byte* CatalogManager::ReadAttributeFromBytePtr(Attribute *attr, Byte* ptr)
{
	attr->name = (char*)ptr;
	ptr += strlen((char*)ptr) + 1;
	attr->type = *(int*)ptr;
	ptr += sizeof(int);
	attr->unique = *(bool*)ptr;
	ptr += sizeof(bool);
	attr->primary = *(bool*)ptr;
	ptr += sizeof(bool);
	return ptr;
}

Byte * CatalogManager::WriteAttributeToBytePtr(Byte * dest, const Attribute & src)
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

Byte * CatalogManager::WriteIndexToBytePtr(Byte * dest, const Index & src)
{
	memcpy(dest, src.table_name.c_str(), src.table_name.size() + 1);
	dest += src.table_name.size() + 1;
	memcpy(dest, src.name.c_str(), src.name.size() + 1);
	dest += src.name.size() + 1;
	dest = WriteAttributeToBytePtr(dest, *(src.attr));
	return dest;
}

void CatalogManager::WriteTableToCatalogFile(const Table & t)
{
	Block b;
	string filename(t.name + ".tlog");
	bm->CreateFile(filename);
	Byte* b_ptr = b.content;
	//Link the table catalog filename and offset to the block.
	string f_name(t.name + ".tlog");
	strcpy(b.file_name, f_name.c_str());
	b.tag = 0;
	//Table_name
	memcpy(b_ptr, t.name.c_str(), t.name.size() + 1);
	b_ptr += t.name.size() + 1;
	//Attr_cnt,pk_idx,idx_cnt
	int attr_cnt = t.attributes->size(), pk_idx = t.primary_key_index, idx_cnt = t.indices->size();
	memcpy(b_ptr, &attr_cnt, sizeof(int));
	b_ptr += sizeof(int);
	memcpy(b_ptr, &pk_idx, sizeof(int));
	b_ptr += sizeof(int);
	memcpy(b_ptr, &idx_cnt, sizeof(int));
	b_ptr += sizeof(int);
	//Attributes
	for (auto it = t.attributes->begin(); it != t.attributes->end(); it++)
	{
		b_ptr = WriteAttributeToBytePtr(b_ptr, *it);
	}
	//Indices
	for (auto it = t.indices->begin(); it != t.indices->end(); it++)
	{
		b_ptr = WriteIndexToBytePtr(b_ptr, *it);
	}
	b.byte_used = b_ptr - b.content;
	bm->WriteBlock(b);
}

void CatalogManager::ReadTableFromCatalogFile(Table *t, Block & b)
{
	Byte* ptr = b.content;
	t->name = (char*)ptr;
	ptr += strlen((char*)ptr) + 1;
	int atr_cnt, pk_idx, idx_cnt;
	atr_cnt = *(int*)ptr;
	ptr += sizeof(int);
	pk_idx = *(int*)ptr;
	ptr += sizeof(int);
	idx_cnt = *(int*)ptr;
	ptr += sizeof(int);
	t->primary_key_index = pk_idx;
	for (int i = 0; i < atr_cnt; i++)
	{
		Attribute a;
		ptr = ReadAttributeFromBytePtr(&a, ptr);
		t->attributes->push_back(a);
	}
	for (int i = 0; i < idx_cnt; i++)
	{
		Index idx;
		ptr = ReadIndexFromBytePtr(&idx, ptr);
		t->indices->push_back(idx);
	}
}

void CatalogManager::WriteIndexToCatalogFile(const Index & idx)
{
	Block b;
	string filename(idx.name + ".ilog");
	bm->CreateFile(filename);
	strcpy(b.file_name, filename.c_str());
	b.tag = 0;
	Byte* b_ptr = b.content;
	b_ptr = WriteIndexToBytePtr(b_ptr, idx);
	b.byte_used = b_ptr - b.content;
	bm->WriteBlock(b);
}

bool CatalogManager::create_table(const string & table_name, const vector<Attribute>* attri)
{
	if (tables->find(table_name) != tables->end())
		return false;
	Table *tbl = new Table(table_name, *attri);//Create a table object.
	Index *idx = new Index(table_name, table_name, tbl->attributes->at(tbl->primary_key_index));
	tbl->indices->push_back(*idx);//Push the automatically create index on primary key attribute to the index vector.
	WriteIndexToCatalogFile(*idx);//Write the index catalog back to the buffer.
	indices->insert(pair <string, Index*>(idx->name, idx));
	tables->insert(pair<string, Table*>(table_name, tbl));
	WriteTableToCatalogFile(*tbl);//Write the table back to the buffer.
	return true;
}

bool CatalogManager::create_index(const string & iname, const string & tname, const string & aname)
{
	if (!find_table(tname))//Wrong table name.
		return false;
	if (tables->find(tname) != tables->end())//If the table is in the buffer.
	{
		Table *cur_table = tables->at(tname);
		int atr_idx = FindAttribute(*cur_table, aname);
		if (atr_idx == -1)
			return false;
		if (!cur_table->attributes->at(atr_idx).unique) return false;
		Index *idx = new Index(iname, tname, cur_table->attributes->at(atr_idx));
		tables->at(tname)->indices->push_back(*idx);
		indices->insert(pair<string, Index*>(idx->name, idx));
		WriteIndexToCatalogFile(*idx);
		WriteTableToCatalogFile(*(tables->at(tname)));
	}
	else//The table is not in the buffer.
	{
		Table *cur_table = new Table;
		Block b = *(bm->FetchBlock(tname + ".tlog", 0));
		ReadTableFromCatalogFile(cur_table, b);
		int atr_idx = FindAttribute(*cur_table, aname);
		if (atr_idx == -1)
			return false;
		Index *idx = new Index(iname, tname, cur_table->attributes->at(atr_idx));
		cur_table->indices->push_back(*idx);
		WriteIndexToCatalogFile(*idx);
		tables->insert(pair<string, Table*>(cur_table->name, cur_table));
		WriteTableToCatalogFile(*cur_table);
	}
	return true;
}

bool CatalogManager::find_table(const string & table_name)
{
	string filename(table_name + ".tlog");
	FILE* fp = fopen(filename.c_str(), "r");
	if (!fp)
	{
		return false;
	}
	fclose(fp);
	if (bm->FileSize(filename.c_str()) == 0 && (tables->find(table_name) == tables->end()))
		return false;
	return true;
}

bool CatalogManager::find_index(const string & index_name)
{
	string filename(index_name + ".ilog");
	FILE* fp = fopen(filename.c_str(), "r");
	if (!fp)
	{
		return false;
	}
	fclose(fp);
	if (bm->FileSize(filename) == 0 && (indices->find(index_name) == indices->end()))
		return false;
	return true;
}

int CatalogManager::FindAttribute(const Table & t, const string &aname)
{
	for (int i = 0; i < (int)t.attributes->size(); i++)
	{
		if (t.attributes->at(i).name == aname) return i;
	}
	return -1;
}

vector<Attribute>* CatalogManager::attr(const string & tname)
{
	if (!find_table(tname))//Wrong table name.
		throw CMException("Creating index error: Table does not exist!");
	if (tables->find(tname) != tables->end())//If the table is in the buffer.
	{
		return tables->at(tname)->attributes;
	}
	else//The table is not in the buffer.
	{
		Table *cur_table = new Table;
		Block *b = bm->FetchBlock(tname + ".tlog", 0);
		ReadTableFromCatalogFile(cur_table, *b);
		tables->insert(pair<string, Table*>(cur_table->name, cur_table));
		return cur_table->attributes;
	}
}

vector<Index>* CatalogManager::idx(const string & tname)
{
	if (!find_table(tname))//Wrong table name.
		throw CMException("Creating index error: Table does not exist!");
	if (tables->find(tname) != tables->end())//If the table is in the buffer.
	{
		return tables->at(tname)->indices;
	}
	else//The table is not in the buffer.
	{
		Table *cur_table = new Table;
		Block b = *(bm->FetchBlock(tname + ".tlog", 0));
		ReadTableFromCatalogFile(cur_table, b);
		tables->insert(pair<string, Table*>(cur_table->name, cur_table));
		return cur_table->indices;
	}
}

bool CatalogManager::drop_table(const string & table_name)
{
	if (!find_table(table_name))//Wrong table name.
		return false;
	Table *this_table;
	if (tables->find(table_name) != tables->end())
	{
		this_table = tables->at(table_name);
	}
	else
	{
		Block b = *(bm->FetchBlock(table_name + ".tlog", 0));
		this_table = new Table;
		ReadTableFromCatalogFile(this_table, b);
		tables->insert(pair<string, Table*>(table_name, this_table));
	}
	for (int i = 0; i < int(this_table->indices->size()); i++)
	{
		bm->DeleteFile(this_table->indices->at(i).name + ".ilog");
	}
	tables->erase(table_name);
	bm->DeleteFile(table_name + ".tlog");
	return true;
}

bool CatalogManager::drop_index(const string & index_name)
{
	if (!find_index(index_name)) return false;
	Block *idx_blk = bm->FetchBlock(index_name + ".ilog", 0);
	Index *idx = new Index;
	ReadIndexFromBytePtr(idx, idx_blk->content);
	Table *this_table;
	if (tables->find(idx->table_name) != tables->end())
	{
		this_table = tables->at(idx->table_name);
	}
	else
	{
		Block b = *(bm->FetchBlock(idx->table_name + ".tlog", 0));
		this_table = new Table;
		ReadTableFromCatalogFile(this_table, b);
		tables->insert(pair<string, Table*>(idx->table_name, this_table));
	}
	int ii = 0;
	/*for (auto i = this_table->indices->begin(); i != this_table->indices->end(); i++)
	{
		if (this_table->indices->at(ii).name == idx->name)
		{
			i=this_table->indices->erase(i);
			
			break;
		}
		ii++;
	}*/
	std::remove(this_table->indices->begin(), this_table->indices->end(), *idx);
	int i = this_table->indices->size() - 1;
	Attribute* tmp = new Attribute(*(this_table->indices->at(i).attr));
	this_table->indices->pop_back();
	this_table->indices->at(this_table->indices->size() - 1).attr = tmp;
	indices->erase(idx->name);
	WriteTableToCatalogFile(*this_table);
	bm->DeleteFile(index_name + ".ilog");
	return true;
}

Index * CatalogManager::get_index(const string & iname)
{
	Block *b = bm->FetchBlock(iname + ".ilog", 0);
	Index* idx = new Index;
	ReadIndexFromBytePtr(idx, b->content);
	indices->insert(pair<string, Index*>(idx->name, idx));
	return idx;
}