#pragma once

#include "mDef.h"

using namespace std;

class API;
class CatalogManager;
class Interpreter
{
public:
	API * ap;
	CatalogManager *cm;

	Interpreter();
	~Interpreter();

	bool syntax();

	template <class T>
	void read(T& rin);

	string gword(string& s,const string& mark);
	bool ctype(int t,string& s);
	int check_attr(const vector<Attribute>* ta, const string& iname);

private:
	string s;
};

