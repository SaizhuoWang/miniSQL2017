#include "stdafx.h"
#include "Widget.h"
#include "utils.h"

#pragma warning(disable : 4996)

int main()
{
	Utils *util = new Utils;
	util->Initialize();
	util->GetCatalogManager()->SetBM(util->GetBufferManager());
	
	return 0;
}

