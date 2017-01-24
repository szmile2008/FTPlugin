#include "stdafx.h"
#include "FormatTime.h"

std::string FormatTime::FormatTimestamp(int nTimestamp, int nTimezone, FormatTimeType eFmtType)
{
	time_t nTime = nTimestamp + 3600 * nTimezone;
	struct tm *stTime = gmtime(&nTime);
	return FormatTimeStruct(stTime->tm_year + 1900, stTime->tm_mon + 1, stTime->tm_mday, stTime->tm_hour, stTime->tm_min, stTime->tm_sec, eFmtType);
}

std::string FormatTime::FormatTimeStruct(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, FormatTimeType eFmtType)
{
	char szFmtBuf[64] = {};	
	switch (eFmtType)
	{
	case FormatTime_YMDHMS:
		sprintf_s(szFmtBuf, "%d-%02d-%02d %02d:%02d:%02d", nYear, nMonth, nDay, nHour, nMin, nSec);
		break;
	case FormatTime_YMD:
		sprintf_s(szFmtBuf, "%d-%02d-%02d", nYear, nMonth, nDay);
		break;
	case FormatTime_HMS:
		sprintf_s(szFmtBuf, "%02d:%02d:%02d", nHour, nMin, nSec);
		break;
	case FormatTime_MDHM:
		sprintf_s(szFmtBuf, "%02d-%02d %02d:%02d", nMonth, nDay, nHour, nMin);
		break;
	default:
		assert(0);
		break;
	}

	std::string strRet(szFmtBuf);
	return strRet;	
}