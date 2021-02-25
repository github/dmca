#include "common.h"

#include "Pager.h"
#include "Timer.h"
#include "Messages.h"
#include "Hud.h"
#include "Camera.h"

void
CPager::Init()
{
	ClearMessages();
	m_nNumDisplayLetters = 8;
}

void
CPager::Process()
{
	if (m_messages[0].m_pText != nil && m_messages[0].m_nCurrentPosition >= (int32)m_messages[0].m_nStringLength) {
		m_messages[0].m_pText = nil;
		uint16 i = 0;
		while (i < NUMPAGERMESSAGES-1) {
			if (m_messages[i + 1].m_pText == nil) break;
			m_messages[i] = m_messages[i + 1];
			i++;
		}
		m_messages[i].m_pText = nil;
		if (m_messages[0].m_pText != nil)
			CMessages::AddToPreviousBriefArray(
				m_messages[0].m_pText,
				m_messages[0].m_nNumber[0],
				m_messages[0].m_nNumber[1],
				m_messages[0].m_nNumber[2],
				m_messages[0].m_nNumber[3],
				m_messages[0].m_nNumber[4],
				m_messages[0].m_nNumber[5],
				0);
	}
	Display();
	if (m_messages[0].m_pText != nil) {
		if (TheCamera.m_WideScreenOn || !CHud::m_Wants_To_Draw_Hud || CHud::m_BigMessage[0][0] || CHud::m_BigMessage[2][0]) {
			RestartCurrentMessage();
		} else {
			if (CTimer::GetTimeInMilliseconds() > m_messages[0].m_nTimeToChangePosition) {
				m_messages[0].m_nCurrentPosition++;
				m_messages[0].m_nTimeToChangePosition = CTimer::GetTimeInMilliseconds() + m_messages[0].m_nSpeedMs;
			}
		}
	}
}

void
CPager::Display()
{
	wchar outstr1[256];
	wchar outstr2[260];

	wchar *pText = m_messages[0].m_pText;
	uint16 i = 0;
	if (pText != nil) {
		CMessages::InsertNumberInString(
			pText,
			m_messages[0].m_nNumber[0],
			m_messages[0].m_nNumber[1],
			m_messages[0].m_nNumber[2],
			m_messages[0].m_nNumber[3],
			m_messages[0].m_nNumber[4],
			m_messages[0].m_nNumber[5],
			outstr1);
		for (; i < m_nNumDisplayLetters; i++) {
			int pos = m_messages[0].m_nCurrentPosition + i;
			if (pos >= 0) {
				if (!outstr1[pos]) break;

				outstr2[i] = outstr1[pos];
			} else {
				outstr2[i] = ' ';
			}
		}
	}
	outstr2[i] = '\0';
	CHud::SetPagerMessage(outstr2);
}

void
CPager::AddMessage(wchar *str, uint16 speed, uint16 priority, uint16 a5)
{
	uint16 size = CMessages::GetWideStringLength(str);
	for (int32 i = 0; i < NUMPAGERMESSAGES; i++) {
		if (m_messages[i].m_pText) {
			if (m_messages[i].m_nPriority >= priority)
				continue;

			for (int j = NUMPAGERMESSAGES-1; j > i; j--)
				m_messages[j] = m_messages[j-1];

		}
		m_messages[i].m_pText = str;
		m_messages[i].m_nSpeedMs = speed;
		m_messages[i].m_nPriority = priority;
		m_messages[i].unused = a5;
		m_messages[i].m_nCurrentPosition = -(m_nNumDisplayLetters + 10);
		m_messages[i].m_nTimeToChangePosition = CTimer::GetTimeInMilliseconds() + speed;
		m_messages[i].m_nStringLength = size;
		m_messages[i].m_nNumber[0] = -1;
		m_messages[i].m_nNumber[1] = -1;
		m_messages[i].m_nNumber[2] = -1;
		m_messages[i].m_nNumber[3] = -1;
		m_messages[i].m_nNumber[4] = -1;
		m_messages[i].m_nNumber[5] = -1;

		if (i == 0)
			CMessages::AddToPreviousBriefArray(
				m_messages[0].m_pText,
				m_messages[0].m_nNumber[0],
				m_messages[0].m_nNumber[1],
				m_messages[0].m_nNumber[2],
				m_messages[0].m_nNumber[3],
				m_messages[0].m_nNumber[4],
				m_messages[0].m_nNumber[5],
				nil);
			return;
	}
}

void
CPager::AddMessageWithNumber(wchar *str, int32 n1, int32 n2, int32 n3, int32 n4, int32 n5, int32 n6, uint16 speed, uint16 priority, uint16 a11)
{
	wchar nstr[520];

	CMessages::InsertNumberInString(str, n1, n2, n3, n4, n5, n6, nstr);
	uint16 size = CMessages::GetWideStringLength(nstr);
	for (int32 i = 0; i < NUMPAGERMESSAGES; i++) {
		if (m_messages[i].m_pText) {
			if (m_messages[i].m_nPriority >= priority)
				continue;

			for (int j = NUMPAGERMESSAGES-1; j > i; j--)
				m_messages[j] = m_messages[j - 1];

		}
		m_messages[i].m_pText = str;
		m_messages[i].m_nSpeedMs = speed;
		m_messages[i].m_nPriority = priority;
		m_messages[i].unused = a11;
		m_messages[i].m_nCurrentPosition = -(m_nNumDisplayLetters + 10);
		m_messages[i].m_nTimeToChangePosition = CTimer::GetTimeInMilliseconds() + speed;
		m_messages[i].m_nStringLength = size;
		m_messages[i].m_nNumber[0] = n1;
		m_messages[i].m_nNumber[1] = n2;
		m_messages[i].m_nNumber[2] = n3;
		m_messages[i].m_nNumber[3] = n4;
		m_messages[i].m_nNumber[4] = n5;
		m_messages[i].m_nNumber[5] = n6;

		if (i == 0)
			CMessages::AddToPreviousBriefArray(
				m_messages[0].m_pText,
				m_messages[0].m_nNumber[0],
				m_messages[0].m_nNumber[1],
				m_messages[0].m_nNumber[2],
				m_messages[0].m_nNumber[3],
				m_messages[0].m_nNumber[4],
				m_messages[0].m_nNumber[5],
				nil);
		return;
	}
}

void
CPager::ClearMessages()
{
	for (int32 i = 0; i < NUMPAGERMESSAGES; i++)
		m_messages[i].m_pText = nil;
}

void
CPager::RestartCurrentMessage()
{
	if (m_messages[0].m_pText != nil) {
		m_messages[0].m_nCurrentPosition = -(m_nNumDisplayLetters + 10);
		m_messages[0].m_nTimeToChangePosition = CTimer::GetTimeInMilliseconds() + m_messages[0].m_nSpeedMs;
	}
}