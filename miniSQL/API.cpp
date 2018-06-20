#include "stdafx.h"
#include "API.h"


API::API()
{
}


API::~API()
{
}

void API::create_table(const string& tname, const vector<Attribute>* ats)
{
	if (cm->create_table(tname, ats))
		cout << "Create " << tname << " succeed" << endl;
	else cout << "Create failed" << endl;
}

void API::create_index(const string& iname, const string& tname, const string& aname)
{
	if (cm->create_index(iname,tname,aname) && im->create_index(iname,tname,aname))
		cout << "Create " << iname << " on " << tname << " succeed" << endl;
	else cout << "Create failed" << endl;
}

void API::drop_table(const string& tname)
{
	if (cm->drop_table(tname) && rm->drop_table(tname))
		cout << "Drop " << tname << " succeed" << endl;
	else cout << "Drop failed" << endl;
}

void API::drop_index(const string& iname)
{
	if (cm->drop_index(iname) && im->drop_index(iname))
		cout << "Drop " << iname << " succeed" << endl;
	else cout << "Drop failed" << endl;
}

void API::select(const string& tname, const vector<Restriction> * res)
{

}

void API::ddelete(const string& tname, const vector<Restriction> * res)
{

}

void API::insert(const string& tname, const string& value)
{
	if (rm->insert(tname, value) && im->insert(tname, value));
}
