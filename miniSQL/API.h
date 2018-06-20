#pragma once
#include "mDef.h"
using namespace std;

class RecordManager;
class IndexManager;
class API
{
public:
	Interpreter * ip;
	RecordManager * rm;
	IndexManager * im;
	CatalogManager * cm;

	API();
	~API();

	void create_table(const string& tname, const vector<Attribute>* ats);
	void create_index(const string& iname, const string& tname, const string& aname);
	void drop_table(const string& tname);
	void drop_index(const string& iname);
	void select(const string& tname, const vector<Restriction> * res);
	void ddelete(const string& tname, const vector<Restriction> * res);
	void insert(const string& tname, const string& value);
};

