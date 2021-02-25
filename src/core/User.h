#pragma once

#include "Pager.h"
#include "OnscreenTimer.h"

class CZone;
class CVehicle;

class CPlaceName
{
	CZone *m_pZone;
	CZone *m_pZone2;
	int16 m_nAdditionalTimer;
public:
	CPlaceName();
	void Init();
	void Process();
	void Display();
};

class CCurrentVehicle
{
	CVehicle *m_pCurrentVehicle;
public:
	CCurrentVehicle();
	void Init();
	void Process();
	void Display();
};

class CUserDisplay
{
public:
	static CPlaceName PlaceName;
	static COnscreenTimer OnscnTimer;
	static CPager Pager;
	static CCurrentVehicle CurrentVehicle;

	static void Init();
	static void Process();
};
