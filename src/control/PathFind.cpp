#include "common.h"

#include "General.h"
#include "FileMgr.h"	// only needed for empty function
#include "Camera.h"
#include "Vehicle.h"
#include "World.h"
#include "Lines.h"	// for debug
#include "PathFind.h"

bool gbShowPedPaths;
bool gbShowCarPaths;
bool gbShowCarPathsLinks;

CPathFind ThePaths;

#define MAX_DIST INT16_MAX-1
#define MIN_PED_ROUTE_DISTANCE 23.8f


#define NUMTEMPNODES 4000
#define NUMDETACHED_CARS 100
#define NUMDETACHED_PEDS 50


// object flags:
//	1	UseInRoadBlock
//	2	east/west road(?)

CPathInfoForObject *InfoForTileCars;
CPathInfoForObject *InfoForTilePeds;

// unused
CTempDetachedNode *DetachedNodesCars;
CTempDetachedNode *DetachedNodesPeds;

bool 
CPedPath::CalcPedRoute(int8 pathType, CVector position, CVector destination, CVector *pointPoses, int16 *pointsFound, int16 maxPoints)
{
	*pointsFound = 0;
	CVector vecDistance = destination - position;
	if (Abs(vecDistance.x) > MIN_PED_ROUTE_DISTANCE || Abs(vecDistance.y) > MIN_PED_ROUTE_DISTANCE || Abs(vecDistance.z) > MIN_PED_ROUTE_DISTANCE)
		return false;
	CVector vecPos = (position + destination) * 0.5f;
	CVector vecSectorStartPos (vecPos.x - 14.0f, vecPos.y - 14.0f, vecPos.z);
	CVector2D vecSectorEndPos (vecPos.x + 28.0f, vecPos.x + 28.0f);
	const int16 nodeStartX = (position.x - vecSectorStartPos.x) / 0.7f;
	const int16 nodeStartY = (position.y - vecSectorStartPos.y) / 0.7f;
	const int16 nodeEndX = (destination.x - vecSectorStartPos.x) / 0.7f;
	const int16 nodeEndY = (destination.y - vecSectorStartPos.y) / 0.7f;
	if (nodeStartX == nodeEndX && nodeStartY == nodeEndY)
		return false;
	CPedPathNode pathNodes[40][40]; 
	CPedPathNode pathNodesList[416];
	for (int32 x = 0; x < 40; x++) {
		for (int32 y = 0; y < 40; y++) {
			pathNodes[x][y].bBlockade = false;
			pathNodes[x][y].id = INT16_MAX;
			pathNodes[x][y].nodeIdX = x;
			pathNodes[x][y].nodeIdY = y;
		}
	}
	CWorld::AdvanceCurrentScanCode();
	if (pathType != ROUTE_NO_BLOCKADE) {
		const int32 nStartX = Max(CWorld::GetSectorIndexX(vecSectorStartPos.x), 0);
		const int32 nStartY = Max(CWorld::GetSectorIndexY(vecSectorStartPos.y), 0);
		const int32 nEndX = Min(CWorld::GetSectorIndexX(vecSectorEndPos.x), NUMSECTORS_X - 1);
		const int32 nEndY = Min(CWorld::GetSectorIndexY(vecSectorEndPos.y), NUMSECTORS_Y - 1);
		for (int32 y = nStartY; y <= nEndY; y++) {
			for (int32 x = nStartX; x <= nEndX; x++) {
				CSector *pSector = CWorld::GetSector(x, y);
				AddBlockadeSectorList(pSector->m_lists[ENTITYLIST_VEHICLES], pathNodes, &vecSectorStartPos);
				AddBlockadeSectorList(pSector->m_lists[ENTITYLIST_VEHICLES_OVERLAP], pathNodes, &vecSectorStartPos);
				AddBlockadeSectorList(pSector->m_lists[ENTITYLIST_OBJECTS], pathNodes, &vecSectorStartPos);
				AddBlockadeSectorList(pSector->m_lists[ENTITYLIST_OBJECTS_OVERLAP], pathNodes, &vecSectorStartPos);
			}
		}
	}
	for (int32 i = 0; i < 416; i++) {
		pathNodesList[i].prev = nil;
		pathNodesList[i].next = nil;
	}
	CPedPathNode *pStartPathNode = &pathNodes[nodeStartX][nodeStartY];
	CPedPathNode *pEndPathNode = &pathNodes[nodeEndX][nodeEndY];
	pEndPathNode->bBlockade = false;
	pEndPathNode->id = 0;
	pEndPathNode->prev = nil;
	pEndPathNode->next = pathNodesList;
	pathNodesList[0].prev = pEndPathNode;
	int32 pathNodeIndex = 0;
	CPedPathNode *pPreviousNode = nil;
	for (; pathNodeIndex < 414; pathNodeIndex++)
	{
		pPreviousNode = pathNodesList[pathNodeIndex].prev;
		while (pPreviousNode && pPreviousNode != pStartPathNode) {
			const uint8 nodeIdX = pPreviousNode->nodeIdX;
			const uint8 nodeIdY = pPreviousNode->nodeIdY;
			if (nodeIdX > 0) {
				AddNodeToPathList(&pathNodes[nodeIdX - 1][nodeIdY], pathNodeIndex + 5, pathNodesList);
				if (nodeIdY > 0)
					AddNodeToPathList(&pathNodes[nodeIdX - 1][nodeIdY - 1], pathNodeIndex + 7, pathNodesList);
				if (nodeIdY < 39)
					AddNodeToPathList(&pathNodes[nodeIdX - 1][nodeIdY + 1], pathNodeIndex + 7, pathNodesList);
			}
			if (nodeIdX < 39) {
				AddNodeToPathList(&pathNodes[nodeIdX + 1][nodeIdY], pathNodeIndex + 5, pathNodesList);
				if (nodeIdY > 0)
					AddNodeToPathList(&pathNodes[nodeIdX + 1][nodeIdY - 1], pathNodeIndex + 7, pathNodesList);
				if (nodeIdY < 39)
					AddNodeToPathList(&pathNodes[nodeIdX + 1][nodeIdY + 1], pathNodeIndex + 7, pathNodesList);
			}
			if (nodeIdY > 0)
				AddNodeToPathList(&pathNodes[nodeIdX][nodeIdY - 1], pathNodeIndex + 5, pathNodesList);
			if (nodeIdY < 39)
				AddNodeToPathList(&pathNodes[nodeIdX][nodeIdY + 1], pathNodeIndex + 5, pathNodesList);
			pPreviousNode = pPreviousNode->prev;
			if (!pPreviousNode)
				break;
		}

		if (pPreviousNode && pPreviousNode == pStartPathNode)
			break;
	}
	if (pathNodeIndex == 414)
		return false;
	CPedPathNode *pPathNode = pStartPathNode;
	for (*pointsFound = 0; pPathNode != pEndPathNode && *pointsFound < maxPoints; ++ *pointsFound) {
		const uint8 nodeIdX = pPathNode->nodeIdX;
		const uint8 nodeIdY = pPathNode->nodeIdY;
		if (nodeIdX > 0 && pathNodes[nodeIdX - 1][nodeIdY].id + 5 == pPathNode->id) 
			pPathNode = &pathNodes[nodeIdX - 1][nodeIdY];
		else if (nodeIdX > 39 && pathNodes[nodeIdX + 1][nodeIdY].id + 5 == pPathNode->id)
			pPathNode = &pathNodes[nodeIdX + 1][nodeIdY];
		else if (nodeIdY > 0 && pathNodes[nodeIdX][nodeIdY - 1].id + 5 == pPathNode->id) 
			pPathNode = &pathNodes[nodeIdX][nodeIdY - 1];
		else if (nodeIdY > 39 && pathNodes[nodeIdX][nodeIdY + 1].id + 5 == pPathNode->id) 
			pPathNode = &pathNodes[nodeIdX][nodeIdY + 1];
		else if (nodeIdX > 0 && nodeIdY > 0 && pathNodes[nodeIdX - 1][nodeIdY - 1].id + 7 == pPathNode->id)
			pPathNode = &pathNodes[nodeIdX - 1][nodeIdY - 1];
		else if (nodeIdX > 0 && nodeIdY < 39 && pathNodes[nodeIdX - 1][nodeIdY + 1].id + 7 == pPathNode->id)
			pPathNode = &pathNodes[nodeIdX - 1][nodeIdY + 1];
		else if (nodeIdX < 39 && nodeIdY > 0 && pathNodes[nodeIdX + 1][nodeIdY - 1].id + 7 == pPathNode->id)
			pPathNode = &pathNodes[nodeIdX + 1][nodeIdY - 1];
		else if (nodeIdX < 39 && nodeIdY < 39 && pathNodes[nodeIdX + 1][nodeIdY + 1].id + 7 == pPathNode->id)
			pPathNode = &pathNodes[nodeIdX + 1][nodeIdY + 1];
		pointPoses[*pointsFound] = vecSectorStartPos;
		pointPoses[*pointsFound].x += pPathNode->nodeIdX * 0.7f;
		pointPoses[*pointsFound].y += pPathNode->nodeIdY * 0.7f;
	}
	return true;
}


void 
CPedPath::AddNodeToPathList(CPedPathNode *pNodeToAdd, int16 id, CPedPathNode *pNodeList) 
{
	if (!pNodeToAdd->bBlockade && id < pNodeToAdd->id) {
		if (pNodeToAdd->id != INT16_MAX)
			RemoveNodeFromList(pNodeToAdd);
		AddNodeToList(pNodeToAdd, id, pNodeList);
	}
}

