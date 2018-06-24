#pragma once
#include "mDef.h"
#include "Interpreter.h"
using namespace std;

class RecordManager;
class IndexManager;
class API
{
public:
	API();
	~API();

	void create_table(const string& tname, const vector<Attribute>* ats);
	void create_index(const string& iname, const string& tname, const string& aname);
	void drop_table(const string& tname);
	void drop_index(const string& iname);
	void select(const string& tname, vector<Condition> * res);
	void ddelete(const string& tname, vector<Condition> * res);
	void insert(const string& tname, char* value, const int size);

	int get_recordSize(const string& tname);
	void attributeGet(const string& tname, vector<Attribute>* ats);
	void recordIndexDelete(const char* value, const string& tname);
	void recordAddIndex(const char* value, const string& tname, const int& offset);

	string gkey(const vector<Attribute>* ats, const string& idx, const char* value);

	Interpreter * ip;
	RecordManager * rm;
	IndexManager * im;
	CatalogManager * cm;
};

