#include "stdafx.h"
#include "Interpreter.h"
#include "CatalogManager.h"
#include "API.h"
#pragma warning(disable : 4996)

Interpreter::Interpreter()
{
}


Interpreter::~Interpreter()
{
}

void Interpreter::read()
{
	s.clear();
	char c;
	do {
		c = cin.get();
		if(c!='\n')s = s + c;
	} while (c != ';');
}

bool Interpreter::fread(ifstream& rin)
{   
	s.clear();
	char c;
	do {
		if (rin.eof())return false;
		c = rin.get();
		if(c!='\n') s = s + c;	
	} while (c != ';');
	return true;
}

string Interpreter::gword(string& s,const string& mark)
{
	int length = mark.length();
	int i;
	for (i = 0; i < s.length() - length; i++)
	{
		string tmp = s.substr(i, length);
		if (!strcmp(tmp.c_str(),mark.c_str())) break;
	}
	string t = s.substr(0, i);
	s.erase(0, i + mark.length());
	return t;
}

bool Interpreter::ctype(const int& t,string& ss)
{   
	int stype = 0;
	int len = ss.length();
	if (ss.find("\"") != string::npos)
	{
		stype = -(len - 2);
		ss.erase(0, 1);
		ss = ss.substr(0, ss.find_first_of("\""));
	}
	else if (ss.find(".") != string::npos)stype = 1;
	else stype = 0;

	if (stype >= 0)
		if (stype == t)return true;
		else return false;
	else
		if (stype >= t) {
			int offset = stype - t;
			while (offset--)ss = ss + " ";
			return true;
		}
		else return false;
}

void Interpreter::value(char* dest, const vector<string>* source, const vector<Attribute>* ats)
{
	stringstream ss;int a;float b;
	for (int i = 0;i < source->size();i++)
	{
		ss.clear();
		ss.str("");
		ss << (*source)[i];
		if ((*ats)[i].type == 0)
		{
			ss >> a;
			memcpy(dest, &a, sizeof(a));
			dest += sizeof(a);
			ss.clear();
		}
		else if ((*ats)[i].type == 1)
		{
			ss >> b;
			//printf("%X", *(int*)&b);
			memcpy(dest,&b,sizeof(float));
			dest += sizeof(b);
			ss.clear();
		}
		else {
			for (int j = 0; j < (*source)[i].length(); j++)
			{
				strncpy(dest, (char*)&(*source)[i][j], sizeof(char));
				dest++;
			}
		}
	}
}

int Interpreter::check_attr(const vector<Attribute>* ta, const string& aname)
{
	int i = 0;
	for (i = 0;i < (*ta).size();i++)
	{
		if (!aname.compare((*ta)[i].name))return (*ta)[i].type;
	}
	return 2;
}

