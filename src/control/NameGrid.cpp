#include "common.h"
#include "NameGrid.h"

// TODO: reverse mobile code

CPlayerName::CPlayerName()
{
	// TODO
}

void
CPlayerName::DisplayName(int)
{
	// TODO
}

CRow::CRow()
{
	// TODO
}

void
CRow::SetLetter(int, wchar *)
{
	// TODO
}

CGrid::CGrid()
{
	// TODO
}

void
CGrid::ProcessAnyLeftJustDown()
{
	unk_int2--;
}

void
CGrid::ProcessAnyRightJustDown()
{
	unk_int2++;
}

void
CGrid::ProcessAnyUpJustDown()
{
	unk_int1--;
}

void
CGrid::ProcessAnyDownJustDown()
{
	unk_int1++;
}

void
CGrid::AllDoneMakePlayerName()
{
	// TODO
}

void
CGrid::ProcessDPadCrossJustDown()
{
	// TODO
}

void
CGrid::DisplayGrid()
{
	// TODO
}

void
CGrid::ProcessControllerInput()
{
	// TODO
}

void
CGrid::Process()
{
	ProcessControllerInput();
	DisplayGrid();
	playerName.DisplayName(2 * playerName.unk_4c);
}