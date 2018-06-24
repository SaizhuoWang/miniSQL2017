#include "stdafx.h"
#include "utils.h"

BufferManager* Utils::bufferManager = NULL;
CatalogManager* Utils::catalogManager = NULL;
RecordManager* Utils::recordManager = NULL;
IndexManager* Utils::indexManager = NULL;

void Utils::Initialize()
{
	bufferManager = new BufferManager();
	catalogManager = new CatalogManager();
	recordManager = new RecordManager();
	indexManager = new IndexManager();
}

BufferManager *Utils::GetBufferManager()
{
	return bufferManager;
}

CatalogManager *Utils::GetCatalogManager()
{
	return catalogManager;
}

RecordManager *Utils::GetRecordManager()
{
	return recordManager;
}

IndexManager * Utils::GetIndexManager()
{
	return indexManager;
}

int Utils::GetTypeSize(int type)
{
	if (type > 1) // Null
		return 0;
	else if (type == 0 || type == 1) // Int or Float
		return 4;
	else if (type < 0)
		return -type + 1;
	cerr << "Type error: Unknown type.";
	return 0;
}

bool Utils::DeleteFile(const char * filename)
{
	return false;
}
