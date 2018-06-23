#ifndef CONDITION_H
#define CONDITION_H
#include "stdafx.h"
#include <sstream>
#include <string>
#include "MDef.h"

using namespace std;
const int INT =  0;
const int REAL = 1;
const int CHAR = 2;


enum OPERATOR
{
	OPERATOR_MORE,
	OPERATOR_LESS,
	OPERATOR_EQUAL,
	OPERATOR_NOT_EQUAL,
	OPERATOR_MORE_EQUAL,
	OPERATOR_LESS_EQUAL,
	OPERAROR_UNKNOW
};

class Condition
{
public:
	Condition(string a, string v, OPERATOR op);
	string attributeName;
	string value;
	OPERATOR operate;

	bool isright(string compared, int type);
	
};
#endif