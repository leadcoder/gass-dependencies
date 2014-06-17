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


