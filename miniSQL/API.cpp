#include "stdafx.h"
#include "Widget.h"

API::API()
{
}


API::~API()
{
}

void API::create_table(const string& tname, const vector<Attribute>* ats)
{
	if (cm->create_table(tname, ats))
	{
		rm->create_table(tname);
		cout << "Create " << tname << " succeed" << endl;
	}
		
	else cout << "Create failed" << endl;
}

void API::create_index(const string& iname, const string& tname, const string& aname)
{
	if (cm->create_index(iname,tname,aname) && im->CreateIndex(iname.c_str()))
		cout << "Create " << iname << " on " << tname << " succeed" << endl;
	else cout << "Create failed" << endl;
}

void API::drop_table(const string& tname)
{
	if (cm->drop_table(tname))
	{
		rm->delete_table(tname);
		cout << "Drop " << tname << " succeed" << endl;
	}
	else cout << "Drop failed" << endl;
}

void API::drop_index(const string& iname)
{
	if (cm->drop_index(iname) && im->DropIndex(iname.c_str()))
		cout << "Drop " << iname << " succeed" << endl;
	else cout << "Drop failed" << endl;
}

void API::select(const string& tname, vector<Condition> * res)
{   
	vector<string> as;
	vector<Attribute> * ats = cm->attr(tname);
	for (int i = 0; i < ats->size();i++)
	{
		as.push_back((*ats)[i].name);
	}
	if (rm->show_allrecord(tname, &as, res))return;
	else cout << "Select failed" << endl;
}

void API::ddelete(const string& tname, vector<Condition> * res)
{
	if (rm->delete_allrecord(tname, res))
		cout << "Delete succeed" << endl;
	else cout << "Delete failed" << endl;
}

void API::insert(const string& tname, char* value, const int size)
{
	if (rm->insert_record(tname, value ,size))
		cout << "Insert succeed" << endl;
	else cout << "Insert failed" << endl;
}

int API::get_recordSize(const string& tname)
{
	vector<Attribute> * ats = cm->attr(tname);
	int i;int size = 0;
	for (i = 0;i < ats->size();i++)
	{
		if ((*ats)[i].type == 0)
			size += 4;
		else if ((*ats)[i].type == 1)
			size += 8;
		else size += (-(*ats)[i].type);
	}
	return size;
}

void API::attributeGet(const string& tname, vector<Attribute>* ats)
{
	ats = cm->attr(tname);
}

void API::recordIndexDelete(char* rp, int rs, int bo)
{
	cout << "index delete" << endl;
}

void API::recordAddIndex(char* rp, int rs, int bo)
{
	cout << "index add" << endl;
}
