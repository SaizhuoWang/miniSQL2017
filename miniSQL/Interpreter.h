#pragma once

#include "mDef.h"

using namespace std;

class API;
class CatalogManager;
class Interpreter
{
public:
	Interpreter(API* api, CatalogManager * cmr);
	~Interpreter();

	bool syntax();
	//check if the syntax is correct

	template <class T>
	void read(T& rin);
	//read in instructions from different streams

	string gword(string& s,const string& mark);
	//get a word from a string and erase that word from the string
	bool ctype(int t,string& s);
	//check if a string matches its claimed type
	int check_attr(const vector<Attribute>* ta, const string& aname);
	//check if an attribute exists and return its type
private:
	string s;

	API * ap;
	CatalogManager *cm;
};

