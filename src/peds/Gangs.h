#pragma once

struct CGangInfo
{
	int32 m_nVehicleMI;
	int8 m_nPedModelOverride;
	int32 m_Weapon1;
	int32 m_Weapon2;

	CGangInfo();
};

VALIDATE_SIZE(CGangInfo, 0x10);

enum {
	GANG_MAFIA = 0,
	GANG_TRIAD,
	GANG_DIABLOS,
	GANG_YAKUZA,
	GANG_YARDIE,
	GANG_COLUMB,
	GANG_HOODS,
	GANG_7,
	GANG_8,
	NUM_GANGS
};

class CGangs
{
public:
	static void Initialise(void);
	static void SetGangVehicleModel(int16, int32);
	static void SetGangWeapons(int16, int32, int32);
	static void SetGangPedModelOverride(int16, int8);
	static int8 GetGangPedModelOverride(int16);
	static void SaveAllGangData(uint8 *, uint32 *);
	static void LoadAllGangData(uint8 *, uint32);

	static int32 GetGangVehicleModel(int16 gang) { return Gang[gang].m_nVehicleMI; }
	static CGangInfo *GetGangInfo(int16 gang) { return &Gang[gang]; }

private:
	static CGangInfo Gang[NUM_GANGS];
};
