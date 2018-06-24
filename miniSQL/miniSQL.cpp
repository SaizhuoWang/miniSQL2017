#include "stdafx.h"
#include "utils.h"
using namespace std;
#pragma warning(disable : 4996)

int main()
{
	Utils::Initialize();

	Interpreter in;
	in.cm = Utils::GetCatalogManager();

	API ap;
	ap.cm = Utils::GetCatalogManager();
	ap.im = Utils::GetIndexManager();
	ap.rm = Utils::GetRecordManager();

	ap.ip = &in;
	in.ap = &ap;
    
	cout << "************************" << endl;
	cout << "*        miniSQL       *" << endl;
	cout << "************************" << endl;
	cout << "*     Contributors     *" << endl;
	cout << "*    SDW WSZ LZH KZY   *" << endl;
	cout << "************************\n" << endl;

	do { 
		cout << "miniSQL >";
		in.read(); 
		cout << in.s << endl;
	} while (!in.syntax());

	return 0;
}

