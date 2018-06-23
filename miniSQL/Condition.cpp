#include "stdafx.h"
#include "Condition.h"

template <typename T>
bool mycompare(T orign, T compared, OPERATOR op)
{
	switch (op)
	{
	case OPERATOR_MORE:
		return orign < compared;
		break;
	case OPERATOR_LESS:
		return orign > compared;
		break;
	case OPERATOR_EQUAL:
		return orign == compared;
		break;
	case OPERATOR_NOT_EQUAL:
		return orign != compared;
		break;
	case OPERATOR_MORE_EQUAL:
		return orign <= compared;
		break;
	case OPERATOR_LESS_EQUAL:
		return orign >= compared;
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
	case 0:
		ss >> int_o >> int_c;
		return mycompare(int_o, int_c, operate);
		break;
	case 1:
		ss >> float_o >> float_c;
		return mycompare(float_o, float_c, operate);
		break;
	default:
		ss >> string_o >> string_c;
		return mycompare(string_o, string_c, operate);
		break;
	}
}