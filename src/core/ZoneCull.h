class CEntity;

class CCullZone
{
public:
	CVector position;
	float minx;
	float maxx;
	float miny;
	float maxy;
	float minz;
	float maxz;

	int32 m_indexStart;
	int16 m_groupIndexCount[3];	// only useful during resolution stage
	int16 m_numBuildings;
	int16 m_numTreadablesPlus10m;
	int16 m_numTreadables;

	void DoStuffLeavingZone(void);
	static void DoStuffLeavingZone_OneBuilding(uint16 i);
	static void DoStuffLeavingZone_OneTreadableBoth(uint16 i);
	void DoStuffEnteringZone(void);
	static void DoStuffEnteringZone_OneBuilding(uint16 i);
	static void DoStuffEnteringZone_OneTreadablePlus10m(uint16 i);
	static void DoStuffEnteringZone_OneTreadable(uint16 i);


	static bool TestLine(CVector vec1, CVector vec2);
	static bool DoThoroughLineTest(CVector vec1, CVector vec2, CEntity *testEntity);
	float CalcDistToCullZoneSquared(float x, float y);
	float CalcDistToCullZone(float x, float y) { return Sqrt(CalcDistToCullZoneSquared(x, y)); };
	bool IsEntityCloseEnoughToZone(CEntity* entity, bool checkLevel);
	bool PointFallsWithinZone(CVector pos, float radius);
	bool TestEntityVisibilityFromCullZone(CEntity *entity, float extraDist, CEntity *LODentity);
	void FindTestPoints();

	void GetGroupStartAndSize(int32 groupid, int32 &start, int32 &size) {
		switch (groupid) {
		case 0:
		default:
			// buildings
			start = m_indexStart;
			size = m_groupIndexCount[0];
			break;
		case 1:
			// treadables + 10m
			start = m_groupIndexCount[0] + m_indexStart;
			size = m_groupIndexCount[1];
			break;
		case 2:
			// treadables
			start = m_groupIndexCount[0] + m_groupIndexCount[1] + m_indexStart;
			size = m_groupIndexCount[2];
			break;
		}
	}
};

enum eZoneAttribs
{
	ATTRZONE_CAMCLOSEIN		= 1,
	ATTRZONE_STAIRS			= 2,
	ATTRZONE_1STPERSON		= 4,
	ATTRZONE_NORAIN			= 8,
	ATTRZONE_NOPOLICE		= 0x10,
	ATTRZONE_NOTCULLZONE		= 0x20,
	ATTRZONE_DOINEEDCOLLISION	= 0x40,
	ATTRZONE_SUBWAYVISIBLE		= 0x80,
};

struct CAttributeZone
{
	float minx;
	float maxx;
	float miny;
	float maxy;
	float minz;
	float maxz;
	int16 attributes;
	int16 wantedLevel;
};

class CCullZones
{
public:
	static int32     NumCullZones;
	static CCullZone aZones[NUMCULLZONES];
	static int32     NumAttributeZones;
	static CAttributeZone aAttributeZones[NUMATTRIBZONES];
	static uint16        aIndices[NUMZONEINDICES];
	static int16         aPointersToBigBuildingsForBuildings[NUMBUILDINGS];
	static int16         aPointersToBigBuildingsForTreadables[NUMTREADABLES];

	static int32 CurrentWantedLevelDrop_Player;
	static int32 CurrentFlags_Camera;
	static int32 CurrentFlags_Player;
	static int32 OldCullZone;
	static int32 EntityIndicesUsed;
	static bool bCurrentSubwayIsInvisible;
	static bool bCullZonesDisabled;

	static void Init(void);
	static void ResolveVisibilities(void);
	static void Update(void);
	static void ForceCullZoneCoors(CVector coors);
	static int32 FindCullZoneForCoors(CVector coors);
	static int32 FindAttributesForCoors(CVector coors, int32 *wantedLevel);
	static CAttributeZone *FindZoneWithStairsAttributeForPlayer(void);
	static void MarkSubwayAsInvisible(bool visible);
	static void AddCullZone(CVector const &position,
		float minx, float maxx,
		float miny, float maxy,
		float minz, float maxz,
		uint16 flag, int16 wantedLevel);
	static bool CamCloseInForPlayer(void) { return (CurrentFlags_Player & ATTRZONE_CAMCLOSEIN) != 0; }
	static bool CamStairsForPlayer(void) { return (CurrentFlags_Player & ATTRZONE_STAIRS) != 0; }
	static bool Cam1stPersonForPlayer(void) { return (CurrentFlags_Player & ATTRZONE_1STPERSON) != 0; }
	static bool NoPolice(void) { return (CurrentFlags_Player & ATTRZONE_NOPOLICE) != 0; }
	static bool DoINeedToLoadCollision(void) { return (CurrentFlags_Player & ATTRZONE_DOINEEDCOLLISION) != 0; }
	static bool PlayerNoRain(void) { return (CurrentFlags_Player & ATTRZONE_NORAIN) != 0; }
	static bool CamNoRain(void) { return (CurrentFlags_Camera & ATTRZONE_NORAIN) != 0; }
	static int32 GetWantedLevelDrop(void) { return CurrentWantedLevelDrop_Player; }

	static void BuildListForBigBuildings();
	static void DoVisibilityTestCullZone(int zoneId, bool doIt);
	static bool DoWeHaveMoreThanXOccurencesOfSet(int32 count, uint16 *set);

	static void CompressIndicesArray();
	static bool PickRandomSetForGroup(int32 zone, int32 group, uint16 *set);
	static void ReplaceSetForAllGroups(uint16 *set, uint16 setid);
	static void TidyUpAndMergeLists(uint16 *extraIndices, int32 numExtraIndices);

	// debug
	static bool LoadTempFile(void);
	static void SaveTempFile(void);
};