void 
CPedPath::RemoveNodeFromList(CPedPathNode *pNode)
{
	pNode->next->prev = pNode->prev;
	if (pNode->prev)
		pNode->prev->next = pNode->next;
}

void 
CPedPath::AddNodeToList(CPedPathNode *pNode, int16 index, CPedPathNode *pList)
{
	pNode->prev = pList[index].prev;
	pNode->next = &pList[index];
	if (pList[index].prev)
		pList[index].prev->next = pNode;
	pList[index].prev = pNode;
	pNode->id = index;
}

void
CPedPath::AddBlockadeSectorList(CPtrList& list, CPedPathNode(*pathNodes)[40], CVector *pPosition)
{
	CPtrNode* listNode = list.first;
	while (listNode) {
		CEntity* pEntity = (CEntity*)listNode->item;
		if (pEntity->m_scanCode != CWorld::GetCurrentScanCode() && pEntity->bUsesCollision) {
			pEntity->m_scanCode = CWorld::GetCurrentScanCode();
			AddBlockade(pEntity, pathNodes, pPosition);
		}
		listNode = listNode->next;
	}
}

void 
CPedPath::AddBlockade(CEntity *pEntity, CPedPathNode(*pathNodes)[40], CVector *pPosition)
{
	const CColBox& boundingBox = pEntity->GetColModel()->boundingBox;
	const float fBoundMaxY = boundingBox.max.y + 0.3f;
	const float fBoundMinY = boundingBox.min.y - 0.3f;
	const float fBoundMaxX = boundingBox.max.x + 0.3f;
	const float fDistanceX = pPosition->x - pEntity->m_matrix.GetPosition().x;
	const float fDistanceY = pPosition->y - pEntity->m_matrix.GetPosition().y;
	const float fBoundRadius = pEntity->GetBoundRadius();
	CVector vecBoundCentre;
	pEntity->GetBoundCentre(vecBoundCentre);
	if (vecBoundCentre.x + fBoundRadius >= pPosition->x && 
		vecBoundCentre.y + fBoundRadius >= pPosition->y &&
		vecBoundCentre.x - fBoundRadius <= pPosition->x + 28.0f &&
		vecBoundCentre.y - fBoundRadius <= pPosition->y + 28.0f) {
		for (int16 x = 0; x < 40; x++) {
			const float pointX = x * 0.7f + fDistanceX;
			for (int16 y = 0; y < 40; y++) {
				if (!pathNodes[x][y].bBlockade) {
					const float pointY = y * 0.7f + fDistanceY;
					CVector2D point(pointX, pointY);
					if (fBoundMaxX > Abs(DotProduct2D(point, pEntity->m_matrix.GetRight()))) {
						float fDotProduct = DotProduct2D(point, pEntity->m_matrix.GetForward());
						if (fBoundMaxY > fDotProduct && fBoundMinY < fDotProduct)
							pathNodes[x][y].bBlockade = true;
					}
				}
			}
		}
	}
}

void
CPathFind::Init(void)
{
	int i;

	m_numPathNodes = 0;
	m_numMapObjects = 0;
	m_numConnections = 0;
	m_numCarPathLinks = 0;
	unk = 0;

	for(i = 0; i < NUM_PATHNODES; i++)
		m_pathNodes[i].distance = MAX_DIST;
}

void
CPathFind::AllocatePathFindInfoMem(int16 numPathGroups)
{
	delete[] InfoForTileCars;
	InfoForTileCars = nil;
	delete[] InfoForTilePeds;
	InfoForTilePeds = nil;

	// NB: MIAMI doesn't use numPathGroups here but hardcodes 4500
	InfoForTileCars = new CPathInfoForObject[12*numPathGroups];
	memset(InfoForTileCars, 0, 12*numPathGroups*sizeof(CPathInfoForObject));
	InfoForTilePeds = new CPathInfoForObject[12*numPathGroups];
	memset(InfoForTilePeds, 0, 12*numPathGroups*sizeof(CPathInfoForObject));

	// unused
	delete[] DetachedNodesCars;
	DetachedNodesCars = nil;
	delete[] DetachedNodesPeds;
	DetachedNodesPeds = nil;
	DetachedNodesCars = new CTempDetachedNode[NUMDETACHED_CARS];
	memset(DetachedNodesCars, 0, NUMDETACHED_CARS*sizeof(CTempDetachedNode));
	DetachedNodesPeds = new CTempDetachedNode[NUMDETACHED_PEDS];
	memset(DetachedNodesPeds, 0, NUMDETACHED_PEDS*sizeof(CTempDetachedNode));
}

void
CPathFind::RegisterMapObject(CTreadable *mapObject)
{
	m_mapObjects[m_numMapObjects++] = mapObject;
}

void
CPathFind::StoreNodeInfoPed(int16 id, int16 node, int8 type, int8 next, int16 x, int16 y, int16 z, int16 width, bool crossing)
{
	int i, j;

	i = id*12 + node;
	InfoForTilePeds[i].type = type;
	InfoForTilePeds[i].next = next;
	InfoForTilePeds[i].x = x;
	InfoForTilePeds[i].y = y;
	InfoForTilePeds[i].z = z;
	InfoForTilePeds[i].numLeftLanes = 0;
	InfoForTilePeds[i].numRightLanes = 0;
	InfoForTilePeds[i].crossing = crossing;

	if(type)
		for(i = 0; i < node; i++){
			j = id*12 + i;
			if(x == InfoForTilePeds[j].x && y == InfoForTilePeds[j].y){
				printf("^^^^^^^^^^^^^ AARON IS TOO CHICKEN TO EAT MEAT!\n");
				printf("Several ped nodes on one road segment have identical coordinates (%d==%d && %d==%d)\n",
					x, InfoForTilePeds[j].x, y, InfoForTilePeds[j].y);
				printf("Modelindex of cullprit: %d\n\n", id);
			}
		}
}

void
CPathFind::StoreNodeInfoCar(int16 id, int16 node, int8 type, int8 next, int16 x, int16 y, int16 z, int16 width, int8 numLeft, int8 numRight)
{
	int i, j;

	i = id*12 + node;
	InfoForTileCars[i].type = type;
	InfoForTileCars[i].next = next;
	InfoForTileCars[i].x = x;
	InfoForTileCars[i].y = y;
	InfoForTileCars[i].z = z;
	InfoForTileCars[i].numLeftLanes = numLeft;
	InfoForTileCars[i].numRightLanes = numRight;


	if(type)
		for(i = 0; i < node; i++){
			j = id*12 + i;
			if(x == InfoForTileCars[j].x && y == InfoForTileCars[j].y){
				printf("^^^^^^^^^^^^^ AARON IS TOO CHICKEN TO EAT MEAT!\n");
				printf("Several car nodes on one road segment have identical coordinates (%d==%d && %d==%d)\n",
					x, InfoForTileCars[j].x, y, InfoForTileCars[j].y);
				printf("Modelindex of cullprit: %d\n\n", id);
			}
		}
}

void
CPathFind::CalcNodeCoors(int16 x, int16 y, int16 z, int id, CVector *out)
{
	CVector pos;
	pos.x = x / 16.0f;
	pos.y = y / 16.0f;
	pos.z = z / 16.0f;
	*out = m_mapObjects[id]->GetMatrix() * pos;
}

bool
CPathFind::LoadPathFindData(void)
{
	CFileMgr::SetDir("");
	return false;
}

