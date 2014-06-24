
#pragma once
#include <stdio.h>
#include <string>
class dtNavMesh;

class TiledNavigationMeshSerializer 
{
public:
	TiledNavigationMeshSerializer();
	virtual ~TiledNavigationMeshSerializer();
	dtNavMesh* Load(const std::string &path) const;
	dtNavMesh* Load(FILE* fp) const;
	void Save(const std::string &path, const dtNavMesh *mesh) const;
	void Save(FILE* fp, const dtNavMesh *mesh) const;
private:
};


