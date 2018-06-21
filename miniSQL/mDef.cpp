#include "stdafx.h"
#include "mDef.h"
#pragma warning(disable : 4996)
Block::Block()
{
	memset(content, 0, BLOCK_SIZE);
	file_name = new char[MAX_FILENAME_LENGTH];
	dirty = false;
	tag = -1;
	pin = false;
	byte_used = 0;
}

Block & Block::operator=(const Block & b)
{
	memcpy(this->content, b.content, BLOCK_SIZE);
	this->tag = b.tag;
	this->dirty = b.dirty;
	strcpy(this->file_name, b.file_name);
	this->pin = b.pin;
	this->byte_used = b.byte_used;
	return *this;
}

Block::Block(const string & s):byte_used(s.size()),dirty(false),tag(-1),pin(false)
{
	memcpy(content, s.c_str(), s.size());
	file_name = new char[MAX_FILENAME_LENGTH];
}

Restriction::Restriction():aname(0),op(0)
{

}

Attribute::Attribute(const Attribute & a)
{
	type = a.type;
	name = a.name;
	primary = a.primary;
	unique = a.unique;
}

Index::Index()
{
	attr = new Attribute;
}

Index::Index(const string & iname, const string & tname, const Attribute & attr)
{
	table_name = tname;
	name = iname;
	this->attr = new Attribute(attr);
}

Index::Index(const Index & idx)
{
	this->table_name = idx.table_name;
	this->name = idx.name;
	this->attr = new Attribute(*(idx.attr));
}

Index::~Index()
{
	delete attr;
}

Table::Table()
{
	primary_key_index = -1;
	attributes = new vector<Attribute>;
	indices = new vector<Index>;
}

Table::Table(const string & name, const vector<Attribute>& attrs, const vector<Index>& idices)
{
	this->name = name;
	this->primary_key_index = -1;
	for (int i = 0; i < (int)attrs.size(); i++)
	{
		if (attrs[i].primary)
		{
			primary_key_index = i;
			break;
		}
	}
	attributes = new vector<Attribute>(attrs);
	indices = new vector<Index>(idices);
}

Table::Table(const string & name, const vector<Attribute>& attrs)
{
	this->name = name;
	this->primary_key_index = -1;
	for (int i = 0; i < (int)attrs.size(); i++)
	{
		if (attrs[i].primary)
		{
			primary_key_index = i;
			break;
		}
	}
	attributes = new vector<Attribute>(attrs);
	indices = new vector<Index>;
}

Table::Table(const Table & tbl)
{
	name = tbl.name;
	primary_key_index = tbl.primary_key_index;
	attributes = new vector<Attribute>(*(tbl.attributes));
	indices = new vector<Index>(*(tbl.indices));
}

Table::~Table()
{
	delete attributes;
	delete indices;
}