void
CPathFind::PreparePathData(void)
{
	int i, j, k;
	int numExtern, numIntern, numLanes;
	float maxX, maxY;
	CTempNode *tempNodes;

	printf("PreparePathData\n");
	if(!CPathFind::LoadPathFindData() &&	// empty
	   InfoForTileCars && InfoForTilePeds &&
	   DetachedNodesCars && DetachedNodesPeds
		){
		tempNodes = new CTempNode[NUMTEMPNODES];

		m_numConnections = 0;

		for(i = 0; i < PATHNODESIZE; i++)
			m_pathNodes[i].unkBits = 0;

		for(i = 0; i < PATHNODESIZE; i++){
			numExtern = 0;
			numIntern = 0;
			for(j = 0; j < 12; j++){
				if(InfoForTileCars[i*12 + j].type == NodeTypeExtern)
					numExtern++;
				if(InfoForTileCars[i*12 + j].type == NodeTypeIntern)
					numIntern++;
			}
			if(numIntern > 1 && numExtern != 2)
				printf("ILLEGAL BLOCK. MORE THAN 1 INTERNALS AND NOT 2 EXTERNALS (Modelindex:%d)\n", i);
		}

		for(i = 0; i < PATHNODESIZE; i++)
			for(j = 0; j < 12; j++)
				if(InfoForTileCars[i*12 + j].type == NodeTypeExtern){
					// MIAMI has MI:%d here but no argument for it
					if(InfoForTileCars[i*12 + j].numLeftLanes < 0)
						printf("ILLEGAL BLOCK. NEGATIVE NUMBER OF LANES (Obj:%d)\n", i);
					if(InfoForTileCars[i*12 + j].numRightLanes < 0)
						printf("ILLEGAL BLOCK. NEGATIVE NUMBER OF LANES (Obj:%d)\n", i);
					if(InfoForTileCars[i*12 + j].numLeftLanes + InfoForTileCars[i*12 + j].numRightLanes <= 0)
						printf("ILLEGAL BLOCK. NO LANES IN NODE (Obj:%d)\n", i);
				}

		m_numPathNodes = 0;
		PreparePathDataForType(PATH_CAR, tempNodes, InfoForTileCars, 1.0f, DetachedNodesCars, NUMDETACHED_CARS);
		m_numCarPathNodes = m_numPathNodes;
		PreparePathDataForType(PATH_PED, tempNodes, InfoForTilePeds, 1.0f, DetachedNodesPeds, NUMDETACHED_PEDS);
		m_numPedPathNodes = m_numPathNodes - m_numCarPathNodes;

		// TODO: figure out what exactly is going on here
		// Some roads seem to get a west/east flag
		for(i = 0; i < m_numMapObjects; i++){
			numExtern = 0;
			numIntern = 0;
			numLanes = 0;
			maxX = 0.0f;
			maxY = 0.0f;
			for(j = 0; j < 12; j++){
				k = m_mapObjects[i]->GetModelIndex()*12 + j;
				if(InfoForTileCars[k].type == NodeTypeExtern){
					numExtern++;
					numLanes = Max(numLanes, InfoForTileCars[k].numLeftLanes + InfoForTileCars[k].numRightLanes);
					maxX = Max(maxX, Abs(InfoForTileCars[k].x));
					maxY = Max(maxY, Abs(InfoForTileCars[k].y));
				}else if(InfoForTileCars[k].type == NodeTypeIntern)
					numIntern++;
			}

			if(numIntern == 1 && numExtern == 2){
				if(numLanes < 4){
					if((i & 7) == 4){		// 1/8 probability
						m_objectFlags[i] |= UseInRoadBlock;
						if(maxX > maxY)
							m_objectFlags[i] |= ObjectEastWest;
						else
							m_objectFlags[i] &= ~ObjectEastWest;
					}
				}else{
					m_objectFlags[i] |= UseInRoadBlock;
					if(maxX > maxY)
						m_objectFlags[i] |= ObjectEastWest;
					else
						m_objectFlags[i] &= ~ObjectEastWest;
				}
			}
		}

		delete[] tempNodes;

		CountFloodFillGroups(PATH_CAR);
		CountFloodFillGroups(PATH_PED);

		delete[] InfoForTileCars;
		InfoForTileCars = nil;
		delete[] InfoForTilePeds;
		InfoForTilePeds = nil;

		delete[] DetachedNodesCars;
		DetachedNodesCars = nil;
		delete[] DetachedNodesPeds;
		DetachedNodesPeds = nil;
	}
	printf("Done with PreparePathData\n");
}

/* String together connected nodes in a list by a flood fill algorithm */
void
CPathFind::CountFloodFillGroups(uint8 type)
{
	int start, end;
	int i, l;
	uint16 n;
	CPathNode *node, *prev;

	switch(type){
	case PATH_CAR:
		start = 0;
		end = m_numCarPathNodes;
		break;
	case PATH_PED:
		start = m_numCarPathNodes;
		end = start + m_numPedPathNodes;
		break;
	}

	for(i = start; i < end; i++)
		m_pathNodes[i].group = 0;

	n = 0;
	for(;;){
		n++;
		if(n > 1500){
			for(i = start; m_pathNodes[i].group && i < end; i++);
			printf("NumNodes:%d Accounted for:%d\n", end - start, i - start);
		}

		// Look for unvisited node
		for(i = start; m_pathNodes[i].group && i < end; i++);
		if(i == end)
			break;

		node = &m_pathNodes[i];
		node->SetNext(nil);
		node->group = n;

		if(node->numLinks == 0){
			if(type == PATH_CAR)
				printf("Single car node: %f %f %f (%d)\n",
					node->GetX(), node->GetY(), node->GetZ(), m_mapObjects[node->objectIndex]->GetModelIndex());
			else
				printf("Single ped node: %f %f %f\n",
					node->GetX(), node->GetY(), node->GetZ());
		}

		while(node){
			prev = node;
			node = node->GetNext();
			for(i = 0; i < prev->numLinks; i++){
				l = ConnectedNode(prev->firstLink + i);
				if(m_pathNodes[l].group == 0){
					m_pathNodes[l].group = n;
					if(m_pathNodes[l].group == 0)
						m_pathNodes[l].group = INT8_MIN;
					m_pathNodes[l].SetNext(node);
					node = &m_pathNodes[l];
				}
			}
		}
	}

	m_numGroups[type] = n-1;
	printf("GraphType:%d. FloodFill groups:%d\n", type, n);
}

int32 TempListLength;

