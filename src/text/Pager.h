#pragma once

struct PagerMessage {
	wchar *m_pText;
	uint16 m_nSpeedMs;
	int16 m_nCurrentPosition;
	uint16 m_nStringLength;
	uint16 m_nPriority;
	uint32 m_nTimeToChangePosition;
	int16 unused; // but still set in SCM. importance? ringtone?
	int32 m_nNumber[6];
};

#define NUMPAGERMESSAGES 8

class CPager
{
	int16 m_nNumDisplayLetters;
	PagerMessage m_messages[NUMPAGERMESSAGES];
public:
	void Init();
	void Process();
	void Display();
	void AddMessage(wchar*, uint16, uint16, uint16);
	void AddMessageWithNumber(wchar *str, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, uint16 speed, uint16 priority, uint16 a11);
	void ClearMessages();
	void RestartCurrentMessage();
};