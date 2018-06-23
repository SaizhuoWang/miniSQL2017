#pragma once
#include "mDef.h"
#include "API.h"
#include "BufferManager.h"

class RecordManager
{
public:
	RecordManager() {};
	~RecordManager() {};

	BufferManager *bm;
	API *api;
	void create_table(string tableName);
	void delete_table(string tableName);

	bool insert_record(string tableName, char * record, int recordSize);
	string record_filename(string tableName);

	bool show_allrecord(string tableName, vector<string> * attributeNameVector, vector<Condition> * conditionVector);
	bool show_blockrecord(string tableName, Block & current_block, vector<string> * attributeNameVector, vector<Condition> * conditionVector, int recordSize);

	bool delete_allrecord(string tableName, vector<Condition> * conditionVector);
	bool delete_blockrecord(Block & current_block, vector<Condition> * conditionVector, string tableName, int block_offset);

	
private:
	bool record_fitcondition(char * recordpoint, int recordSize, vector<Attribute> * attributeVector, vector<Condition> * conditionVector);
	bool content_fitcondition(char * content, int type, Condition * condition);

	int left_blockspace(Block & current_block);
	char * get_recordpoint(Block & current_block, int offset);
	char * find_lastrecord(string tableName, int recordSize);
	int typeSizeGet(int type);
	void print_record(char * recordpoint, int recordSize, vector<Attribute> * attributeVector, vector<string> * attributeNameVector);
};
