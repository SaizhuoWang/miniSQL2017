#include "Interpreter.h"
#include "CatalogManager.h"
#include "API.h"

Interpreter::Interpreter(API* api, CatalogManager * cmr):ap(api),cm(cmr)
{
}


Interpreter::~Interpreter()
{
}

template <class T>
void Interpreter::read(T& rin)
{   
	s.clear();
	string temp;
	while (temp[temp.length() - 1] != ';') 
	{
		rin >> temp;
		s = s + temp + " ";		
	} 
}

string Interpreter::gword(string& s,const string& mark)
{
	string t = s.substr(0, s.find_first_of(mark));
	s.erase(0, s.find_first_of(mark) + mark.length());
	return t;
}

bool Interpreter::ctype(int t,string& s)
{   
	int stype = 0;
	if (s.find("\"") != string::npos)stype = -(s.length() - 2);
	else if (s.find(".") != string::npos)stype = 1;
	else stype = 0;

	if (stype >= 0)
		if (stype == t)return true;
		else return false;
	else
		if (stype >= t) {
			int offset = stype - t;
			while (offset--)s = s + " ";
			return true;
		}
		else return false;
}

int Interpreter::check_attr(const vector<Attribute>* ta, const string& aname)
{
	int i = 0;
	for (i = 0;i < (*ta).size();i++)
	{
		if (aname.compare((*ta)[i].name))return (*ta)[i].type;
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
					if (s.find(",")!=string::npos) 
						att = gword(s, ",");
					else if (s.find(")") != string::npos) {
						att = s.substr(0, s.find_last_of(")"));over = 1;
					}					
					else { cout << "Absence of ',' or ')'" << endl; return 0; }
					string tat = att;

					if (att.find("primary") == string::npos) {
						Attribute at;
						at.name = att.substr(0, att.find_first_of(" "));
						if (att.find("unique") != string::npos)at.unique = true;
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
						if (over) { ap->create_table(tname, ats);return 0; }
					}
					//primary key
					else {
						if (att.find("key(") == string::npos) 
						{ cout << "Syntax is wrong at primary key" << endl;return 0; }
						string pk = att.substr(att.find("(") + 1, att.find(")") - att.find("(") - 1);
						for(int i=0;i<ats.size();i++)
							if (!pk.compare(ats[i].name)) { ats[i].primary = true;break; }

						if (over) { ap->create_table(tname, ats);return 0; }
					}
				}
			}
		}
		else if (!type.compare("index")) {
			if (s.find("on") == string::npos) { cout << "'on' is absent" << endl;return 0; }
			string iname = gword(s," ");
			s.erase(0, s.find_first_of(" ") + 1);
			string tname = gword(s," ");
			if (!cm->find_table(tname)) { cout << tname << " does not exist" << endl; return 0; }
			ta = cm->attr(tname);
			if(s.find("(")==string::npos || s.find(")")==string::npos)
			{cout << "'(' or ')' is absent" << endl;return 0;}
			string aname = s.substr(s.find_first_of("(") + 1, s.find_first_of(")") - s.find_first_of("(") - 1);
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
			string tname = gword(s," ");
			if (!cm->find_table(tname)) { cout << tname << " does not exist" << endl;return 0; }
			ap->drop_table(tname);
		}
		else if (!type.compare("index")) {
			string iname = gword(s," ");
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
		string val;
		int over = 0;int index = 0;
		while (1) {
			if (over)
				if (index == (*ta).size())
				{ap->insert(tname, val);return 0;}	
				else { cout << "Not enough amount of values" << endl;return 0; }
			string va;
			if(s.find(",")!=string::npos)
			 va = gword(s, ", ");
			else if(s.find(")")!=string::npos)
			{
			va = s.substr(0, s.find_first_of(")"));over = 1;
			}
			
			if(ctype((*ta)[index++].type,va)){ val = val + va;continue; }
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
		string tname;
		if(s.find("where")!=string::npos)
		tname = gword(s, " ");
		s.erase(0, s.find_first_of(" "));
		vector<Restriction> res;
		ta = cm->attr(tname);
		int over = 0;
		while (1) {
			Restriction rest;
			string re;
			if (s.find("and") != string::npos)
				re = gword(s, " and ");
			else { re = gword(s, ";");over = 1; }

			string aname = gword(re, " ");int type = check_attr(ta, aname);
			if (type>1) { cout << aname << " does not exist" << endl;return 0; }
			int ina = 0;int i = 0;

			for(i=0;i<(*ta).size;i++)
				if (!aname.compare((*ta)[i].name)) { ina = i;break; }
		
			string oper = gword(re, " ");
			int ope = 0;
			if (!oper.compare("="))ope = 0;
			else if (!oper.compare("<>"))ope = 1;
			else if (!oper.compare("<"))ope = 2;
			else if (!oper.compare(">"))ope = 3;
			else if (!oper.compare("<="))ope = 4;
			else if (!oper.compare(">="))ope = 5;

			string value = re;
			if (!ctype(type,value)) 
			{ cout << value << " is not in correct type as " << aname << endl; return 0; }

			rest.aname = ina;
			rest.op = ope;
			rest.value = value;

			res.push_back(rest);
			if(over)
				if (sod) { ap->select(tname, res);return 0; }
				else { ap->ddelete(tname, res);return 0; }
		}
	}

	else if (!op.compare("quit")) {
		return 1;
	}
	else if (!op.compare("execfile")) {
		string fname = gword(s, ";");
		ifstream fin(fname);
		read(fin);
		bool quit = 0;
		while (!fin.eof()&&!quit) { quit = syntax();read(fin); }
		if (quit)return 1;
		else return 0;
	}	
	else cout << "Not a legal instruction" << endl;
}