bool Interpreter::syntax()
{
	string op = gword(s," ");
	vector<Attribute>* ta;
	if (!op.compare("create")) {
		string type = gword(s," ");

		if (!type.compare("table")) {
			string tname = gword(s," ");
			
			if (cm->find_table(tname)) {cout << "Table name is duplicated" << endl;return 0 ;}
			else {
				if (s[0] != '(') { cout << "'(' is absent after table's name" << endl;return 0 ; }
				s.erase(0, 1);
				int over = 0;
				string att;vector<Attribute> ats;
				while (1) {
					s.erase(0, s.find_first_not_of(" "));
					s.erase(0, s.find_first_not_of("\t"));
					s.erase(0, s.find_first_not_of(" "));
					if (s.find(",")!=string::npos) 
						att = gword(s, ",");
					else if (s.find(")") != string::npos) {
						att = s.substr(0, s.find_last_of(")"));over = 1;
					}					
					else { cout << "Absence of ',' or ')'" << endl; return 0; }
					string tat = att;

				if(att.find("primary key") != string::npos && att.find("(")!= string::npos)
				{
					if (att.find(")") == string::npos)
					{
						cout << "Syntax is wrong at primary key" << endl;return 0;
					}
					string pk = att.substr(att.find("(") + 1, att.find(")") - att.find("(") - 1);
					for (int i = 0;i<ats.size();i++)
						if (!pk.compare(ats[i].name)) { ats[i].primary = true;break; }

					if (over) { ap->create_table(tname, &ats);return 0; }
				}
				else
				{
						Attribute at;
						//at.primary = false;at.unique = false;
						at.name = att.substr(0, att.find_first_of(" "));
						if (att.find("unique") != string::npos)at.unique = true;
						if (att.find("primary key") != string::npos)at.primary = true;
						if (att.find("int") != string::npos)at.type = 0;
						else if (att.find("float") != string::npos)at.type = 1;
						else if (att.find("char") != string::npos) {
							if (att.find("(") == string::npos) 
							{ cout << "'char' is not specified at: " << tat << endl;return 0; }
							else {
								stringstream ss;
								ss << att.substr(att.find("(") + 1, att.find(")") - att.find("(") - 1);
								ss >> at.type;
								at.type = -at.type;
							}
						}
						else { cout << "Unknown type at: " << tat << endl; return 0; }
						ats.push_back(at);
						if (over) { ap->create_table(tname, &ats);return 0; }
					}
					//primary key
					
				}
			}
		}
		else if (!type.compare("index")) {
			if (s.find("on") == string::npos) { cout << "'on' is absent" << endl;return 0; }
			if (s.find("(") == string::npos || s.find(")") == string::npos)
			{
				cout << "'(' or ')' is absent" << endl;return 0;
			}
			string iname = gword(s," ");
			s.erase(0, s.find_first_of(" ") + 1);
			string tname = gword(s,"(");
			if (!cm->find_table(tname)) { cout << tname << " does not exist" << endl; return 0; }
			ta = cm->attr(tname);
			
			string aname = gword(s,")");
			if (check_attr(ta,aname) > 1) 
			 { cout << aname << " does not exist in " << tname << endl;return 0; }
			ap->create_index(iname, tname, aname);
		}
			
		else { cout << "Not such a type as '" << type << "'" <<endl;
		cout << "Candidate: 'table' or 'index' " << endl;return 0;}
	}
	else if (!op.compare("drop")) {
		string type = gword(s," ");
		if (!type.compare("table")) {
			string tname = gword(s,";");
			if (!cm->find_table(tname)) { cout << tname << " does not exist" << endl;return 0; }
			ap->drop_table(tname);
		}
		else if (!type.compare("index")) {
			string iname = gword(s,";");
			if (!cm->find_index(iname)) { cout << iname << " does not exist" << endl;return 0; }
			ap->drop_index(iname);
		}
		else { cout << "Not such a type as '" << type << "'" <<endl;
		cout << "Candidate: 'table' or 'index' " << endl;return 0; }
	}
	else if (!op.compare("insert")) {
		if (s.find("into") == string::npos) { cout << "'into' is absent" << endl; }
		if (s.find("values") == string::npos) { cout << "'values' is absent" << endl; }
		s.erase(0, s.find_first_of(" ") + 1);
		string tname = gword(s," ");
		if(!cm->find_table(tname)) { cout << tname << " does not exist" << endl;return 0; }

		ta = cm->attr(tname);

		if (s.find("(") == string::npos || s.find(")") == string::npos)
		{cout << "'(' or ')' is absent" << endl;return 0;}
		s.erase(0, s.find_first_of("(") + 1);
		vector<string> val;
		int over = 0;int index = 0;
		while (1) {
			if (over)
				if (index == (*ta).size())
				{   
					int size = ap->get_recordSize(tname);
					char *vva=new char[size];
					value(vva, &val, ta);
					ap->insert(tname, vva,size);return 0;
				}	
				else { cout << "Not enough amount of values" << endl;return 0; }
			string va;
			if(s.find(",")!=string::npos)
			 va = gword(s, ", ");
			else if(s.find(")")!=string::npos)
			{
			va = s.substr(0, s.find_first_of(")"));over = 1;
			}
			
			if(ctype((*ta)[index++].type,va)){ val.push_back(va);continue; }
			else { cout << "The value " << va << " is not in correct type" << endl;return 0; }

		}
	}

	else if (!op.compare("select") || !op.compare("delete")) {
		int sod = 0;
		if (!op.compare("select")) sod = 1;
		if(sod && s.find("*")==string::npos)
		{
			cout << "'*' is absent" << endl;return 0;
		}
		if (s.find("from") == string::npos)
		{
			cout << "'from' is absent" << endl;return 0;
		}
		s.erase(0, s.find_first_of("from") + 5);
		string tname;vector<Condition> res;
		if (s.find("where") == string::npos)
		{
			tname = gword(s, ";");
			if (sod)ap->select(tname, &res);
			else ap->ddelete(tname, &res);
			return 0;
		}
		
		tname = gword(s, " ");
		s.erase(0, 6);
		
		//cout << "ini s: " << s << endl;
		ta = cm->attr(tname);
		int over = 0;
		while (1) {
			string re;
			if (s.find("and") != string::npos)
			{
				re = gword(s, " and ");
				//cout << "there is an and" << endl;
			}
				
			else { re = gword(s, ";");over = 1; }

			//cout << "re: " << re << endl;
			//cout << "s: " << s << endl;

			string aname = gword(re, " ");int type = check_attr(ta, aname);
			if (type>1) { cout << aname << " does not exist" << endl;return 0; }
	
			string oper = gword(re, " ");
			OPERATOR ope;
			if (!oper.compare("="))ope = OPERATOR_EQUAL;
			else if (!oper.compare("<>"))ope = OPERATOR_NOT_EQUAL;
			else if (!oper.compare("<"))ope = OPERATOR_LESS;
			else if (!oper.compare(">"))ope = OPERATOR_MORE;
			else if (!oper.compare("<="))ope = OPERATOR_LESS_EQUAL;
			else if (!oper.compare(">="))ope = OPERATOR_MORE_EQUAL;
			else { cout << oper << " is not legal" << endl;return 0 ; }

			string value = re;
			if (!ctype(type,value)) 
			{ cout << value << " is not in correct type as " << aname << endl; return 0; }

			Condition rest(aname, value, ope);

			res.push_back(rest);
			if(over)
				if (sod) { ap->select(tname, &res);return 0; }
				else { ap->ddelete(tname, &res);return 0; }
		}
	}

	else if (!op.compare("quit;")) {
		return 1;
	}
	else if (!op.compare("execfile")) {
		string fname = gword(s, ";");
		FILE* fp;
		fp = fopen(fname.c_str(), "r");
		if (!fp) { cout << fname << " does not exist" << endl;return 0; }

		ifstream fin(fname);
		bool quit = 0;
		do {
			if(fread(fin))quit = syntax();
			else break;
		} while (!fin.eof() && !quit);
		fin.close();
	    return 0;
	}	
	else {
		cout << "Not a legal instruction" << endl; return 0;
	}
	return 0;
}