void
CPathFind::PreparePathDataForType(uint8 type, CTempNode *tempnodes, CPathInfoForObject *objectpathinfo,
	float maxdist, CTempDetachedNode *detachednodes, int numDetached)
{
	static CVector CoorsXFormed;
	int i, j, k, l;
	int l1, l2;
	int start;
	float posx, posy;
	float dx, dy, mag;
	float nearestDist;
	int nearestId;
	int next;
	int oldNumPathNodes, oldNumLinks;
	float dist;
	int iseg, jseg;
	int istart, jstart;
	int done, cont;
	int tileStart;

#ifndef MASTER
	for (i = 0; i < m_numMapObjects-1; i++)
		for (j = i+1; j < m_numMapObjects; j++) {
			CTreadable *obj1 = m_mapObjects[i];
			CTreadable *obj2 = m_mapObjects[j];
			if (obj1->GetModelIndex() == obj2->GetModelIndex() &&
				obj1->GetPosition().x == obj2->GetPosition().x && obj1->GetPosition().y == obj2->GetPosition().y && obj1->GetPosition().z == obj2->GetPosition().z &&
				obj1->GetRight().x == obj2->GetRight().x && obj1->GetForward().x == obj2->GetForward().x && obj1->GetUp().x == obj2->GetUp().x &&
				obj1->GetRight().y == obj2->GetRight().y && obj1->GetForward().y == obj2->GetForward().y && obj1->GetUp().y == obj2->GetUp().y &&
				obj1->GetRight().z == obj2->GetRight().z && obj1->GetForward().z == obj2->GetForward().z && obj1->GetUp().z == obj2->GetUp().z) {
					printf("THIS IS VERY BAD INDEED. FIX IMMEDIATELY!!!\n");
					printf("Double road objects at the following coors: %f %f %f\n", obj1->GetPosition().x, obj1->GetPosition().y, obj1->GetPosition().z);
				}
		}
#endif // !MASTER

	oldNumPathNodes = m_numPathNodes;
	oldNumLinks = m_numConnections;

#define OBJECTINDEX(n) (m_pathNodes[(n)].objectIndex)
	// Initialize map objects
	for(i = 0; i < m_numMapObjects; i++)
		for(j = 0; j < 12; j++)
			m_mapObjects[i]->m_nodeIndices[type][j] = -1;

	// Calculate internal nodes, store them and connect them to defining object
	for(i = 0; i < m_numMapObjects; i++){
		tileStart = m_numPathNodes;
		start = 12 * m_mapObjects[i]->GetModelIndex();
		for(j = 0; j < 12; j++){
			if(objectpathinfo[start + j].type == NodeTypeIntern){
				CalcNodeCoors(
					objectpathinfo[start + j].x,
					objectpathinfo[start + j].y,
					objectpathinfo[start + j].z,
					i,
					&CoorsXFormed);
				m_pathNodes[m_numPathNodes].SetPosition(CoorsXFormed);
				OBJECTINDEX(m_numPathNodes) = i;
				m_pathNodes[m_numPathNodes].unkBits = 1;
				m_mapObjects[i]->m_nodeIndices[type][j] = m_numPathNodes;
				m_numPathNodes++;
			}
		}
	}


	// Insert external nodes into TempList
	TempListLength = 0;
	for(i = 0; i < m_numMapObjects; i++){
		start = 12 * m_mapObjects[i]->GetModelIndex();
		for(j = 0; j < 12; j++){
			if(objectpathinfo[start + j].type != NodeTypeExtern)
				continue;
			CalcNodeCoors(
				objectpathinfo[start + j].x,
				objectpathinfo[start + j].y,
				objectpathinfo[start + j].z,
				i,
				&CoorsXFormed);

			// find closest unconnected node
			nearestId = -1;
			nearestDist = maxdist;
			for(k = 0; k < TempListLength; k++){
				if(tempnodes[k].linkState != 1)
					continue;
				dx = tempnodes[k].pos.x - CoorsXFormed.x;
				if(Abs(dx) < nearestDist){
					dy = tempnodes[k].pos.y - CoorsXFormed.y;
					if(Abs(dy) < nearestDist){
						nearestDist = Max(Abs(dx), Abs(dy));
						nearestId = k;
					}
				}
			}

			if(nearestId < 0){
				// None found, add this one to temp list
				tempnodes[TempListLength].pos = CoorsXFormed;
				next = objectpathinfo[start + j].next;
				if(next < 0){
					// no link from this node, find link to this node
					next = 0;
					for(k = start; j != objectpathinfo[k].next; k++)
						next++;
				}
				// link to connecting internal node
				tempnodes[TempListLength].link1 = m_mapObjects[i]->m_nodeIndices[type][next];
				if(type == PATH_CAR){
					tempnodes[TempListLength].numLeftLanes = objectpathinfo[start + j].numLeftLanes;
					tempnodes[TempListLength].numRightLanes = objectpathinfo[start + j].numRightLanes;
				}
				tempnodes[TempListLength++].linkState = 1;
			}else{
				// Found nearest, connect it to our neighbour
				next = objectpathinfo[start + j].next;
				if(next < 0){
					// no link from this node, find link to this node
					next = 0;
					for(k = start; j != objectpathinfo[k].next; k++)
						next++;
				}
				tempnodes[nearestId].link2 = m_mapObjects[i]->m_nodeIndices[type][next];
				tempnodes[nearestId].linkState = 2;

				// collapse this node with nearest we found
				dx = m_pathNodes[tempnodes[nearestId].link1].GetX() - m_pathNodes[tempnodes[nearestId].link2].GetX();
				dy = m_pathNodes[tempnodes[nearestId].link1].GetY() - m_pathNodes[tempnodes[nearestId].link2].GetY();
				tempnodes[nearestId].pos = (tempnodes[nearestId].pos + CoorsXFormed)*0.5f;
				mag = Sqrt(dx*dx + dy*dy);
				tempnodes[nearestId].dirX = dx/mag;
				tempnodes[nearestId].dirY = dy/mag;
				// do something when number of lanes doesn't agree
				if(type == PATH_CAR)
					if(tempnodes[nearestId].numLeftLanes != 0 && tempnodes[nearestId].numRightLanes != 0 &&
					   (objectpathinfo[start + j].numLeftLanes == 0 || objectpathinfo[start + j].numRightLanes == 0)){
						// why switch left and right here?
						tempnodes[nearestId].numLeftLanes = objectpathinfo[start + j].numRightLanes;
						tempnodes[nearestId].numRightLanes = objectpathinfo[start + j].numLeftLanes;
					}
			}
		}
	}

	// Loop through previously added internal nodes and link them
	for(i = oldNumPathNodes; i < m_numPathNodes; i++){
		// Init link
		m_pathNodes[i].numLinks = 0;
		m_pathNodes[i].firstLink = m_numConnections;

		// See if node connects to external nodes
		for(j = 0; j < TempListLength; j++){
			if(tempnodes[j].linkState != 2)
				continue;

			// Add link to other side of the external
			// NB this clears the flags in MIAMI
			if(tempnodes[j].link1 == i)
				m_connections[m_numConnections] = tempnodes[j].link2;
			else if(tempnodes[j].link2 == i)
				m_connections[m_numConnections] = tempnodes[j].link1;
			else
				continue;

			dist = (m_pathNodes[i].GetPosition() - m_pathNodes[ConnectedNode(m_numConnections)].GetPosition()).Magnitude();
			m_distances[m_numConnections] = dist;
			m_connectionFlags[m_numConnections].flags = 0;

			if(type == PATH_CAR){
				// IMPROVE: use a goto here
				// Find existing car path link
				for(k = 0; k < m_numCarPathLinks; k++){
					if(m_carPathLinks[k].dir.x == tempnodes[j].dirX &&
					   m_carPathLinks[k].dir.y == tempnodes[j].dirY &&
					   m_carPathLinks[k].pos.x == tempnodes[j].pos.x &&
					   m_carPathLinks[k].pos.y == tempnodes[j].pos.y){
						m_carPathConnections[m_numConnections] = k;
						k = m_numCarPathLinks;
					}
				}
				// k is m_numCarPathLinks+1 if we found one
				if(k == m_numCarPathLinks){
					m_carPathLinks[m_numCarPathLinks].dir.x = tempnodes[j].dirX;
					m_carPathLinks[m_numCarPathLinks].dir.y = tempnodes[j].dirY;
					m_carPathLinks[m_numCarPathLinks].pos.x = tempnodes[j].pos.x;
					m_carPathLinks[m_numCarPathLinks].pos.y = tempnodes[j].pos.y;
					m_carPathLinks[m_numCarPathLinks].pathNodeIndex = i;
					m_carPathLinks[m_numCarPathLinks].numLeftLanes = tempnodes[j].numLeftLanes;
					m_carPathLinks[m_numCarPathLinks].numRightLanes = tempnodes[j].numRightLanes;
					m_carPathLinks[m_numCarPathLinks].trafficLightType = 0;
					assert(m_numCarPathLinks <= NUM_CARPATHLINKS);
					m_carPathConnections[m_numConnections] = m_numCarPathLinks++;
				}
			}

			m_pathNodes[i].numLinks++;
			m_numConnections++;
		}


		// Find i inside path segment
		iseg = 0;
		for(j = Max(oldNumPathNodes, i-12); j < i; j++)
			if(OBJECTINDEX(j) == OBJECTINDEX(i))
				iseg++;

		istart = 12 * m_mapObjects[m_pathNodes[i].objectIndex]->GetModelIndex();
		// Add links to other internal nodes
		for(j = Max(oldNumPathNodes, i-12); j < Min(m_numPathNodes, i+12); j++){
			if(OBJECTINDEX(i) != OBJECTINDEX(j) || i == j)
				continue;
			// N.B.: in every path segment, the externals have to be at the end
			jseg = j-i + iseg;

			jstart = 12 * m_mapObjects[m_pathNodes[j].objectIndex]->GetModelIndex();
			if(objectpathinfo[istart + iseg].next == jseg ||
			   objectpathinfo[jstart + jseg].next == iseg){
				// Found a link between i and jConnectionSetCrossesRoad
				// NB this clears the flags in MIAMI
				m_connections[m_numConnections] = j;
				dist = (m_pathNodes[i].GetPosition() - m_pathNodes[j].GetPosition()).Magnitude();
				m_distances[m_numConnections] = dist;

				if(type == PATH_CAR){
					posx = (m_pathNodes[i].GetX() + m_pathNodes[j].GetX())*0.5f;
					posy = (m_pathNodes[i].GetY() + m_pathNodes[j].GetY())*0.5f;
					dx = m_pathNodes[j].GetX() - m_pathNodes[i].GetX();
					dy = m_pathNodes[j].GetY() - m_pathNodes[i].GetY();
					mag = Sqrt(dx*dx + dy*dy);
					dx /= mag;
					dy /= mag;
					if(i < j){
						dx = -dx;
						dy = -dy;
					}
					// IMPROVE: use a goto here
					// Find existing car path link
					for(k = 0; k < m_numCarPathLinks; k++){
						if(m_carPathLinks[k].dir.x == dx &&
						   m_carPathLinks[k].dir.y == dy &&
						   m_carPathLinks[k].pos.x == posx &&
						   m_carPathLinks[k].pos.y == posy){
							m_carPathConnections[m_numConnections] = k;
							k = m_numCarPathLinks;
						}
					}
					// k is m_numCarPathLinks+1 if we found one
					if(k == m_numCarPathLinks){
						m_carPathLinks[m_numCarPathLinks].dir.x = dx;
						m_carPathLinks[m_numCarPathLinks].dir.y = dy;
						m_carPathLinks[m_numCarPathLinks].pos.x = posx;
						m_carPathLinks[m_numCarPathLinks].pos.y = posy;
						m_carPathLinks[m_numCarPathLinks].pathNodeIndex = i;
						m_carPathLinks[m_numCarPathLinks].numLeftLanes = -1;
						m_carPathLinks[m_numCarPathLinks].numRightLanes = -1;
						m_carPathLinks[m_numCarPathLinks].trafficLightType = 0;
						assert(m_numCarPathLinks <= NUM_CARPATHLINKS);
						m_carPathConnections[m_numConnections] = m_numCarPathLinks++;
					}
				}else{
					// Crosses road
					if(objectpathinfo[istart + iseg].next == jseg && objectpathinfo[istart + iseg].crossing ||
					   objectpathinfo[jstart + jseg].next == iseg && objectpathinfo[jstart + jseg].crossing)
						m_connectionFlags[m_numConnections].bCrossesRoad = true;
					else
						m_connectionFlags[m_numConnections].bCrossesRoad = false;
				}

				m_pathNodes[i].numLinks++;
				m_numConnections++;
			}
		}
	}

	if(type == PATH_CAR){
		done = 0;
		// Set number of lanes for all nodes somehow
		// very strange code
		for(k = 0; !done && k < 10; k++){
			done = 1;
			for(i = 0; i < m_numPathNodes; i++){
				if(m_pathNodes[i].numLinks != 2)
					continue;
				l1 = m_carPathConnections[m_pathNodes[i].firstLink];
				l2 = m_carPathConnections[m_pathNodes[i].firstLink+1];

				if(m_carPathLinks[l1].numLeftLanes == -1 &&
				   m_carPathLinks[l2].numLeftLanes != -1){
					done = 0;
					if(m_carPathLinks[l2].pathNodeIndex == i){
						// why switch left and right here?
						m_carPathLinks[l1].numLeftLanes = m_carPathLinks[l2].numRightLanes;
						m_carPathLinks[l1].numRightLanes = m_carPathLinks[l2].numLeftLanes;
					}else{
						m_carPathLinks[l1].numLeftLanes = m_carPathLinks[l2].numLeftLanes;
						m_carPathLinks[l1].numRightLanes = m_carPathLinks[l2].numRightLanes;
					}
					m_carPathLinks[l1].pathNodeIndex = i;
				}else if(m_carPathLinks[l1].numLeftLanes != -1 &&
				         m_carPathLinks[l2].numLeftLanes == -1){
					done = 0;
					if(m_carPathLinks[l1].pathNodeIndex == i){
						// why switch left and right here?
						m_carPathLinks[l2].numLeftLanes = m_carPathLinks[l1].numRightLanes;
						m_carPathLinks[l2].numRightLanes = m_carPathLinks[l1].numLeftLanes;
					}else{
						m_carPathLinks[l2].numLeftLanes = m_carPathLinks[l1].numLeftLanes;
						m_carPathLinks[l2].numRightLanes = m_carPathLinks[l1].numRightLanes;
					}
					m_carPathLinks[l2].pathNodeIndex = i;
				}else if(m_carPathLinks[l1].numLeftLanes == -1 &&
				         m_carPathLinks[l2].numLeftLanes == -1)
					done = 0;
			}
		}

		// Fall back to default values for number of lanes
		for(i = 0; i < m_numPathNodes; i++)
			for(j = 0; j < m_pathNodes[i].numLinks; j++){
				k = m_carPathConnections[m_pathNodes[i].firstLink + j];
				if(m_carPathLinks[k].numLeftLanes < 0)
					m_carPathLinks[k].numLeftLanes = 1;
				if(m_carPathLinks[k].numRightLanes < 0)
					m_carPathLinks[k].numRightLanes = 1;
			}
	}

	// Set flags for car nodes
	if(type == PATH_CAR){
		do{
			cont = 0;
			for(i = 0; i < m_numPathNodes; i++){
				m_pathNodes[i].bDisabled = false;
				m_pathNodes[i].bBetweenLevels = false;
				// See if node is a dead end, if so, we're not done yet
				if(!m_pathNodes[i].bDeadEnd){
					k = 0;
					for(j = 0; j < m_pathNodes[i].numLinks; j++)
						if(!m_pathNodes[ConnectedNode(m_pathNodes[i].firstLink + j)].bDeadEnd)
							k++;
					if(k < 2){
						m_pathNodes[i].bDeadEnd = true;
						cont = 1;
					}
				}
			}
		}while(cont);
	}

	// Remove isolated ped nodes
	if(type == PATH_PED)
		for(i = oldNumPathNodes; i < m_numPathNodes; i++){
			if(m_pathNodes[i].numLinks != 0)
				continue;

			// Remove node
			for(j = i; j < m_numPathNodes-1; j++)
				m_pathNodes[j] = m_pathNodes[j+1];

			// Fix links
			for(j = oldNumLinks; j < m_numConnections; j++){
				int node = ConnectedNode(j);
				if(node >= i)
					m_connections[j] = node-1;
			}

			// Also in treadables
			for(j = 0; j < m_numMapObjects; j++)
				for(k = 0; k < 12; k++){
					if(m_mapObjects[j]->m_nodeIndices[PATH_PED][k] == i){
						// remove this one
						for(l = k; l < 12-1; l++)
							m_mapObjects[j]->m_nodeIndices[PATH_PED][l] = m_mapObjects[j]->m_nodeIndices[PATH_PED][l+1];
						m_mapObjects[j]->m_nodeIndices[PATH_PED][11] = -1;
					}else if(m_mapObjects[j]->m_nodeIndices[PATH_PED][k] > i)
						m_mapObjects[j]->m_nodeIndices[PATH_PED][k]--;
				}

			i--;
			m_numPathNodes--;
		}
}

