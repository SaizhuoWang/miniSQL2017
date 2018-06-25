#pragma once
#include "stdafx.h"
using namespace std;

#define BLOCK_SIZE 4096
#define BLOCK_A_SIZE 4100
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
public:
	Attribute() {};
	Attribute(int ty, string name, bool unique, bool primary) :
		type(ty), name(name), unique(unique), primary(primary) {};
	Attribute(const Attribute &a);
};

class Index
{
public:
	Index();
	Index(const string& iname, const string& tname, const Attribute& attr);
	Index(const Index& idx);
	~Index();
public:
	string table_name, name;
	Attribute *attr;
};

class Table
{
public:
	Table(const string& name, const vector<Attribute>& attrs, const vector<Index>& idices);
	Table(const string& name, const vector<Attribute>& attrs);
	Table(const Table& tbl);
	Table();
	~Table();
public:
	string name;
	int primary_key_index;
	vector<Attribute> *attributes;
	vector<Index> *indices;
};

class Block
{
public:
	Byte content[BLOCK_SIZE];
	bool dirty;
	char* file_name;
	int tag;
	bool pin;
	int byte_used;
public:
	Block & operator=(const Block& b);
	Block();
	Block(const string& s);
};

const int INT = 0;
const int REAL = 1;
const int CHAR = 2;


enum OPERATOR
{
	OPERATOR_MORE,
	OPERATOR_LESS,
	OPERATOR_EQUAL,
	OPERATOR_NOT_EQUAL,
	OPERATOR_MORE_EQUAL,
	OPERATOR_LESS_EQUAL,
	OPERAROR_UNKNOW
};

class Condition
{
public:
	Condition(string a, string v, OPERATOR op);
	string attributeName;
	string value;
	OPERATOR operate;

	bool isright(char* compared, int type);

};

struct index_return
{
	char *r_point;
	int r_offset;
};

typedef struct index_return index_param;