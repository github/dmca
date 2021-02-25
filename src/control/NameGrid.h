#pragma once

// TODO: reverse mobile code

class CPlayerName
{
	friend class CGrid;

	float x;
	float y;
	wchar unk_8[34];
	int unk_4c;
public:
	CPlayerName();
	void DisplayName(int);
};

class CRow
{
	friend class CGrid;

	int unk_0;
	int unk_4;
	wchar unk_8[20];
	int unk_30;
public:
	CRow();
	void SetLetter(int, wchar *);
};

class CGrid
{
	CRow rows[5];
	int unk_int1;
	int unk_int2;
	int unk_int3;
	float unk_float1;
	float unk_float2;
	CPlayerName playerName;
	char unk2[4];
	char unk3[4];
public:
	CGrid();
	void ProcessAnyLeftJustDown();
	void ProcessAnyRightJustDown();
	void ProcessAnyUpJustDown();
	void ProcessAnyDownJustDown();
	void AllDoneMakePlayerName();
	void ProcessDPadCrossJustDown();
	void DisplayGrid();
	void ProcessControllerInput();
	void Process();
};