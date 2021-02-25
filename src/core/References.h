#pragma once

class CEntity;

struct CReference
{
	CReference *next;
	CEntity **pentity;
};

class CReferences
{
public:
	static CReference aRefs[NUMREFERENCES];
	static CReference *pEmptyList;

	static void Init(void);
	static void RemoveReferencesToPlayer(void);
	static void PruneAllReferencesInWorld(void);
};
