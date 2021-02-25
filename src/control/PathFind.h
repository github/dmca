#pragma once

#include "Treadable.h"

class CVehicle;
class CPtrList;

enum
{
	NodeTypeExtern = 1,
	NodeTypeIntern = 2,

	UseInRoadBlock = 1,
	ObjectEastWest = 2,
};

enum
{
	PATH_CAR = 0,
	PATH_PED = 1,
};

enum
{
	SWITCH_OFF = 0,
	SWITCH_ON = 1,
};

enum 
{
	ROUTE_ADD_BLOCKADE = 0,
	ROUTE_NO_BLOCKADE = 1
};

struct CPedPathNode
{
	bool bBlockade;
	uint8 nodeIdX;
	uint8 nodeIdY;
	int16 id;
	CPedPathNode* prev;
	CPedPathNode* next;
};

VALIDATE_SIZE(CPedPathNode, 0x10);

class CPedPath {
public:
	static bool CalcPedRoute(int8 pathType, CVector position, CVector destination, CVector *pointPoses, int16 *pointsFound, int16 maxPoints);
	static void AddNodeToPathList(CPedPathNode *pNodeToAdd, int16 id, CPedPathNode *pNodeList);
	static void RemoveNodeFromList(CPedPathNode *pNode);
	static void AddNodeToList(CPedPathNode *pNode, int16 index, CPedPathNode *pList);
	static void AddBlockade(CEntity *pEntity, CPedPathNode(*pathNodes)[40], CVector *pPosition);
	static void AddBlockadeSectorList(CPtrList& list, CPedPathNode(*pathNodes)[40], CVector *pPosition);
};

struct CPathNode
{
	CVector pos;
	CPathNode *prev;
	CPathNode *next;
	int16 distance;		// in path search
	int16 objectIndex;
	int16 firstLink;
	uint8 numLinks;

	uint8 unkBits : 2;
	uint8 bDeadEnd : 1;
	uint8 bDisabled : 1;
	uint8 bBetweenLevels : 1;

	int8 group;

	CVector &GetPosition(void) { return pos; }
	void SetPosition(const CVector &p) { pos = p; }
	float GetX(void) { return pos.x; }
	float GetY(void) { return pos.y; }
	float GetZ(void) { return pos.z; }

	CPathNode *GetPrev(void) { return prev; }
	CPathNode *GetNext(void) { return next; }
	void SetPrev(CPathNode *node) { prev = node; }
	void SetNext(CPathNode *node) { next = node; }
};

union CConnectionFlags
{
	uint8 flags;
	struct {
		uint8 bCrossesRoad : 1;
		uint8 bTrafficLight : 1;
	};
};

struct CCarPathLink
{
	CVector2D pos;
	CVector2D dir;
	int16 pathNodeIndex;
	int8 numLeftLanes;
	int8 numRightLanes;
	uint8 trafficLightType;

	uint8 bBridgeLights : 1;
	// more?

	CVector2D &GetPosition(void) { return pos; }
	CVector2D &GetDirection(void) { return dir; }
	float GetX(void) { return pos.x; }
	float GetY(void) { return pos.y; }
	float GetDirX(void) { return dir.x; }
	float GetDirY(void) { return dir.y; }

	float OneWayLaneOffset()
	{
		if (numLeftLanes == 0)
			return 0.5f - 0.5f * numRightLanes;
		if (numRightLanes == 0)
			return 0.5f - 0.5f * numLeftLanes;
		return 0.5f;
	}
};

// This is what we're reading from the files, only temporary
struct CPathInfoForObject
{
	int16 x;
	int16 y;
	int16 z;
	int8 type;
	int8 next;
	int8 numLeftLanes;
	int8 numRightLanes;
	uint8 crossing : 1;
};
extern CPathInfoForObject *InfoForTileCars;
extern CPathInfoForObject *InfoForTilePeds;

struct CTempNode
{
	CVector pos;
	float dirX;
	float dirY;
	int16 link1;
	int16 link2;
	int8 numLeftLanes;
	int8 numRightLanes;
	int8 linkState;
};

struct CTempDetachedNode	// unused
{
	uint8 foo[20];
};

class CPathFind
{
public:
	CPathNode m_pathNodes[NUM_PATHNODES];
	CCarPathLink m_carPathLinks[NUM_CARPATHLINKS];
	CTreadable *m_mapObjects[NUM_MAPOBJECTS];
	uint8 m_objectFlags[NUM_MAPOBJECTS];
	int16 m_connections[NUM_PATHCONNECTIONS];
	int16 m_distances[NUM_PATHCONNECTIONS];
	CConnectionFlags m_connectionFlags[NUM_PATHCONNECTIONS];
	int16 m_carPathConnections[NUM_PATHCONNECTIONS];

