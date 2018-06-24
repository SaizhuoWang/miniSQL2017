#pragma once

#include "mDef.h"

class API;
class CatalogManager;
class Interpreter
{
public:
	Interpreter(API* api, CatalogManager * cmr);
	~Interpreter();

	/*check if the syntax is correct
	@member param 's': the instruction to be checked*/
	bool syntax();
	
	/*read in instructions from different streams
	@param T& rin: a stream to read, here it can be 'istream' or 'ifstream' */
	template <class T>
	void read(T& rin);
	
	/*get a word from a string and erase that word from the string
	@param string& s: initial string which provides the word
	@param const string& mark: get a word from 's' until it reaches 'mark' */
	string gword(string& s,const string& mark);
	
	/*check if a string matches its claimed type
	@param const int& t: its claimed type
	@param string& s: the string to be judged */
	bool ctype(const int& t,string& s);
	
	/*transform a series of string into a fixed length
	@param char* dest: put the result at the address of 'dest' 
	@param const vector<string>* source: the source strings
	@param const vector<Attribute>* ats: the attributes of the table*/
	void value(char* dest, const vector<string>* source, const vector<Attribute>* ats);

	/*check if an attribute exists and return its type
	@param const vector<Attribute>* ta: the attributes of the table
	@param const string& aname: the name of attribute to be checked*/
	int check_attr(const vector<Attribute>* ta, const string& aname);
	
private:
	string s;

	API * ap;
	CatalogManager *cm;
};

