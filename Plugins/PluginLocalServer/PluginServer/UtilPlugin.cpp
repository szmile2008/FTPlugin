#include "stdafx.h"
#include "UtilPlugin.h"
#include "UsTimezone.h"

int  UtilPlugin::GetMarketTimezone(StockMktType eMkt, int nTimestamp)
{
	switch (eMkt)
	{
	case StockMkt_HK:
	case StockMkt_Feature_Old:
	case StockMkt_Feature_New:
	case StockMkt_SH:
	case StockMkt_SZ:
		return 8;
		break;
	case StockMkt_US:
		return UsTimezone::GetTimestampTimezone(nTimestamp);
		break;
	default:
		assert(0);
		return 0;
		break;
	}
}

int  UtilPlugin::GetMarketTimezone2(StockMktType eMkt, int nYear, int nMonth, int nDay)
{
	switch (eMkt)
	{
	case StockMkt_HK:
	case StockMkt_Feature_Old:
	case StockMkt_Feature_New:
	case StockMkt_SH:
	case StockMkt_SZ:
		return 8;
		break;
	case StockMkt_US:
		return UsTimezone::GetTMStructTimezone(nYear, nMonth, nDay);
			break;
	default:
		assert(0);
		return 0;
		break;
	}
}

std::string UtilPlugin::FormatMktTimestamp(int nTimestamp, StockMktType eMkt, FormatTimeType eFmtType)
{
	int nTimezone = GetMarketTimezone(eMkt, nTimestamp);
	std::string strFmt = FormatTime::FormatTimestamp(nTimestamp, nTimezone, eFmtType);
	return strFmt;
}