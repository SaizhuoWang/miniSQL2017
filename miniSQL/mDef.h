#pragma once
#include "stdafx.h"

using namespace std;

#define BLOCK_SIZE 4096
#define BUFFER_SIZE 30
#define MAX_FILENAME_LENGTH 40

typedef unsigned char Byte;

enum Type{INT, REAL, CHAR};

class Attribute
{
public:
	Type type;
	string name;
	bool is_unique;
};

class Index
{
public:
	string table_name;
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
	Byte content[BLOCK_SIZE];
	bool dirty;
	char* file_name;
	int tag;
	bool pin;
	Block& operator=(const Block& b);
};