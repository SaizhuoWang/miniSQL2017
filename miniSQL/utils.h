#pragma once
#include "Widget.h"
class Utils
{
private:
	static BufferManager* bufferManager;
	static CatalogManager* catalogManager;
	static RecordManager* recordManager;
	static IndexManager* indexManager;

public:
	static void Initialize();
	static void DeleteUtils();
	static BufferManager *GetBufferManager();
	static CatalogManager *GetCatalogManager();
	static RecordManager *GetRecordManager();
	static IndexManager *GetIndexManager();

	static int GetTypeSize(int type);
	static bool DeleteFile(const char * filename);
};

