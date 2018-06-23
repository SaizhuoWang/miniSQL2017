#pragma once
#include "mDef.h"
#include "Interpreter.h"
using namespace std;

class RecordManager;
class IndexManager;
class API
{
public:
    API(Interpreter* tip, RecordManager* trm, /*IndexManager* tim,*/ CatalogManager* tcm);
	~API();

	void create_table(const string& tname, const vector<Attribute>* ats);
	void create_index(const string& iname, const string& tname, const string& aname);
	void drop_table(const string& tname);
	void drop_index(const string& iname);
	void select(const string& tname, const vector<Condition> * res);
	void ddelete(const string& tname, const vector<Condition> * res);
	void insert(const string& tname, const string& value);

private:
	Interpreter * ip;
	RecordManager * rm;
	IndexManager * im;
	CatalogManager * cm;
};

