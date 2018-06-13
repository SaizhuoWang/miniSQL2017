#include "stdafx.h"
#include "mDef.h"

Block::Block()
{
	memset(content, 0, BLOCK_SIZE);
	file_name = new char[MAX_FILENAME_LENGTH];
	dirty = false;
	tag = -1;
	pin = false;
}

Block & Block::operator=(const Block & b)
{
	memcpy(this->content, b.content, BLOCK_SIZE);
	this->tag = b.tag;
	this->dirty = b.dirty;
	this->file_name = b.file_name;
	this->pin = b.pin;
	return *this;
}