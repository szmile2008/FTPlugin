#pragma once
#include "FormatTime.h"
#include "Include/FTPluginQuoteDefine.h"

class UtilPlugin
{
public:
	static  int  GetMarketTimezone(StockMktType eMkt, int nTimestamp);
	static  int  GetMarketTimezone2(StockMktType eMkt, int nYear, int nMonth, int nDay);
	static  std::string FormatMktTimestamp(int nTimestamp, StockMktType eMkt, FormatTimeType eFmtType);

};