float
CPathFind::CalcRoadDensity(float x, float y)
{
	int i, j;
	float density = 0.0f;

	for(i = 0; i < m_numCarPathNodes; i++){
		if(Abs(m_pathNodes[i].GetX() - x) < 80.0f &&
		   Abs(m_pathNodes[i].GetY() - y) < 80.0f &&
		   m_pathNodes[i].numLinks > 0){
			for(j = 0; j < m_pathNodes[i].numLinks; j++){
				int next = ConnectedNode(m_pathNodes[i].firstLink + j);
				float dist = (m_pathNodes[i].GetPosition() - m_pathNodes[next].GetPosition()).Magnitude2D();
				next = m_carPathConnections[m_pathNodes[i].firstLink + j];
				density += m_carPathLinks[next].numLeftLanes * dist;
				density += m_carPathLinks[next].numRightLanes * dist;

				if(m_carPathLinks[next].numLeftLanes < 0)
					printf("Link from object %d to %d (MIs)\n",
						m_mapObjects[m_pathNodes[i].objectIndex]->GetModelIndex(),
						m_mapObjects[m_pathNodes[ConnectedNode(m_pathNodes[i].firstLink + j)].objectIndex]->GetModelIndex());
				if(m_carPathLinks[next].numRightLanes < 0)
					printf("Link from object %d to %d (MIs)\n",
						m_mapObjects[m_pathNodes[i].objectIndex]->GetModelIndex(),
						m_mapObjects[m_pathNodes[ConnectedNode(m_pathNodes[i].firstLink + j)].objectIndex]->GetModelIndex());
			}
		}
	}
	return density/2500.0f;
}

bool
CPathFind::TestForPedTrafficLight(CPathNode *n1, CPathNode *n2)
{
	int i;
	for(i = 0; i < n1->numLinks; i++)
		if(&m_pathNodes[ConnectedNode(n1->firstLink + i)] == n2)
			return ConnectionHasTrafficLight(n1->firstLink + i);
	return false;
}

bool
CPathFind::TestCrossesRoad(CPathNode *n1, CPathNode *n2)
{
	int i;
	for(i = 0; i < n1->numLinks; i++)
		if(&m_pathNodes[ConnectedNode(n1->firstLink + i)] == n2)
			return ConnectionCrossesRoad(n1->firstLink + i);
	return false;
}

void
CPathFind::AddNodeToList(CPathNode *node, int32 listId)
{
	int i = listId & 0x1FF;
	node->SetNext(m_searchNodes[i].GetNext());
	node->SetPrev(&m_searchNodes[i]);
	if(m_searchNodes[i].GetNext())
		m_searchNodes[i].GetNext()->SetPrev(node);
	m_searchNodes[i].SetNext(node);
	node->distance = listId;
}

void
CPathFind::RemoveNodeFromList(CPathNode *node)
{
	node->GetPrev()->SetNext(node->GetNext());
	if(node->GetNext())
		node->GetNext()->SetPrev(node->GetPrev());
}

void
CPathFind::RemoveBadStartNode(CVector pos, CPathNode **nodes, int16 *n)
{
	int i;
	if(*n < 2)
		return;
	if(DotProduct2D(nodes[1]->GetPosition() - pos, nodes[0]->GetPosition() - pos) < 0.0f){
		(*n)--;
		for(i = 0; i < *n; i++)
			nodes[i] = nodes[i+1];
	}
}

void
CPathFind::SetLinksBridgeLights(float x1, float x2, float y1, float y2, bool enable)
{
	int i;
	for(i = 0; i < m_numCarPathLinks; i++){
		CVector2D pos = m_carPathLinks[i].GetPosition();
		if(x1 < pos.x && pos.x < x2 &&
		   y1 < pos.y && pos.y < y2)
			m_carPathLinks[i].bBridgeLights = enable;
	}
}

void
CPathFind::SwitchOffNodeAndNeighbours(int32 nodeId, bool disable)
{
	int i, next;

	m_pathNodes[nodeId].bDisabled = disable;
	if(m_pathNodes[nodeId].numLinks < 3)
		for(i = 0; i < m_pathNodes[nodeId].numLinks; i++){
			next = ConnectedNode(m_pathNodes[nodeId].firstLink + i);
			if(m_pathNodes[next].bDisabled != disable &&
			   m_pathNodes[next].numLinks < 3)
				SwitchOffNodeAndNeighbours(next, disable);
		}
}

void
CPathFind::SwitchRoadsOffInArea(float x1, float x2, float y1, float y2, float z1, float z2, bool disable)
{
	int i;

	for(i = 0; i < m_numCarPathNodes; i++){
		CVector pos = m_pathNodes[i].GetPosition();
		if(x1 <= pos.x && pos.x <= x2 &&
		   y1 <= pos.y && pos.y <= y2 &&
		   z1 <= pos.z && pos.z <= z2 &&
			disable != m_pathNodes[i].bDisabled)
			SwitchOffNodeAndNeighbours(i, disable);
	}
}

void
CPathFind::SwitchPedRoadsOffInArea(float x1, float x2, float y1, float y2, float z1, float z2, bool disable)
{
	int i;

	for(i = m_numCarPathNodes; i < m_numPathNodes; i++){
		CVector pos = m_pathNodes[i].GetPosition();
		if(x1 <= pos.x && pos.x <= x2 &&
		   y1 <= pos.y && pos.y <= y2 &&
		   z1 <= pos.z && pos.z <= z2 &&
			disable != m_pathNodes[i].bDisabled)
			SwitchOffNodeAndNeighbours(i, disable);
	}
}

void
CPathFind::SwitchRoadsInAngledArea(float x1, float y1, float z1, float x2, float y2, float z2, float length, uint8 type, uint8 mode)
{
	int i;
	int firstNode, lastNode;

	// this is NOT PATH_CAR
	if(type != 0){
		firstNode = 0;
		lastNode = m_numCarPathNodes;
	}else{
		firstNode = m_numCarPathNodes;
		lastNode = m_numPathNodes;
	}

	if(z1 > z2){
		float tmp = z2;
		z2 = z1;
		z1 = tmp;
	}

	// angle of vector from p2 to p1
	float angle = CGeneral::GetRadianAngleBetweenPoints(x1, y1, x2, y2) + HALFPI;
	while(angle < 0.0f) angle += TWOPI;
	while(angle > TWOPI) angle -= TWOPI;
	// vector from p1 to p2
	CVector2D v12(x2 - x1, y2 - y1);
	float len12 = v12.Magnitude();
	v12 /= len12;

	// vector from p2 to new point p3
	CVector2D v23(Sin(angle)*length, -(Cos(angle)*length));
	v23 /= v23.Magnitude();	// obivously just 'length' but whatever

	bool disable = mode == SWITCH_OFF;
	for(i = firstNode; i < lastNode; i++){
		CVector pos = m_pathNodes[i].GetPosition();
		if(pos.z < z1 || pos.z > z2)
			continue;
		CVector2D d(pos.x - x1, pos.y - y1);
		float dot = DotProduct2D(d, v12);
		if(dot < 0.0f || dot > len12)
			continue;
		dot = DotProduct2D(d, v23);
		if(dot < 0.0f || dot > length)
			continue;
		if(m_pathNodes[i].bDisabled != disable)
			SwitchOffNodeAndNeighbours(i, disable);
	}
}

