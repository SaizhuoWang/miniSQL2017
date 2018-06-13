#include "stdafx.h"
#include "mDef.h"

Block & Block::operator=(const Block & b)
{
	memcpy(this->content, b.content, BLOCK_SIZE);
	this->tag = b.tag;
	this->dirty = b.dirty;
	this->file_name = b.file_name;
	this->pin = b.pin;
	return *this;
}