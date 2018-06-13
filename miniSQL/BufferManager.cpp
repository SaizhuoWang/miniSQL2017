#include "stdafx.h"
#include "BufferManager.h"
#include "mDef.h"

#pragma warning(disable : 4996)

BufferManager::BufferManager()
{
	buffer = new Block[BUFFER_SIZE];//Create the space for buffer
	for (int i = 0; i < BUFFER_SIZE; i++)//Initialize the substitution table.
	{
		sub_que.push_back(i);
	}
	this->current_file_name = "";//Init table name.
	this->opening_a_file = false;//Init opening state.
}

BufferManager::~BufferManager()
{
	for (int i = 0; i < BUFFER_SIZE; i++)//Test if there's need to write back before deconstruction.
	{
		if (buffer[i].dirty)
			WriteToDisk(buffer[i]);
	}
	delete[] buffer;
}

bool BufferManager::hit(const string& name, int offset, const Block& b)
{
	//Return true if both name and address match.
	return (!strcmp(name.c_str(), b.file_name) && (b.tag/BLOCK_SIZE == offset/BLOCK_SIZE));
}

Block BufferManager::FetchBlock(const string& name, int offset)
{
	//File name exceeding limit will lead to exception.
	if (name.size() > MAX_FILENAME_LENGTH)
	{
		throw BMException("File name exceeds limit!");
	}
	/*First, check in the current buffer if the desired block is already in. If so, just return its address.*/
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		if (hit(name, offset, buffer[i]))
		{
			sub_que.pop_front();
			sub_que.push_back(i);
			return buffer[i];
		}
	}
	/*If not hit, fetch this block from the disk.*/
	//Step 1: Set the file pointer to the file we want.
	if (current_file_name != name || !opening_a_file)
	{
		fp = fopen(name.c_str(), "r+");
		current_file_name = name;
		opening_a_file = true;
	}
	//Step 2: Read the block from the file.
	Byte temp[BLOCK_SIZE];
	fseek(fp, offset, SEEK_SET);
	fread(&temp, 1, BLOCK_SIZE, fp);
	//Step 3: Put it to the buffer.
	int index = substitute(temp, name, offset);
	//Step 4: Return it.
	return buffer[index];
}

void BufferManager::WriteBlock(const Block& b)
{
	//If this block is already in, just overwrite it.
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		if (hit(b.file_name,b.tag,buffer[i]))
		{
			buffer[i] = b;
			buffer[i].dirty = true;
			return;
		}
	}
	//If not, substitute one using LRU
	int i = substitute(b.content, b.file_name, b.tag);
	buffer[i].dirty = true;
	return;
}

void BufferManager::SetPin(int index)
{
	buffer[index].pin = true;
}

void BufferManager::ResetPin(int index)
{
	buffer[index].pin = false;
}

void BufferManager::CreateFile(const string & name)
{
	//Test if file name exceeds limit.
	if (name.size() > MAX_FILENAME_LENGTH)
		throw BMException("File name exceeds length limit!");
	//Create a file
	opening_a_file = true;
	current_file_name = name;
	fp = fopen(name.c_str(), "w");
	return;
}

void BufferManager::DeleteFile(const string & name)
{
	//Test if file name exceeds limit.
	if (name.size() > MAX_FILENAME_LENGTH)
		throw BMException("File name exceeds length limit!");
	remove(name.c_str());
	return;
}

/*This function puts a block into the buffer. It does the substitution work if the block is full.
@param const Byte*: the Block content
@return value: the index in buffer of the newly arranged block
*/
int BufferManager::substitute(const Byte * b, const string& name, int offset)
{
	int it = 0, pos = sub_que.front();
	//First we check if there's room available using LRU.
	while (it < sub_que.size() && buffer[pos].pin)
	{
		sub_que.pop_front();
		sub_que.push_back(pos);
		pos = sub_que.front();
		it++;
	}
	//If there is no room, an exception is thrown.
	if (it == sub_que.size())
	{
		throw BMException("No buffer room available!");
	}
	//If the block to be substituted is dirty, we have to write it to the disk.
	if (buffer[pos].dirty)
		WriteToDisk(buffer[pos]);
	//Substitute the old block with the new one.
	memcpy(buffer[pos].content, b, BLOCK_SIZE);
	strcpy(buffer[pos].file_name, name.c_str());
	buffer[pos].dirty = false;
	buffer[pos].pin = false;
	buffer[pos].tag = offset;
	sub_que.pop_front();
	sub_que.push_back(pos);
	return pos;
}

void BufferManager::WriteToDisk(const Block & b)
{
	fp = fopen(b.file_name, "r+");
	current_file_name = b.file_name;
	opening_a_file = true;
	fseek(fp, b.tag / BLOCK_SIZE, SEEK_SET);
	fwrite(b.content, 1, BLOCK_SIZE, fp);
	return;
}