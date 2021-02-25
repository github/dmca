#pragma once

struct tMessage 
{
	wchar *m_pText;
	uint16 m_nFlag;
	uint32 m_nTime;
	uint32 m_nStartTime;
	int32 m_nNumber[6];
	wchar *m_pString;
};

struct tBigMessage 
{
	tMessage m_Stack[4];
};

struct tPreviousBrief 
{
	wchar *m_pText;
	int32 m_nNumber[6];
	wchar *m_pString;
};

#define NUMBRIEFMESSAGES 8
#define NUMBIGMESSAGES 6
#define NUMPREVIOUSBRIEFS 5

class CMessages 
{
public:
	static tMessage BriefMessages[NUMBRIEFMESSAGES];
	static tBigMessage BIGMessages[NUMBIGMESSAGES];
	static tPreviousBrief PreviousBriefs[NUMPREVIOUSBRIEFS];
	static char PreviousMissionTitle[16]; // unused
public:
	static void Init(void);
	static uint16 GetWideStringLength(wchar *src);
	static void WideStringCopy(wchar *dst, wchar *src, uint16 size);
	static bool WideStringCompare(wchar *str1, wchar *str2, uint16 size);
	static void Process(void);
	static void Display(void);
	static void AddMessage(wchar *key, uint32 time, uint16 pos);
	static void AddMessageJumpQ(wchar *key, uint32 time, uint16 pos);
	static void AddMessageSoon(wchar *key, uint32 time, uint16 pos);
	static void ClearMessages(void);
	static void ClearSmallMessagesOnly(void);
	static void AddBigMessage(wchar *key, uint32 time, uint16 pos);
	static void AddBigMessageQ(wchar *key, uint32 time, uint16 pos);
	static void AddToPreviousBriefArray(wchar *text, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, wchar *string);
	static void InsertNumberInString(wchar *src, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, wchar *dst);
	static void InsertStringInString(wchar *str1, wchar *str2);
	static void InsertPlayerControlKeysInString(wchar *src);
	static void AddMessageWithNumber(wchar *key, uint32 time, uint16 pos, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6);
	static void AddMessageJumpQWithNumber(wchar *key, uint32 time, uint16 pos, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6);
	static void AddMessageSoonWithNumber(wchar *key, uint32 time, uint16 pos, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6);
	static void AddBigMessageWithNumber(wchar *key, uint32 time, uint16 pos, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6);
	static void AddBigMessageWithNumberQ(wchar *key, uint32 time, uint16 pos, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6);
	static void AddMessageWithString(wchar *text, uint32 time, uint16 flag, wchar *str);
	static void AddMessageJumpQWithString(wchar *text, uint32 time, uint16 flag, wchar *str);
	static void ClearThisPrint(wchar *str);
	static void ClearThisBigPrint(wchar *str);
	static void ClearAllMessagesDisplayedByGame(void);

	// unused or cut
	//static void AddMessageSoonWithString(wchar*, uint32, uint16, wchar*);
	//static void CutString(int16, char*, char**);
	//static void PrintString(char*, int16, int16, int16);
};
