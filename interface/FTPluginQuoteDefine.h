#pragma once

#ifndef interface
#define interface struct
#endif

/*************************************************
Copyright: FUTU
Author: ysq
Date: 2015-03-18
Description: 行情API和回调接口定义
**************************************************/

/**
*股票的市场类型
*/
enum StockMktType
{
	StockMkt_None = 0,
	StockMkt_HK = 1,  //港股 
	StockMkt_US = 2,  //美股
	StockMkt_SH = 3,  //沪股
	StockMkt_SZ = 4,  //深股
	StockMkt_Feature_Old = 5,  //旧的期货 code: 999000, 999001 （旧期货分时数据在一天连续）
	StockMkt_Feature_New = 6,  //新期货 code: 999010, 999011 （新期货分时数据会跨天，与传统软件保持一致）
};

enum StockSubErrCode
{
	StockSub_Suc = 0,	//订阅成功
	StockSub_FailUnknown = 1,	//未知的失败
	StockSub_FailMaxSubNum = 2,	//到达最大订阅数
	StockSub_FailCodeNoFind = 3,	//代码没找到(也有可能是市场类型错了)
	StockSub_FailGuidNoFind = 4,	//插件GUID传错
	StockSub_FailNoImplInf = 5,		//行情接口未完成
	StockSub_UnSubTimeError = 6,	//未满足反订阅要求时间-1分钟
};

enum QueryDataErrCode
{
	QueryData_Suc = 0,	//查询成功
	QueryData_FailUnknown = 1,	//未知的失败
	QueryData_FailMaxSubNum = 2,	//到达最大查询数
	QueryData_FailCodeNoFind = 3,	//代码没找到(也有可能是市场类型错了)
	QueryData_FailGuidNoFind = 4,	//插件GUID传错
	QueryData_FailNoImplInf = 5,		//行情接口未完成
	QueryData_IsExisted = 6,
};

enum StockSubType
{
	StockSubType_Simple = 1,
	StockSubType_Gear = 2,
	StockSubType_Ticker = 4,
	StockSubType_RT = 5,
	StockSubType_KL_DAY = 6,
	StockSubType_KL_MIN5 = 7,
	StockSubType_KL_MIN15 = 8,
	StockSubType_KL_MIN30 = 9,
	StockSubType_KL_MIN60 = 10,
	StockSubType_KL_MIN1 = 11,
	StockSubType_KL_WEEK = 12,
	StockSubType_KL_MONTH = 13,
};

enum PluginSecurityType
{
	PluginSecurity_All = 0,
	PluginSecurity_Bond = 1, //债券	
	PluginSecurity_Stock = 3, //正股	
	PluginSecurity_ETF = 4,
	PluginSecurity_Warrant = 5, //涡轮牛熊		
	PluginSecurity_Index = 6,
};

/**
* 股票基础报价信息：
* 价格精度是3位小数， 如报价8.888存储值 88888
*/
typedef struct tagQuotePriceBase
{
	DWORD dwOpen;		//开盘价
	DWORD dwLastClose;  //昨收价
	DWORD dwCur;		//当前价
	DWORD dwHigh;		//最高价
	DWORD dwLow;		//最低价
	INT64 ddwVolume;	//成交量
	INT64 ddwTrunover;	//成交额
	DWORD dwTime;		//报价时间
	DWORD ddwLotSize;	//每手数量
}Quote_PriceBase, *LPQuote_PriceBase;


/**
* 股票十档数据
* IFTQuoteData::FillOrderQueue 的接口参数
*/
typedef struct tagQuoteOrderItem
{
	DWORD	dwBuyPrice, dwSellPrice;  //买价 卖价
	INT64	ddwBuyVol, ddwSellVol;    //买量 卖量
	int		nBuyOrders, nSellOrders;  //档位 
}Quote_OrderItem, *LPQuote_OrderItem;

typedef struct tagPluginTickItem
{
	DWORD dwPrice;
	DWORD dwTime;
	int nDealType;
	INT64 nSequence;
	INT64 nVolume;
	INT64 nTurnover; //成交额
}PluginTickItem, *LPPluginTickItem;

/**
* 分时数据
*/
typedef struct tagQuoteStockRTData
{
	int   nDataStatus;
	DWORD dwTime;
	DWORD dwOpenedMins;  //开盘第多少分钟  

	int   nCurPrice;
	DWORD nLastClosePrice; //昨天的收盘价 

	int   nAvgPrice;

	INT64 ddwTDVolume;
	INT64 ddwTDValue;
}Quote_StockRTData, *LPQuote_StockRTData;

