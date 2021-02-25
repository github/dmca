#pragma once

enum
{
	POWERPOINT_NONE = 0,
	POWERPOINT_HEALTH,
	POWERPOINT_HIDEOUT_INDUSTRIAL,
	POWERPOINT_HIDEOUT_COMMERCIAL,
	POWERPOINT_HIDEOUT_SUBURBAN
};

class CPowerPoint
{
public:
	void Update();
};

class CPowerPoints
{
public:
	static void Init();
	static void Update();
	static void GenerateNewOne(float, float, float, float, float, float, uint8);
	static void Save(uint8**, uint32*);
	static void Load(uint8*, uint32);
};