#pragma once

#include "templates.h"
#include "Lists.h"
#include "Treadable.h"
#include "Object.h"
#include "CutsceneHead.h"
#include "PlayerPed.h"
#include "Automobile.h"
#include "DummyPed.h"
#include "AudioScriptObject.h"

typedef CPool<CPtrNode> CCPtrNodePool;
typedef CPool<CEntryInfoNode> CEntryInfoNodePool;
typedef CPool<CPed,CPlayerPed> CPedPool;
typedef CPool<CVehicle,CAutomobile> CVehiclePool;
typedef CPool<CBuilding> CBuildingPool;
typedef CPool<CTreadable> CTreadablePool;
typedef CPool<CObject, CCutsceneHead> CObjectPool;
typedef CPool<CDummy, CDummyPed> CDummyPool;
typedef CPool<cAudioScriptObject> CAudioScriptObjectPool;

class CPools
{
	static CCPtrNodePool *ms_pPtrNodePool;
	static CEntryInfoNodePool *ms_pEntryInfoNodePool;
	static CPedPool *ms_pPedPool;
	static CVehiclePool *ms_pVehiclePool;
	static CBuildingPool *ms_pBuildingPool;
	static CTreadablePool *ms_pTreadablePool;
	static CObjectPool *ms_pObjectPool;
	static CDummyPool *ms_pDummyPool;
	static CAudioScriptObjectPool *ms_pAudioScriptObjectPool;
public:
	static CCPtrNodePool *GetPtrNodePool(void) { return ms_pPtrNodePool; }
	static CEntryInfoNodePool *GetEntryInfoNodePool(void) { return ms_pEntryInfoNodePool; }
	static CPedPool *GetPedPool(void) { return ms_pPedPool; }
	static CVehiclePool *GetVehiclePool(void) { return ms_pVehiclePool; }
	static CBuildingPool *GetBuildingPool(void) { return ms_pBuildingPool; }
	static CTreadablePool *GetTreadablePool(void) { return ms_pTreadablePool; }
	static CObjectPool *GetObjectPool(void) { return ms_pObjectPool; }
	static CDummyPool *GetDummyPool(void) { return ms_pDummyPool; }
	static CAudioScriptObjectPool *GetAudioScriptObjectPool(void) { return ms_pAudioScriptObjectPool; }

	static void Initialise(void);
	static void ShutDown(void);
	static int32 GetPedRef(CPed *ped);
	static CPed *GetPed(int32 handle);
	static int32 GetVehicleRef(CVehicle *vehicle);
	static CVehicle *GetVehicle(int32 handle);
	static int32 GetObjectRef(CObject *object);
	static CObject *GetObject(int32 handle);
	static void CheckPoolsEmpty();
	static void MakeSureSlotInObjectPoolIsEmpty(int32 slot);
	static void LoadObjectPool(uint8 *buf, uint32 size);
	static void LoadPedPool(uint8 *buf, uint32 size);
	static void LoadVehiclePool(uint8 *buf, uint32 size);
	static void SaveObjectPool(uint8 *buf, uint32 *size);
	static void SavePedPool(uint8 *buf, uint32 *size);
	static void SaveVehiclePool(uint8 *buf, uint32 *size);
};
