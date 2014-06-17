/****************************************************************************
*                                                                           *
* GAL                                                                       *
* Copyright (C)2008 - 2013 SWEDISH DEFENCE RESEARCH AGENCY (FOI)            *
* Author: Johan Hedstr�m                                                    *
* Email: johan.hedstrom@foi.se                                              *
*                                                                           *
* GAL is only used with knowledge from the author. This software            *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to contact author.           *
*                                                                           *
*                                                                           *
*****************************************************************************/ 

#pragma once

class dtNavMesh;
class dtTileCache;
class dtNavMeshQuery;
class InputGeom;
class rcContext;


class TiledNavigationMeshBuilder
{
public:
	TiledNavigationMeshBuilder(rcContext* ctx);
	virtual ~TiledNavigationMeshBuilder();
	dtNavMesh* Build(InputGeom* geom);
private:
	unsigned char* BuildTileMesh(InputGeom* geom, const int tx, const int ty, const float* bmin, const float* bmax, int& dataSize);
private:
	rcContext* m_Ctx;
public:
	float m_CellSize;
	float m_CellHeight;
	float m_AgentHeight;
	float m_AgentRadius;
	float m_AgentMaxClimb;
	float m_AgentMaxSlope;
	float m_RegionMinSize;
	float m_RegionMergeSize;
	float m_EdgeMaxLen;
	float m_EdgeMaxError;
	float m_VertsPerPoly;
	float m_DetailSampleDist;
	float m_DetailSampleMaxError;
	int m_TileSize;
	bool m_MonotonePartitioning;
};