/**
* K线数据
*/
typedef struct tagQueryStockKLData
{
	int   nDataStatus;
	DWORD dwTime;

	int   nOpenPrice;
	int   nClosePrice;

	int   nHighestPrice;
	int   nLowestPrice;

	int   nPERatio; //市盈率(三位小数)
	int   nTurnoverRate;//换手率(正股及指数的日/周/月K线)

	INT64 ddwTDVol;
	INT64 ddwTDVal;
}Quote_StockKLData, *LPQuote_StockKLData;

typedef struct tagSubInfo
{
	INT64 ddwStockHash;
	int   nStockSubType;
}Quote_SubInfo, *LPQuote_SubInfo;

//nKLType:
//1 = 1分K;  
//2 = 日K; 
//3 = 周K;   
//4 = 月K;
//6 = 5分K; 
//7 = 15分K;  
//8 = 30分K;  
//9 = 60分K;
//
//nRehabType:
//0 = 不复权；
//1 = 前复权；
//2 = 后复权；

//nStockSubType:
//1 = 报价
//2 = 摆盘
//4 = 逐笔
//5 = 分时//未做
//6 = 日K
//7 =  5分K
//8 =  15分K
//9 =  30分K
//10 =  60分K
//11 =  1分K
//12 = 周K
//13 = 月K

typedef struct tagPluginStockInfo
{
	INT64 nStockID;
	int  nLotSize;
	PluginSecurityType nSecType;
	WCHAR chSimpName[64];
	WCHAR chCodeSig[16];
}PluginStockInfo, *LPPluginStockInfo;

typedef struct tagBatchBasic
{
	DWORD dwOpen;		//开盘价
	DWORD dwLastClose;  //昨收价
	DWORD dwCur;		//当前价
	DWORD dwHigh;		//最高价
	DWORD dwLow;		//最低价
	DWORD dwAmpli;
	INT64 ddwVolume;	//成交量
	INT64 ddwTurnover;	//成交额
	int   nSuspension;
	int   nTurnoverRate;
	DWORD dwTime;		//报价时间
	DWORD dwListTime;	//上市时间
}Quote_BatchBasic, *LPQuote_BatchBasic;

enum PlugErtFlag
{
	Ert_NONE = 0x00,
	Ert_SPLIT = 0x01,    //拆股
	Ert_JOIN = 0x02,	 //合股
	Ert_BONUS_STK = 0x04,//送股
	Ert_INTOSHARES = 0x08,//转增股
	Ert_ALLOT = 0x10,    //配股
	Ert_ADD = 0x20,		//增发股
	Ert_DIVIDEND = 0x40,//有现金分红
	Ert_SPECIALDIVIDEND = 0x80, //有现金分红
};

//除权记录：
struct PluginExRightItem
{
	INT64 stock_id;
	UINT ex_date;    // 除权除息日期, 例如20160615
	UINT ert_flag;    // 公司行动类型组合，ErtFlag

	//拆股(eg. 1拆5，Base为1，ERT为5)
	UINT split_base;
	UINT split_ert;

	//合股(eg. 50合1，Base为50，ERT为1)
	UINT join_base;
	UINT join_ert;

	//送股(eg. 10送3, Base为10,ERT为3)
	UINT bonus_stk_base;
	UINT bonus_stk_ert;

	//配股(eg. 10送2, 配股价为6.3元, Base为10, ERT为2, Price为6300)
	UINT allot_base;
	UINT allot_ert;
	INT64 allot_price;

	//转增股(跟送股类似)
	UINT into_shr_base;
	UINT into_ert;

	//增发(跟配股类似)
	UINT stk_add_base;
	UINT stk_add_ert;
	INT64 stk_add_price;

	// 现金分红(eg. 每10股派现0.5元，Base为10, Amount为500)
	UINT dividend_base;
	INT64 dividend_amount;

	// 特别股息
	UINT dividend_special_base;
	UINT dividend_special_amount;

	// result_self
	double fwd_factor_a;
	double fwd_factor_b;
	double bwd_factor_a;
	double bwd_factor_b;

	// 简体中文文本描述
	wchar_t *sc_txt;

	// 繁体中文文本描述
	wchar_t *tc_txt;
};

// 股票的行情快照数据
struct PluginStockSnapshot
{
	INT64 stock_id;    // 股票id
	int  ret;    // 是否找到快照记录，0为成功找到，snapshot有数据。其他值，snapshot无数据（可能是找不到股票）
	char   stock_code[16];
	UINT instrument_type;
	UINT market_code;

	// 价格相关
	double nominal_price;
	double last_close_price;
	double open_price;
	INT64 update_time;

	INT64 suspend_flag;
	INT64 listing_status;
	INT64 listing_date;

	// 成交统计信息
	INT64 shares_traded;
	double turnover;
	double highest_price;
	double lowest_price;
	float  turnover_ratio;
};