#pragma once

class UsTimezone
{
public:
	UsTimezone();
	virtual ~UsTimezone();
	
	static int GetTimestampTimezone(int nTimestamp);

	//nYear, nMonth, nDay: 日常认知的年月日
	static int GetTMStructTimezone(int nYear, int nMonth, int nDay);

private:
};