void
CPathFind::MarkRoadsBetweenLevelsNodeAndNeighbours(int32 nodeId)
{
	int i, next;

	m_pathNodes[nodeId].bBetweenLevels = true;
	if(m_pathNodes[nodeId].numLinks < 3)
		for(i = 0; i < m_pathNodes[nodeId].numLinks; i++){
			next = ConnectedNode(m_pathNodes[nodeId].firstLink + i);
			if(!m_pathNodes[next].bBetweenLevels &&
			   m_pathNodes[next].numLinks < 3)
				MarkRoadsBetweenLevelsNodeAndNeighbours(next);
		}
}

void
CPathFind::MarkRoadsBetweenLevelsInArea(float x1, float x2, float y1, float y2, float z1, float z2)
{
	int i;

	for(i = 0; i < m_numPathNodes; i++){
		CVector pos = m_pathNodes[i].GetPosition();
		if(x1 < pos.x && pos.x < x2 &&
		   y1 < pos.y && pos.y < y2 &&
		   z1 < pos.z && pos.z < z2)
			MarkRoadsBetweenLevelsNodeAndNeighbours(i);
	}
}

void
CPathFind::PedMarkRoadsBetweenLevelsInArea(float x1, float x2, float y1, float y2, float z1, float z2)
{
	int i;

	for(i = m_numCarPathNodes; i < m_numPathNodes; i++){
		CVector pos = m_pathNodes[i].GetPosition();
		if(x1 < pos.x && pos.x < x2 &&
		   y1 < pos.y && pos.y < y2 &&
		   z1 < pos.z && pos.z < z2)
			MarkRoadsBetweenLevelsNodeAndNeighbours(i);
	}
}

int32
CPathFind::FindNodeClosestToCoors(CVector coors, uint8 type, float distLimit, bool ignoreDisabled, bool ignoreBetweenLevels)
{
	int i;
	int firstNode, lastNode;
	float dist;
	float closestDist = 10000.0f;
	int closestNode = 0;

	switch(type){
	case PATH_CAR:
		firstNode = 0;
		lastNode = m_numCarPathNodes;
		break;
	case PATH_PED:
		firstNode = m_numCarPathNodes;
		lastNode = m_numPathNodes;
		break;
	}

	for(i = firstNode; i < lastNode; i++){
		if(ignoreDisabled && m_pathNodes[i].bDisabled) continue;
		if(ignoreBetweenLevels && m_pathNodes[i].bBetweenLevels) continue;
		switch(m_pathNodes[i].unkBits){
		case 1:
		case 2:
			dist = Abs(m_pathNodes[i].GetX() - coors.x) +
			       Abs(m_pathNodes[i].GetY() - coors.y) +
			       3.0f*Abs(m_pathNodes[i].GetZ() - coors.z);
			if(dist < closestDist){
				closestDist = dist;
				closestNode = i;
			}
			break;
		}
	}
	return closestDist < distLimit ? closestNode : -1;
}

int32
CPathFind::FindNodeClosestToCoorsFavourDirection(CVector coors, uint8 type, float dirX, float dirY)
{
	int i;
	int firstNode, lastNode;
	float dist, dX, dY;
	NormalizeXY(dirX, dirY);
	float closestDist = 10000.0f;
	int closestNode = 0;

	switch(type){
	case PATH_CAR:
		firstNode = 0;
		lastNode = m_numCarPathNodes;
		break;
	case PATH_PED:
		firstNode = m_numCarPathNodes;
		lastNode = m_numPathNodes;
		break;
	}

	for(i = firstNode; i < lastNode; i++){
		switch(m_pathNodes[i].unkBits){
		case 1:
		case 2:
			dX = m_pathNodes[i].GetX() - coors.x;
			dY = m_pathNodes[i].GetY() - coors.y;
			dist = Abs(dX) + Abs(dY) +
			       3.0f*Abs(m_pathNodes[i].GetZ() - coors.z);
			if(dist < closestDist){
				NormalizeXY(dX, dY);
				dist -= (dX*dirX + dY*dirY - 1.0f)*20.0f;
				if(dist < closestDist){
					closestDist = dist;
					closestNode = i;
				}
			}
			break;
		}
	}
	return closestNode;
}

float
CPathFind::FindNodeOrientationForCarPlacement(int32 nodeId)
{
	if(m_pathNodes[nodeId].numLinks == 0)
		return 0.0f;
	CVector dir = m_pathNodes[ConnectedNode(m_pathNodes[nodeId].firstLink)].GetPosition() - m_pathNodes[nodeId].GetPosition();
	dir.z = 0.0f;
	dir.Normalise();
	return RADTODEG(dir.Heading());
}

float
CPathFind::FindNodeOrientationForCarPlacementFacingDestination(int32 nodeId, float x, float y, bool towards)
{
	int i;

	CVector targetDir(x - m_pathNodes[nodeId].GetX(), y - m_pathNodes[nodeId].GetY(), 0.0f);
	targetDir.Normalise();
	CVector dir;

	if(m_pathNodes[nodeId].numLinks == 0)
		return 0.0f;

	int bestNode = ConnectedNode(m_pathNodes[nodeId].firstLink);
#ifdef FIX_BUGS
	float bestDot = towards ? -2.0f : 2.0f;
#else
	int bestDot = towards ? -2 : 2;	// why int?
#endif

	for(i = 0; i < m_pathNodes[nodeId].numLinks; i++){
		dir = m_pathNodes[ConnectedNode(m_pathNodes[nodeId].firstLink + i)].GetPosition() - m_pathNodes[nodeId].GetPosition();
		dir.z = 0.0f;
		dir.Normalise();
		float angle = DotProduct2D(dir, targetDir);
		if(towards){
			if(angle > bestDot){
				bestDot = angle;
				bestNode = ConnectedNode(m_pathNodes[nodeId].firstLink + i);
			}
		}else{
			if(angle < bestDot){
				bestDot = angle;
				bestNode = ConnectedNode(m_pathNodes[nodeId].firstLink + i);
			}
		}
	}

	dir = m_pathNodes[bestNode].GetPosition() - m_pathNodes[nodeId].GetPosition();
	dir.z = 0.0f;
	dir.Normalise();
	return RADTODEG(dir.Heading());
}

