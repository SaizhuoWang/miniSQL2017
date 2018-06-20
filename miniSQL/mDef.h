#pragma once
#include "stdafx.h"

using namespace std;

#define BLOCK_SIZE 4096
#define BLOCK_A_SIZE 5000
#define BUFFER_SIZE 30
#define MAX_FILENAME_LENGTH 40
#define SEGMENT_LENGTH 33

typedef unsigned char Byte;

class Attribute
{
public:
	int type;//0 for int, 1 for float, -n for char(n)
	string name;//The name of this attribute
	bool primary;
	bool unique;
	Attribute() {};
	Attribute(int ty, string name, bool unique, bool primary) :
		type(ty), name(name), unique(unique), primary(primary) {};
};

class Restriction {
public:
	Restriction();
	int aname;
	//its index
	int op;
	//0 for '=', 1 for '<>', 2 for '<', 3 for '>', 4 for '<=', 5 for '>=
	string value;
};

class Index
{
public:
	Index() {};
	Index(const string& iname, const string& tname, const Attribute& attr) :
		name(iname), table_name(tname), attr(attr) {};
	string table_name, name;
	Attribute attr;
};

class Table
{
public:
	Table(const string& name, int pki, const vector<Attribute>& attrs) :
		name(name), primary_key_index(pki), attributes(attrs) {};
	Table() {};
	string name;
	int primary_key_index;
	vector<Attribute> attributes;
	vector<Index> indices;
};

class Block
{
public:
	Block();
	Block(const string& s);
	Byte content[BLOCK_SIZE];
	bool dirty;
	char* file_name;
	int tag;
	bool pin;
	Block& operator=(const Block& b);
	int byte_used;
};