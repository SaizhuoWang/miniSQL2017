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

template <typename T>
bool mycompare(T orign, T compared, OPERATOR op)
{
	switch (op)
	{
	case OPERATOR_MORE:
		return orign < compared;
		break;
	case OPERATOR_LESS:
		return orign > compared;
		break;
	case OPERATOR_EQUAL:
		return orign == compared;
		break;
	case OPERATOR_NOT_EQUAL:
		return orign != compared;
		break;
	case OPERATOR_MORE_EQUAL:
		return orign <= compared;
		break;
	case OPERATOR_LESS_EQUAL:
		return orign >= compared;
		break;
	default:
		return false;
		break;
	}
}

Condition::Condition(string a, string v, OPERATOR op)
{
	attributeName = a;
	value = v;
	operate = op;
}

bool Condition::isright(char* compared, int type)
{
	stringstream ss;
	int int_o, int_c;
	float float_o, float_c;
	string string_o, string_c;
	ss << value;
	switch (type)
	{
	case 0:
		ss >> int_o;
		int_c = *(int*)compared;
		return mycompare(int_o, int_c, operate);
		break;
	case 1:
		ss >> float_o;
		float_c = *(float*)compared;
		return mycompare(float_o, float_c, operate);
		break;
	default:
		string_o = value;
		string_c = compared;
		return mycompare(string_o, string_c, operate);
		break;
	}
}