#pragma once
#include "stdafx.h"

#define BLOCK_SIZE 4096
#define BUFFER_SIZE 30
#define MAX_FILENAME_LENGTH 40

typedef unsigned char Byte;

class Block
{
public:
	Byte content[BLOCK_SIZE];
	bool dirty;
	char* file_name;
	int tag;
	bool pin;
	Block& operator=(const Block& b);
};