	int32 m_numPathNodes;
	int32 m_numCarPathNodes;
	int32 m_numPedPathNodes;
	int16 m_numMapObjects;
	int16 m_numConnections;
	int32 m_numCarPathLinks;
	int32 unk;
	uint8 m_numGroups[2];
	CPathNode m_searchNodes[512];

	void Init(void);
	void AllocatePathFindInfoMem(int16 numPathGroups);
	void RegisterMapObject(CTreadable *mapObject);
	void StoreNodeInfoPed(int16 id, int16 node, int8 type, int8 next, int16 x, int16 y, int16 z, int16 width, bool crossing);
	void StoreNodeInfoCar(int16 id, int16 node, int8 type, int8 next, int16 x, int16 y, int16 z, int16 width, int8 numLeft, int8 numRight);
	void CalcNodeCoors(int16 x, int16 y, int16 z, int32 id, CVector *out);
	bool LoadPathFindData(void);
	void PreparePathData(void);
	void CountFloodFillGroups(uint8 type);
	void PreparePathDataForType(uint8 type, CTempNode *tempnodes, CPathInfoForObject *objectpathinfo,
		float maxdist, CTempDetachedNode *detachednodes, int32 numDetached);

	bool IsPathObject(int id) { return id < PATHNODESIZE && (InfoForTileCars[id*12].type != 0 || InfoForTilePeds[id*12].type != 0); }

	float CalcRoadDensity(float x, float y);
	bool TestForPedTrafficLight(CPathNode *n1, CPathNode *n2);
	bool TestCrossesRoad(CPathNode *n1, CPathNode *n2);
	void AddNodeToList(CPathNode *node, int32 listId);
	void RemoveNodeFromList(CPathNode *node);
	void RemoveBadStartNode(CVector pos, CPathNode **nodes, int16 *n);
	void SetLinksBridgeLights(float, float, float, float, bool);
	void SwitchOffNodeAndNeighbours(int32 nodeId, bool disable);
	void SwitchRoadsOffInArea(float x1, float x2, float y1, float y2, float z1, float z2, bool disable);
	void SwitchPedRoadsOffInArea(float x1, float x2, float y1, float y2, float z1, float z2, bool disable);
	void SwitchRoadsInAngledArea(float x1, float y1, float z1, float x2, float y2, float z2, float length, uint8 type, uint8 enable);
	void MarkRoadsBetweenLevelsNodeAndNeighbours(int32 nodeId);
	void MarkRoadsBetweenLevelsInArea(float x1, float x2, float y1, float y2, float z1, float z2);
	void PedMarkRoadsBetweenLevelsInArea(float x1, float x2, float y1, float y2, float z1, float z2);
	int32 FindNodeClosestToCoors(CVector coors, uint8 type, float distLimit, bool ignoreDisabled = false, bool ignoreBetweenLevels = false);
	int32 FindNodeClosestToCoorsFavourDirection(CVector coors, uint8 type, float dirX, float dirY);
	float FindNodeOrientationForCarPlacement(int32 nodeId);
	float FindNodeOrientationForCarPlacementFacingDestination(int32 nodeId, float x, float y, bool towards);
	bool NewGenerateCarCreationCoors(float x, float y, float dirX, float dirY, float spawnDist, float angleLimit, bool forward, CVector *pPosition, int32 *pNode1, int32 *pNode2, float *pPositionBetweenNodes, bool ignoreDisabled = false);
	bool GeneratePedCreationCoors(float x, float y, float minDist, float maxDist, float minDistOffScreen, float maxDistOffScreen, CVector *pPosition, int32 *pNode1, int32 *pNode2, float *pPositionBetweenNodes, CMatrix *camMatrix);
	CTreadable *FindRoadObjectClosestToCoors(CVector coors, uint8 type);
	void FindNextNodeWandering(uint8, CVector, CPathNode**, CPathNode**, uint8, uint8*);
	void DoPathSearch(uint8 type, CVector start, int32 startNodeId, CVector target, CPathNode **nodes, int16 *numNodes, int16 maxNumNodes, CVehicle *vehicle, float *dist, float distLimit, int32 forcedTargetNode);
	bool TestCoorsCloseness(CVector target, uint8 type, CVector start);
	void Save(uint8 *buf, uint32 *size);
	void Load(uint8 *buf, uint32 size);
	uint16 ConnectedNode(int id) { return m_connections[id]; }
	bool ConnectionCrossesRoad(int id) { return m_connectionFlags[id].bCrossesRoad; }
	bool ConnectionHasTrafficLight(int id) { return m_connectionFlags[id].bTrafficLight; }
	void ConnectionSetTrafficLight(int id) { m_connectionFlags[id].bTrafficLight = true; }

	void DisplayPathData(void);
};

VALIDATE_SIZE(CPathFind, 0x49bf4);

extern CPathFind ThePaths;

extern bool gbShowPedPaths;
extern bool gbShowCarPaths;
extern bool gbShowCarPathsLinks;
