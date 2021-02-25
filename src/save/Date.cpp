#include "common.h"
#include "Date.h"

CDate::CDate()
{
	m_nYear = 0;
	m_nSecond = 0;
	m_nMinute = 0;
	m_nHour = 0;
	m_nDay = 0;
	m_nMonth = 0;
}

bool
CDate::operator>(const CDate &right)
{
	if (m_nYear > right.m_nYear)
		return true;
	if (m_nYear != right.m_nYear)
		return false;

	if (m_nMonth > right.m_nMonth)
		return true;
	if (m_nMonth != right.m_nMonth)
		return false;

	if (m_nDay > right.m_nDay)
		return true;
	if (m_nDay != right.m_nDay)
		return false;

	if (m_nHour > right.m_nHour)
		return true;
	if (m_nHour != right.m_nHour)
		return false;

	if (m_nMinute > right.m_nMinute)
		return true;
	if (m_nMinute != right.m_nMinute)
		return false;
	return m_nSecond > right.m_nSecond;
}

bool
CDate::operator<(const CDate &right)
{
	if (m_nYear < right.m_nYear)
		return true;
	if (m_nYear != right.m_nYear)
		return false;

	if (m_nMonth < right.m_nMonth)
		return true;
	if (m_nMonth != right.m_nMonth)
		return false;

	if (m_nDay < right.m_nDay)
		return true;
	if (m_nDay != right.m_nDay)
		return false;

	if (m_nHour < right.m_nHour)
		return true;
	if (m_nHour != right.m_nHour)
		return false;

	if (m_nMinute < right.m_nMinute)
		return true;
	if (m_nMinute != right.m_nMinute)
		return false;
	return m_nSecond < right.m_nSecond;
}

bool
CDate::operator==(const CDate &right)
{
	if (m_nYear != right.m_nYear || m_nMonth != right.m_nMonth || m_nDay != right.m_nDay || m_nHour != right.m_nHour || m_nMinute != right.m_nMinute)
		return false;
	return m_nSecond == right.m_nSecond;
}

void
CDate::PopulateDateFields(int8 &second, int8 &minute, int8 &hour, int8 &day, int8 &month, int16 year)
{
	m_nSecond = second;
	m_nMinute = minute;
	m_nHour = hour;
	m_nDay = day;
	m_nMonth = month;
	m_nYear = year;
}