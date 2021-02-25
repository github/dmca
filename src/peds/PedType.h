#pragma once

// Index into the PedType array
enum ePedType
{
	PEDTYPE_PLAYER1,
	PEDTYPE_PLAYER2,
	PEDTYPE_PLAYER3,
	PEDTYPE_PLAYER4,
	PEDTYPE_CIVMALE,
	PEDTYPE_CIVFEMALE,
	PEDTYPE_COP,
	PEDTYPE_GANG1,
	PEDTYPE_GANG2,
	PEDTYPE_GANG3,
	PEDTYPE_GANG4,
	PEDTYPE_GANG5,
	PEDTYPE_GANG6,
	PEDTYPE_GANG7,
	PEDTYPE_GANG8,
	PEDTYPE_GANG9,
	PEDTYPE_EMERGENCY,
	PEDTYPE_FIREMAN,
	PEDTYPE_CRIMINAL,
	PEDTYPE_UNUSED1,
	PEDTYPE_PROSTITUTE,
	PEDTYPE_SPECIAL,
	PEDTYPE_UNUSED2,

	NUM_PEDTYPES
};

enum
{
	PED_FLAG_PLAYER1 = 1 << 0,
	PED_FLAG_PLAYER2 = 1 << 1,
	PED_FLAG_PLAYER3 = 1 << 2,
	PED_FLAG_PLAYER4 = 1 << 3,
	PED_FLAG_CIVMALE = 1 << 4,
	PED_FLAG_CIVFEMALE = 1 << 5,
	PED_FLAG_COP = 1 << 6,
	PED_FLAG_GANG1 = 1 << 7,
	PED_FLAG_GANG2 = 1 << 8,
	PED_FLAG_GANG3 = 1 << 9,
	PED_FLAG_GANG4 = 1 << 10,
	PED_FLAG_GANG5 = 1 << 11,
	PED_FLAG_GANG6 = 1 << 12,
	PED_FLAG_GANG7 = 1 << 13,
	PED_FLAG_GANG8 = 1 << 14,
	PED_FLAG_GANG9 = 1 << 15,
	PED_FLAG_EMERGENCY = 1 << 16,
	PED_FLAG_PROSTITUTE = 1 << 17,
	PED_FLAG_CRIMINAL = 1 << 18,
	PED_FLAG_SPECIAL = 1 << 19,
	PED_FLAG_GUN = 1 << 20,
	PED_FLAG_COP_CAR = 1 << 21,
	PED_FLAG_FAST_CAR = 1 << 22,
	PED_FLAG_EXPLOSION = 1 << 23,
	PED_FLAG_FIREMAN = 1 << 24,
	PED_FLAG_DEADPEDS = 1 << 25,
};

class CPedType
{
	uint32 m_flag;
	float unknown1;
	float unknown2;
	float unknown3;
	float unknown4;
	float unknown5;
	uint32 m_threats;
	uint32 m_avoid;

	static CPedType *ms_apPedType[NUM_PEDTYPES];
public:

	static void Initialise(void);
	static void Shutdown(void);
	static void LoadPedData(void);
	static ePedType FindPedType(char *type);
	static uint32 FindPedFlag(char *type);
	static void Save(uint8 *buf, uint32 *size);
	static void Load(uint8 *buf, uint32 size);

	static uint32 GetFlag(int type) { return ms_apPedType[type]->m_flag; }
	static uint32 GetAvoid(int type) { return ms_apPedType[type]->m_avoid; }
	static uint32 GetThreats(int type) { return ms_apPedType[type]->m_threats; }
	static void SetThreats(int type, uint32 threat) { ms_apPedType[type]->m_threats = threat; }
	static void AddThreat(int type, int threat) { ms_apPedType[type]->m_threats |= threat; }
	static void RemoveThreat(int type, int threat) { ms_apPedType[type]->m_threats &= ~threat; }
	static bool IsThreat(int type, int threat) { return ms_apPedType[type]->m_threats & threat; }
};

VALIDATE_SIZE(CPedType, 0x20);

enum ePedStats
{
	PEDSTAT_PLAYER,
	PEDSTAT_COP,
	PEDSTAT_MEDIC,
	PEDSTAT_FIREMAN,
	PEDSTAT_GANG1,
	PEDSTAT_GANG2,
	PEDSTAT_GANG3,
	PEDSTAT_GANG4,
	PEDSTAT_GANG5,
	PEDSTAT_GANG6,
	PEDSTAT_GANG7,
	PEDSTAT_STREET_GUY,
	PEDSTAT_SUIT_GUY,
	PEDSTAT_SENSIBLE_GUY,
	PEDSTAT_GEEK_GUY,
	PEDSTAT_OLD_GUY,
	PEDSTAT_TOUGH_GUY,
	PEDSTAT_STREET_GIRL,
	PEDSTAT_SUIT_GIRL,
	PEDSTAT_SENSIBLE_GIRL,
	PEDSTAT_GEEK_GIRL,
	PEDSTAT_OLD_GIRL,
	PEDSTAT_TOUGH_GIRL,
	PEDSTAT_TRAMP_MALE,
	PEDSTAT_TRAMP_FEMALE,
	PEDSTAT_TOURIST,
	PEDSTAT_PROSTITUTE,
	PEDSTAT_CRIMINAL,
	PEDSTAT_BUSKER,
	PEDSTAT_TAXIDRIVER,
	PEDSTAT_PSYCHO,
	PEDSTAT_STEWARD,
	PEDSTAT_SPORTSFAN,
	PEDSTAT_SHOPPER,
	PEDSTAT_OLDSHOPPER,

	NUM_PEDSTATS
};

// flags
enum
{
	STAT_PUNCH_ONLY = 1,
	STAT_CAN_KNEE_HEAD = 2,
	STAT_CAN_KICK = 4,
	STAT_CAN_ROUNDHOUSE = 8,
	STAT_NO_DIVE = 0x10,
	STAT_ONE_HIT_KNOCKDOWN = 0x20,
	STAT_SHOPPING_BAGS = 0x40,
	STAT_GUN_PANIC = 0x80
};

class CPedStats
{
public:
	ePedStats m_type;
	char m_name[24];
	float m_fleeDistance;
	float m_headingChangeRate;
	int8 m_fear;
	int8 m_temper;
	int8 m_lawfulness;
	int8 m_sexiness;
	float m_attackStrength;
	float m_defendWeakness;
	int16 m_flags;

	static CPedStats *ms_apPedStats[NUM_PEDSTATS];

	static void Initialise(void);
	static void Shutdown(void);
	static void LoadPedStats(void);
	static ePedStats GetPedStatType(char *name);
};

VALIDATE_SIZE(CPedStats, 0x34);