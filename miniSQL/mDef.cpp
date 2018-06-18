#include "stdafx.h"
#include "mDef.h"
#pragma warning(disable : 4996)
Block::Block()
{
	memset(content, 0, BLOCK_SIZE);
	file_name = new char[MAX_FILENAME_LENGTH];
	dirty = false;
	tag = -1;
	pin = false;
	byte_used = 0;
}

Block & Block::operator=(const Block & b)
{
	memcpy(this->content, b.content, BLOCK_SIZE);
	this->tag = b.tag;
	this->dirty = b.dirty;
	strcpy(this->file_name, b.file_name);
	this->pin = b.pin;
	this->byte_used = b.byte_used;
	return *this;
}

Block::Block(const string & s):byte_used(s.size()),dirty(false),tag(-1),pin(false)
{
	memcpy(content, s.c_str(), s.size());
	file_name = new char[MAX_FILENAME_LENGTH];
}

Restriction::Restriction():aname(0),op(0)
{

}