bool
CPathFind::NewGenerateCarCreationCoors(float x, float y, float dirX, float dirY, float spawnDist, float angleLimit, bool forward, CVector *pPosition, int32 *pNode1, int32 *pNode2, float *pPositionBetweenNodes, bool ignoreDisabled)
{
	int i, j;
	int node1, node2;
	float dist1, dist2, d1, d2;

	if(m_numCarPathNodes == 0)
		return false;

	for(i = 0; i < 500; i++){
		node1 = (CGeneral::GetRandomNumber()>>3) % m_numCarPathNodes;
		if(m_pathNodes[node1].bDisabled && !ignoreDisabled)
			continue;
		dist1 = Distance2D(m_pathNodes[node1].GetPosition(), x, y);
		if(dist1 < spawnDist + 60.0f){
			d1 = dist1 - spawnDist;
			for(j = 0; j < m_pathNodes[node1].numLinks; j++){
				node2 = ConnectedNode(m_pathNodes[node1].firstLink + j);
				if(m_pathNodes[node2].bDisabled && !ignoreDisabled)
					continue;
				dist2 = Distance2D(m_pathNodes[node2].GetPosition(), x, y);
				d2 = dist2 - spawnDist;
				if(d1*d2 < 0.0f){
					// nodes are on different sides of spawn distance
					float f2 = Abs(d1)/(Abs(d1) + Abs(d2));
					float f1 = 1.0f - f2;
					*pPositionBetweenNodes = f2;
					CVector pos = m_pathNodes[node1].GetPosition()*f1 + m_pathNodes[node2].GetPosition()*f2;
					CVector2D dist2d(pos.x - x, pos.y - y);
					dist2d.Normalise();	// done manually in the game
					float dot = DotProduct2D(dist2d, CVector2D(dirX, dirY));
					if(forward){
						if(dot > angleLimit){
							*pNode1 = node1;
							*pNode2 = node2;
							*pPosition = pos;
							return true;
						}
					}else{
						if(dot <= angleLimit){
							*pNode1 = node1;
							*pNode2 = node2;
							*pPosition = pos;
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool
CPathFind::GeneratePedCreationCoors(float x, float y, float minDist, float maxDist, float minDistOffScreen, float maxDistOffScreen, CVector *pPosition, int32 *pNode1, int32 *pNode2, float *pPositionBetweenNodes, CMatrix *camMatrix)
{
	int i;
	int node1, node2;

	if(m_numPedPathNodes == 0)
		return false;

	for(i = 0; i < 400; i++){
		node1 = m_numCarPathNodes + CGeneral::GetRandomNumber() % m_numPedPathNodes;
		if(DistanceSqr2D(m_pathNodes[node1].GetPosition(), x, y) < sq(maxDist+30.0f)){
			if(m_pathNodes[node1].numLinks == 0)
				continue;
			int link = m_pathNodes[node1].firstLink + CGeneral::GetRandomNumber() % m_pathNodes[node1].numLinks;
			if(ConnectionCrossesRoad(link))
				continue;
			node2 = ConnectedNode(link);
			if(m_pathNodes[node1].bDisabled || m_pathNodes[node2].bDisabled)
				continue;

			float f2 = (CGeneral::GetRandomNumber()&0xFF)/256.0f;
			float f1 = 1.0f - f2;
			*pPositionBetweenNodes = f2;
			CVector pos = m_pathNodes[node1].GetPosition()*f1 + m_pathNodes[node2].GetPosition()*f2;
			if(Distance2D(pos, x, y) < maxDist+20.0f){
				pos.x += ((CGeneral::GetRandomNumber()&0xFF)-128)*0.01f;
				pos.y += ((CGeneral::GetRandomNumber()&0xFF)-128)*0.01f;
				float dist = Distance2D(pos, x, y);

				bool visible;
				if(camMatrix)
					visible = TheCamera.IsSphereVisible(pos, 2.0f, camMatrix);
				else
					visible = TheCamera.IsSphereVisible(pos, 2.0f);
				if(!visible){
					minDist = minDistOffScreen;
					maxDist = maxDistOffScreen;
				}
				if(minDist < dist && dist < maxDist){
					*pNode1 = node1;
					*pNode2 = node2;
					*pPosition = pos;

					bool found;
					float groundZ = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z+2.0f, &found);
					if(!found)
						return false;
					if(Abs(groundZ - pos.z) > 3.0f)
						return false;
					pPosition->z = groundZ;
					return true;
				}
			}
		}
	}
	return false;
}

CTreadable*
CPathFind::FindRoadObjectClosestToCoors(CVector coors, uint8 type)
{
	int i, j, k;
	int node1, node2;
	CTreadable *closestMapObj = nil;
	float closestDist = 10000.0f;

	for(i = 0; i < m_numMapObjects; i++){
		CTreadable *mapObj = m_mapObjects[i];
		if(mapObj->m_nodeIndices[type][0] < 0)
			continue;
		CVector vDist = mapObj->GetPosition() - coors;
		float fDist = Abs(vDist.x) + Abs(vDist.y) + Abs(vDist.z);
		if(fDist < 200.0f || fDist < closestDist)
			for(j = 0; j < 12; j++){
				node1 = mapObj->m_nodeIndices[type][j];
				if(node1 < 0)
					break;
				// FIX: game uses ThePaths here explicitly
				for(k = 0; k < m_pathNodes[node1].numLinks; k++){
					node2 = ConnectedNode(m_pathNodes[node1].firstLink + k);
					float lineDist = CCollision::DistToLine(&m_pathNodes[node1].GetPosition(), &m_pathNodes[node2].GetPosition(), &coors);
					if(lineDist < closestDist){
						closestDist = lineDist;
						if((coors - m_pathNodes[node1].GetPosition()).MagnitudeSqr() < (coors - m_pathNodes[node2].GetPosition()).MagnitudeSqr())
							closestMapObj = m_mapObjects[m_pathNodes[node1].objectIndex];
						else
							closestMapObj = m_mapObjects[m_pathNodes[node2].objectIndex];
					}
				}
			}
	}
	return closestMapObj;
}

void
CPathFind::FindNextNodeWandering(uint8 type, CVector coors, CPathNode **lastNode, CPathNode **nextNode, uint8 curDir, uint8 *nextDir)
{
	int i;
	CPathNode *node;

	if(lastNode == nil || (node = *lastNode) == nil || (coors - (*lastNode)->GetPosition()).MagnitudeSqr() > 7.0f){
		// need to find the node we're coming from
		node = nil;
		CTreadable *obj = FindRoadObjectClosestToCoors(coors, type);
		float nodeDist = 1000000000.0f;
		for(i = 0; i < 12; i++){
			if(obj->m_nodeIndices[type][i] < 0)
				break;
			float dist = (coors - m_pathNodes[obj->m_nodeIndices[type][i]].GetPosition()).MagnitudeSqr();
			if(dist < nodeDist){
				nodeDist = dist;
				node = &m_pathNodes[obj->m_nodeIndices[type][i]];
			}
		}
	}

	CVector2D vCurDir(Sin(curDir*PI/4.0f), Cos(curDir * PI / 4.0f));
	*nextNode = 0;
	float bestDot = -999999.0f;
	for(i = 0; i < node->numLinks; i++){
		int next = ConnectedNode(node->firstLink+i);
		if(!node->bDisabled && m_pathNodes[next].bDisabled)
			continue;
		CVector pedCoors = coors;
		pedCoors.z += 1.0f;
		CVector nodeCoors = m_pathNodes[next].GetPosition();
		nodeCoors.z += 1.0f;
		if(!CWorld::GetIsLineOfSightClear(pedCoors, nodeCoors, true, false, false, false, false, false))
			continue;
		CVector2D nodeDir = m_pathNodes[next].GetPosition() - node->GetPosition();
		nodeDir.Normalise();
		float dot = DotProduct2D(nodeDir, vCurDir);
		if(dot >= bestDot){
			*nextNode = &m_pathNodes[next];
			bestDot = dot;

			// direction is 0, 2, 4, 6 for north, east, south, west
			// this could be done simpler...
			if(nodeDir.x < 0.0f){
				if(2.0f*Abs(nodeDir.y) < -nodeDir.x)
					*nextDir = 6;	// west
				else if(-2.0f*nodeDir.x < nodeDir.y)
					*nextDir = 0;	// north
				else if(2.0f*nodeDir.x > nodeDir.y)
					*nextDir = 4;	// south
				else if(nodeDir.y > 0.0f)
					*nextDir = 7;	// north west
				else
					*nextDir = 5;	// south west`
			}else{
				if(2.0f*Abs(nodeDir.y) < nodeDir.x)
					*nextDir = 2;	// east
				else if(2.0f*nodeDir.x < nodeDir.y)
					*nextDir = 0;	// north
				else if(-2.0f*nodeDir.x > nodeDir.y)
					*nextDir = 4;	// south
				else if(nodeDir.y > 0.0f)
					*nextDir = 1;	// north east
				else
					*nextDir = 3;	// south east`
			}
		}
	}
	if(*nextNode == nil){
		*nextDir = 0;
		*nextNode = node;
	}
}

static CPathNode *apNodesToBeCleared[4995];

void
CPathFind::DoPathSearch(uint8 type, CVector start, int32 startNodeId, CVector target, CPathNode **nodes, int16 *pNumNodes, int16 maxNumNodes, CVehicle *vehicle, float *pDist, float distLimit, int32 targetNodeId)
{
	int i, j;

	// Find target
	if(targetNodeId < 0)
		targetNodeId = FindNodeClosestToCoors(target, type, distLimit);
	if(targetNodeId < 0) {
		*pNumNodes = 0;
		if(pDist) *pDist = 100000.0f;
		return;
	}

	// Find start
	int numPathsToTry;
	CTreadable *startObj;
	if(startNodeId < 0){
		if(vehicle == nil || (startObj = vehicle->m_treadable[type]) == nil)
			startObj = FindRoadObjectClosestToCoors(start, type);
		numPathsToTry = 0;
		for(i = 0; i < 12; i++){
			if(startObj->m_nodeIndices[type][i] < 0)
				break;
			if(m_pathNodes[startObj->m_nodeIndices[type][i]].group == m_pathNodes[targetNodeId].group)
				numPathsToTry++;
		}
	}else{
		numPathsToTry = 1;
		startObj = m_mapObjects[m_pathNodes[startNodeId].objectIndex];
	}
	if(numPathsToTry == 0) {
		*pNumNodes = 0;
		if(pDist) *pDist = 100000.0f;
		return;
	}

	if(startNodeId < 0){
		// why only check node 0?
		if(m_pathNodes[startObj->m_nodeIndices[type][0]].group !=
		   m_pathNodes[targetNodeId].group) {
			*pNumNodes = 0;
			if(pDist) *pDist = 100000.0f;
			return;
		}
	}else{
		if(m_pathNodes[startNodeId].group != m_pathNodes[targetNodeId].group) {
			*pNumNodes = 0;
			if(pDist) *pDist = 100000.0f;
			return;
		}
	}

	for(i = 0; i < ARRAY_SIZE(m_searchNodes); i++)
		m_searchNodes[i].SetNext(nil);
	AddNodeToList(&m_pathNodes[targetNodeId], 0);
	int numNodesToBeCleared = 0;
	apNodesToBeCleared[numNodesToBeCleared++] = &m_pathNodes[targetNodeId];

	// Dijkstra's algorithm
	// Find distances
	int numPathsFound = 0;
	if(startNodeId < 0 && m_mapObjects[m_pathNodes[targetNodeId].objectIndex] == startObj)
		numPathsFound++;
	for(i = 0; numPathsFound < numPathsToTry; i = (i+1) & 0x1FF){
		CPathNode *node;
		for(node = m_searchNodes[i].GetNext(); node; node = node->GetNext()){
			if(m_mapObjects[node->objectIndex] == startObj &&
			   (startNodeId < 0 || node == &m_pathNodes[startNodeId]))
				numPathsFound++;

			for(j = 0; j < node->numLinks; j++){
				int next = ConnectedNode(node->firstLink + j);
				int dist = node->distance + m_distances[node->firstLink + j];
				if(dist < m_pathNodes[next].distance){
					if(m_pathNodes[next].distance != MAX_DIST)
						RemoveNodeFromList(&m_pathNodes[next]);
					if(m_pathNodes[next].distance == MAX_DIST)
						apNodesToBeCleared[numNodesToBeCleared++] = &m_pathNodes[next];
					AddNodeToList(&m_pathNodes[next], dist);
				}
			}

			RemoveNodeFromList(node);
		}
	}

	// Find out whence to start tracing back
	CPathNode *curNode;
	if(startNodeId < 0){
		int minDist = MAX_DIST;
		*pNumNodes = 1;
		for(i = 0; i < 12; i++){
			if(startObj->m_nodeIndices[type][i] < 0)
				break;
			int dist = (m_pathNodes[startObj->m_nodeIndices[type][i]].GetPosition() - start).Magnitude();
			if(m_pathNodes[startObj->m_nodeIndices[type][i]].distance + dist < minDist){
				minDist = m_pathNodes[startObj->m_nodeIndices[type][i]].distance + dist;
				curNode = &m_pathNodes[startObj->m_nodeIndices[type][i]];
			}
		}
		if(maxNumNodes == 0){
			*pNumNodes = 0;
		}else{
			nodes[0] = curNode;
			*pNumNodes = 1;
		}
		if(pDist)
			*pDist = minDist;
	}else
	{
		curNode = &m_pathNodes[startNodeId];
		*pNumNodes = 0;
		if(pDist)
			*pDist = m_pathNodes[startNodeId].distance;
	}

	// Trace back to target and update list of nodes
	while(*pNumNodes < maxNumNodes && curNode != &m_pathNodes[targetNodeId])
		for(i = 0; i < curNode->numLinks; i++){
			int next = ConnectedNode(curNode->firstLink + i);
			if(curNode->distance - m_distances[curNode->firstLink + i] == m_pathNodes[next].distance){
				curNode = &m_pathNodes[next];
				nodes[(*pNumNodes)++] = curNode;
				i = 29030;	// could have used a break...
			}
		}

	for(i = 0; i < numNodesToBeCleared; i++)
		apNodesToBeCleared[i]->distance = MAX_DIST;
	return;
}

static CPathNode *pNodeList[32];
static int16 DummyResult;
static int16 DummyResult2;

bool
CPathFind::TestCoorsCloseness(CVector target, uint8 type, CVector start)
{
	float dist;

	if(type == PATH_CAR)
		DoPathSearch(type, start, -1, target, pNodeList, &DummyResult, 32, nil, &dist, 999999.88f, -1);
	else
		DoPathSearch(type, start, -1, target, nil, &DummyResult2, 0, nil, &dist, 50.0f, -1);
#ifdef FIX_BUGS
	// dist has GenerationDistMultiplier as a factor, so our reference dist should have it too
	if(type == PATH_CAR)
		return dist < 160.0f*TheCamera.GenerationDistMultiplier;
	else
		return dist < 100.0f*TheCamera.GenerationDistMultiplier;
#else
	if(type == PATH_CAR)
		return dist < 160.0f;
	else
		return dist < 100.0f;
#endif
}

void
CPathFind::Save(uint8 *buf, uint32 *size)
{
	int i;
	int n = m_numPathNodes/8 + 1;

	*size = 2*n;

	for(i = 0; i < m_numPathNodes; i++)
		if(m_pathNodes[i].bDisabled)
			buf[i/8] |= 1 << i%8;
		else
			buf[i/8] &= ~(1 << i%8);

	for(i = 0; i < m_numPathNodes; i++)
		if(m_pathNodes[i].bBetweenLevels)
			buf[i/8 + n] |= 1 << i%8;
		else
			buf[i/8 + n] &= ~(1 << i%8);
}

void
CPathFind::Load(uint8 *buf, uint32 size)
{
	int i;
	int n = m_numPathNodes/8 + 1;

	for(i = 0; i < m_numPathNodes; i++)
		if(buf[i/8] & (1 << i%8))
			m_pathNodes[i].bDisabled = true;
		else
			m_pathNodes[i].bDisabled = false;

	for(i = 0; i < m_numPathNodes; i++)
		if(buf[i/8 + n] & (1 << i%8))
			m_pathNodes[i].bBetweenLevels = true;
		else
			m_pathNodes[i].bBetweenLevels = false;
}

void
CPathFind::DisplayPathData(void)
{
	// Not the function from mobile but my own!

	int i, j, k;
	// Draw 50 units around camera
	CVector pos = TheCamera.GetPosition();
	const float maxDist = 50.0f;

	// Render car path nodes
	if(gbShowCarPaths)
	for(i = 0; i < m_numCarPathNodes; i++){
		if((m_pathNodes[i].GetPosition() - pos).MagnitudeSqr() > SQR(maxDist))
			continue;

		CVector n1 = m_pathNodes[i].GetPosition();
		n1.z += 0.3f;

		// Draw node itself
		CLines::RenderLineWithClipping(n1.x, n1.y, n1.z,
			n1.x, n1.y, n1.z + 1.0f,
			0xFFFFFFFF, 0xFFFFFFFF);

		for(j = 0; j < m_pathNodes[i].numLinks; j++){
			k = ConnectedNode(m_pathNodes[i].firstLink + j);
			CVector n2 = m_pathNodes[k].GetPosition();
			n2.z += 0.3f;
			// Draw links to neighbours
			CLines::RenderLineWithClipping(n1.x, n1.y, n1.z,
				n2.x, n2.y, n2.z,
				0xFFFFFFFF, 0xFFFFFFFF);
		}
	}

	// Render car path nodes
	if(gbShowCarPathsLinks)
	for(i = 0; i < m_numCarPathLinks; i++){
		CVector2D n1_2d = m_carPathLinks[i].GetPosition();
		if((n1_2d - pos).MagnitudeSqr() > SQR(maxDist))
			continue;

		int ni = m_carPathLinks[i].pathNodeIndex;
		CVector pn1 = m_pathNodes[ni].GetPosition();
		pn1.z += 0.3f;
		CVector n1(n1_2d.x, n1_2d.y, pn1.z);
		n1.z += 0.3f;

		// Draw car node itself
		CLines::RenderLineWithClipping(n1.x, n1.y, n1.z,
			n1.x, n1.y, n1.z + 1.0f,
			0xFFFFFFFF, 0xFFFFFFFF);
		CLines::RenderLineWithClipping(n1.x, n1.y, n1.z + 0.5f,
			n1.x+m_carPathLinks[i].GetDirX(), n1.y+m_carPathLinks[i].GetDirY(), n1.z + 0.5f,
			0xFFFFFFFF, 0xFFFFFFFF);

		// Draw connection to car path node
		CLines::RenderLineWithClipping(n1.x, n1.y, n1.z,
			pn1.x, pn1.y, pn1.z,
			0xFF0000FF, 0xFFFFFFFF);

		// traffic light type
		uint32 col = 0xFF;
		if((m_carPathLinks[i].trafficLightType&0x7F) == 1)
			col += 0xFF000000;
		if((m_carPathLinks[i].trafficLightType&0x7F) == 2)
			col += 0x00FF0000;
		if(m_carPathLinks[i].trafficLightType & 0x80)
			col += 0x0000FF00;
		CLines::RenderLineWithClipping(n1.x+0.2f, n1.y, n1.z,
			n1.x+0.2f, n1.y, n1.z + 1.0f,
			col, col);

		for(j = 0; j < m_pathNodes[ni].numLinks; j++){
			k = m_carPathConnections[m_pathNodes[ni].firstLink + j];
			CVector2D n2_2d = m_carPathLinks[k].GetPosition();
			int nk = m_carPathLinks[k].pathNodeIndex;
			CVector pn2 = m_pathNodes[nk].GetPosition();
			pn2.z += 0.3f;
			CVector n2(n2_2d.x, n2_2d.y, pn2.z);
			n2.z += 0.3f;

			// Draw links to neighbours
			CLines::RenderLineWithClipping(n1.x, n1.y, n1.z,
				n2.x, n2.y, n2.z,
				0xFF00FFFF, 0xFF00FFFF);
		}
	}

	// Render ped path nodes
	if(gbShowPedPaths)
	for(i = m_numCarPathNodes; i < m_numPathNodes; i++){
		if((m_pathNodes[i].GetPosition() - pos).MagnitudeSqr() > SQR(maxDist))
			continue;

		CVector n1 = m_pathNodes[i].GetPosition();
		n1.z += 0.3f;

		// Draw node itself
		CLines::RenderLineWithClipping(n1.x, n1.y, n1.z,
			n1.x, n1.y, n1.z + 1.0f,
			0xFFFFFFFF, 0xFFFFFFFF);

		for(j = 0; j < m_pathNodes[i].numLinks; j++){
			k = ConnectedNode(m_pathNodes[i].firstLink + j);
			CVector n2 = m_pathNodes[k].GetPosition();
			n2.z += 0.3f;
			// Draw links to neighbours
			CLines::RenderLineWithClipping(n1.x, n1.y, n1.z,
				n2.x, n2.y, n2.z,
				0xFFFFFFFF, 0xFFFFFFFF);

			// Draw connection flags
			CVector mid = (n1+n2)/2.0f;
			uint32 col = 0xFF;
			if(ConnectionCrossesRoad(m_pathNodes[i].firstLink + j))
				col += 0x00FF0000;
			if(ConnectionHasTrafficLight(m_pathNodes[i].firstLink + j))
				col += 0xFF000000;
			CLines::RenderLineWithClipping(mid.x, mid.y, mid.z,
				mid.x, mid.y, mid.z + 1.0f,
				col, col);
		}
	}
}
