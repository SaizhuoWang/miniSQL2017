#pragma once
#include "stdafx.h"
#include "mDef.h"

using namespace std;

class BMException
{
private:
	string err_message;
public:
	BMException(const string& message):err_message(message){}
	BMException();
	~BMException();
	void show() { cout << err_message << endl; }
};

class BufferManager
{
private:
	Block *buffer;
	string current_file_name;
	bool opening_a_file;
	list<int> sub_que;
	FILE* fp;
	int substitute(const Byte* b,const string& name, int offset);
	void WriteToDisk(const Block& b);
	bool hit(const string& name, int offset, Block b);
	bool hit(const Block& b1, const Block& b2);
public:
	BufferManager();
	~BufferManager();
	Block FetchBlock(const string& name, int offset);
	void WriteBlock(const string& name, int offset, Block b);
	void SetPin(int index);
	void ResetPin(int index);
	void CreateFile(const string& name);
	void DeleteFile(const string& name);
};