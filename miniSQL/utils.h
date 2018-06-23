#pragma once
#include "BufferManager.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

static class Utils
{
private:
	static BufferManager* bufferManager;
	static CatalogManager* catalogManager;
	static RecordManager* recordManager;
	static IndexManager* indexManager;

public:
	static void Initialize();

	static BufferManager *GetBufferManager();
	static CatalogManager *GetCatalogManager();
	static RecordManager *GetRecordManager();
	static IndexManager *GetIndexManager();

	static int GetTypeSize(int type);
};

