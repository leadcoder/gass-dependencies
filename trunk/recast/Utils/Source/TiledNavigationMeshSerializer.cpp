/****************************************************************************
*                                                                           *
* GAL                                                                       *
* Copyright (C)2008 - 2013 SWEDISH DEFENCE RESEARCH AGENCY (FOI)            *
* Author: Johan Hedström                                                    *
* Email: johan.hedstrom@foi.se                                              *
*                                                                           *
* GAL is only used with knowledge from the author. This software            *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to contact author.           *
*                                                                           *
*                                                                           *
*****************************************************************************/ 


#include "TiledNavigationMeshSerializer.h"
#include "Recast.h"
#include "RecastAlloc.h"
#include "RecastAssert.h"
#include "DetourAssert.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"
#include "DetourTileCache.h"

#include <string.h>


TiledNavigationMeshSerializer::TiledNavigationMeshSerializer() 
{
}

TiledNavigationMeshSerializer::~TiledNavigationMeshSerializer()
{

}

// from recast tile example
static const int NAVMESHSET_MAGIC = 'M'<<24 | 'S'<<16 | 'E'<<8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

struct NavMeshSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

dtNavMesh* TiledNavigationMeshSerializer::Load(const std::string &path) const
{
	FILE* fp = fopen(path.c_str(), "rb");
	if (!fp) return 0;
	dtNavMesh* ret = Load(fp);
	fclose(fp);
	return ret;
}

dtNavMesh* TiledNavigationMeshSerializer::Load(FILE* fp) const
{
	if (!fp) return 0;

	// Read header.
	NavMeshSetHeader header;
	fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		return 0;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		return 0;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(fp);
		return 0;
	}

	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return 0;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		fread(data, tileHeader.dataSize, 1, fp);

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}
	return mesh;
}

void TiledNavigationMeshSerializer::Save(const std::string &path, const dtNavMesh* mesh) const
{
	if (!mesh) return;
	FILE* fp = fopen(path.c_str(), "wb");
	if (!fp)
		return;
	fclose(fp);
}

void TiledNavigationMeshSerializer::Save(FILE* fp, const dtNavMesh* mesh) const
{
	if (!fp)
		return;

	// Store header.
	NavMeshSetHeader header;
	header.magic = NAVMESHSET_MAGIC;
	header.version = NAVMESHSET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header.numTiles++;
	}
	memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
	fwrite(&header, sizeof(NavMeshSetHeader), 1, fp);

	// Store tiles.
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

		fwrite(tile->data, tile->dataSize, 1, fp);
	}
}


