#pragma once
#include "stdafx.h"



using namespace std;

#define BLOCK_SIZE 4096
#define BLOCK_A_SIZE 5000
#define BUFFER_SIZE 30
#define MAX_FILENAME_LENGTH 40
#define SEGMENT_LENGTH 33

typedef unsigned char Byte;

enum Type{INT, REAL, CHAR};

class Attribute
{
public:
	Type type;
	string name;
	bool is_unique;
	//length saves the length of attribute if it is char, else it is -1
	int length;
	//************************************************//
	//int type;
	//0 for 'int', 1 for 'float', -n for 'char(n)'
	//bool primary;
	//***********************************************//
	Attribute();
	Attribute(Type ty, string name, bool unique) :type(ty), name(name), is_unique(unique) {};
	Attribute(Type ty, string name, bool unique, int length) :
		type(ty), name(name), is_unique(unique), length(length) {};
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
	Index();
	Index(const string& iname, const string& tname, const Attribute& attr) :
		name(iname), table_name(tname), attr(attr) {};
	string table_name, name;
	Attribute attr;
};

class Table
{
public:
	string name;
	vector<Attribute> attributes, primary_key;
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