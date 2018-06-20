#pragma once
#include "stdafx.h"
#include "mDef.h"

using namespace std;

/*BM is short for Buffer Manager. This exception class is thrown when there's any
runtime error happens in Buffer Manager. The error message is stored in the string member
"err_message".*/
class BMException
{
private:
	string err_message;
public:
	BMException(const string& message):err_message(message){}
	BMException();
	~BMException() {};
	void show() { cout << err_message << endl; }
};

/*Buffer Manager is responsible for the management of buffer. It has a inner memory space which is called
"buffer".*/
class BufferManager
{
private:
	//The buffer space
	Block *buffer;
	//The file name which is currently linked.
	string current_file_name;
	//Marking if BM is opening a file
	bool opening_a_file;
	//A list recording the order of use. This list is essential for LRU substitution strategy.
	list<int> sub_que;
	//File pointer to carry out read and write
	FILE* fp;
	/*
		This function carries out the substitution work. In general, this function does the work of putting a block
		into the memory.
		@param const Byte&*: The content of the block.
		@param const string& name: The file name of this block.
		@param int offset: The byte offset of the content.
		@return int: The index of the newly put block in the buffer.
	*/
	int substitute(const Block& b);
	/*
		This funtion carries out the work of writing a block into the disk.
		@param consty Block& b: The block to be written
	*/
	void WriteToDisk(const Block& b);
	/*
		This function judges if a given block is the wanted one, given the file name and byte offset.
		@param const string& name: The file name.
		@param int offset: The byte offset.
		@param const Block& b: The given block.
		@return bool: True if the block matches, false if not.
	*/
	bool hit(const string& name, int offset, const Block& b);
public:
	/*
		The default c'tor which initializes the contents in the buffer.
	*/
	BufferManager();
	/*
		The default d'tor. It contains writing all the necessary blocks back to the disk.
	*/
	~BufferManager();
	/*
		This function interface get the block of the given file name and byte offset.
		@param const string& name: The file name.
		@param int offset: The byte offset.
		@return Block: The wanted block.
	*/
	Block FetchBlock(const string& name, int offset);
	/*
		This function interface takes in a set of data, together with the exact address it is needed to be written.
		It then get the corresponding block to the buffer and then change it.
		However, it is not a write-through.
		@param const Block& b: The block to be written.
	*/
	void WriteBlock(const Block& b);
	/*
		This interface allows appending a piece of record onto an existing one.
	*/
	void AppendRecord(const string& name, int offset, Byte* src, int length);
	/*
		Pin and unpin a block
	*/
	void SetPin(int index);
	void ResetPin(int index);
	/*
		Create and delete a file.
	*/
	void CreateFile(const string& name);
	void DeleteFile(const string& name);
	/*
		Get the size of a file with given filename.
	*/
	int FileSize(const string& filename);
};