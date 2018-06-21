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

Block* BufferManager::FetchBlock(const string& name, int offset)
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
			return &buffer[i];
		}
	}
	/*If not hit, fetch this block from the disk.*/
	//Step 1: Set the file pointer to the file we want.
	if (current_file_name != name || !opening_a_file)
	{
		fp = fopen(name.c_str(), "ab+");
		current_file_name = name;
		opening_a_file = true;
	}
	//Step 2: Read the block from the file.
	Block temp;
	int pp = (offset / BLOCK_SIZE)*BLOCK_A_SIZE;
	fseek(fp, (offset / BLOCK_SIZE)*BLOCK_A_SIZE, SEEK_SET);
	fread(&temp.byte_used, sizeof(int), 1, fp);
	fseek(fp, sizeof(int), SEEK_CUR);
	fread(temp.content, 1, BLOCK_SIZE, fp);
	strcpy(temp.file_name, name.c_str());
	temp.tag = offset;
	//Step 3: Put it to the buffer.
	int index = substitute(temp);
	//Step 4: Return it.
	return &buffer[index];
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
	int i = substitute(b);
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
	fp = fopen(name.c_str(), "ab+");
	return;
}

void BufferManager::DeleteFile(const string & name)
{
	//Test if file name exceeds limit.
	if (name.size() > MAX_FILENAME_LENGTH)
		throw BMException("File name exceeds length limit!");
	fp = fopen(name.c_str(), "w");
	current_file_name = "";
	opening_a_file = false;
	int p=remove(name.c_str());
	int pp = errno;
	perror("delete failed:");
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		if (!strcmp(buffer[i].file_name, name.c_str()))
		{
			Block new_block;
			buffer[i] = new_block;
		}
	}
	return;
}

int BufferManager::FileSize(const string & filename)
{
	if (filename != current_file_name)
	{
		current_file_name = filename;
		opening_a_file = true;
		fp = fopen(current_file_name.c_str(), "ab+");
	}
	int length;
	fseek(fp, 0, SEEK_END);
	length = (int)ftell(fp);
	return length;
}

/*This function puts a block into the buffer. It does the substitution work if the block is full.
@param const Byte*: the Block content
@return value: the index in buffer of the newly arranged block
*/
int BufferManager::substitute(const Block& b)
{
	int it = 0, pos = sub_que.front();
	//First we check if there's room available using LRU.
	while (it < (int)sub_que.size() && buffer[pos].pin)
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
	buffer[pos] = b;

	sub_que.pop_front();
	sub_que.push_back(pos);
	return pos;
}

void BufferManager::WriteToDisk(const Block & b)
{
	fp = fopen(b.file_name, "ab+");
	current_file_name = b.file_name;
	opening_a_file = true;
	fseek(fp, (b.tag / BLOCK_SIZE)*BLOCK_A_SIZE, SEEK_SET);
	fwrite(&b.byte_used,sizeof(int),1,fp);
	fseek(fp, sizeof(int), SEEK_CUR);
	fwrite(b.content, 1, BLOCK_SIZE, fp);
	return;
}

void BufferManager::AppendRecord(const string & name, int offset, Byte * src, int length)
{
	int i, index;
	for (i = 0; i < BUFFER_SIZE; i++)
	{
		if (hit(name, offset, buffer[i])) break;
	}
	if (i == BUFFER_SIZE)
	{
		if (current_file_name != name || !opening_a_file)
		{
			fp = fopen(name.c_str(), "ab+");
			current_file_name = name;
			opening_a_file = true;
		}
		//Step 2: Read the block from the file.
		Block temp;
		fseek(fp, (offset / BLOCK_SIZE)*BLOCK_A_SIZE, SEEK_SET);
		fread(&temp.byte_used, sizeof(int), 1, fp);
		fseek(fp, sizeof(int), SEEK_CUR);
		fread(temp.content, 1, BLOCK_SIZE, fp);
		strcpy(temp.file_name, name.c_str());
		temp.tag = offset;
		//Step 3: Put it to the buffer.
		index = substitute(temp);
	}
	else
	{
		index = i;
	}
	if (buffer[index].byte_used + length > BLOCK_SIZE)
		throw(BMException("Block out of space!"));
	buffer[index].dirty = true;
	memcpy(buffer[index].content + buffer[index].byte_used, src, length);
	buffer[index].byte_used += length;
}