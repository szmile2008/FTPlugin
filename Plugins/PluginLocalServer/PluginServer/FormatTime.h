#pragma once

enum FormatTimeType
{
	FormatTime_YMDHMS,
	FormatTime_YMD,
	FormatTime_HMS,
	FormatTime_MDHM,
};

class FormatTime
{
public:
	FormatTime();
	virtual ~FormatTime();	
	
	static std::string FormatTimestamp(int nTimestamp, int nTimezone, FormatTimeType eFmtType);

	//nYear, nMonth, nDay: 日常认知的年月日
	static std::string FormatTimeStruct(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, FormatTimeType eFmtType);
};