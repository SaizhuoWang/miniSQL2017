#ifndef CONDITION_H
#define CONDITION_H
#include "stdafx.h"
#include <sstream>
#include <string>
#include "MDef.h"

using namespace std;

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

template <typename T> 
bool mycompare(T orign, T compared, OPERATOR op)
{
	switch (op)
	{
		case OPERATOR_MORE:
			return orign > compared;
			break;
		case OPERATOR_LESS:
			return orign < compared;
			break;
		case OPERATOR_EQUAL:
			return orign == compared;
			break;
		case OPERATOR_NOT_EQUAL:
			return orign != compared;
			break;
		case OPERATOR_MORE_EQUAL:
			return orign >= compared;
			break;
		case OPERATOR_LESS_EQUAL:
			return orign <= compared;
			break;
		default:
			return false;
			break;
	}
}

Condition::Condition(string a, string v, OPERATOR op)
{
	attributeName = a;
	value = v;
	operate = op;
}

bool Condition::isright(string compared, int type)
{
	stringstream ss;
	int int_o, int_c;
	float float_o, float_c;
	string string_o, string_c;
	ss << value << " " << compared;
	switch (type)
	{
		case INT:
			ss >> int_o >> int_c;
			return mycompare(int_o, int_c, operate);
			break;
		case REAL:
			ss >> float_o >>float_c;
			return mycompare(float_o, float_c, operate);
			break;
		case CHAR:
			ss >> string_o >> string_c;
			return mycompare(string_o, string_c, operate);
			break;
		default:
			return false;
			break;
	}
}
#endif