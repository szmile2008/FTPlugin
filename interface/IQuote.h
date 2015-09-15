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
	StockMkt_HK = 1,  //港股 
	StockMkt_US = 2,  //美股
	StockMkt_SH = 3,  //沪股
	StockMkt_SZ = 4,  //深股
}; 

enum StockSubErrCode
{
	StockSub_Suc = 0,	//订阅成功
	StockSub_FailUnknown	= 1,	//未知的失败
	StockSub_FailMaxSubNum	= 2,	//到达最大订阅数
	StockSub_FailCodeNoFind = 3,	//代码没找到(也有可能是市场类型错了)
	StockSub_FailGuidNoFind = 4,	//插件GUID传错
	StockSub_FailNoImplInf = 5,		//行情接口未完成
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
}Quote_PriceBase, *LPQuote_PriceBase;


/**
* 股票十档数据
* IFTQuoteData::FillOrderQueue 的接口参数  
*/
typedef struct tagQuoteOrderQueue
{
	DWORD	dwBuyPrice, dwSellPrice;  //买价 卖价
	INT64	ddwBuyVol, ddwSellVol;    //买量 卖量
	int		nBuyOrders, nSellOrders;  //档位 
}Quote_OrderQueue, *LPQuote_OrderQueue;  


/**
* 行情操作接口 
*/
interface IFTQuoteOperation 
{
	//行情定阅，返回错误码
	virtual StockSubErrCode Subscribe_PriceBase(const GUID &guidPlugin, LPCWSTR wstrStockCode,  StockMktType eType, bool bSubb) = 0;  
	virtual StockSubErrCode Subscribe_OrderQueue(const GUID &guidPlugin, LPCWSTR wstrStockCode, StockMktType eType, bool bSubb) = 0; 
};

/**
* 行情数据的接口
*/
interface IFTQuoteData
{ 
	/**
	* 当前是否是实时行情
	*/
	virtual bool   IsRealTimeQuotes() = 0; 

	/**
	* stock 的hash值, 回调接口方便 
	*/ 
	virtual INT64  GetStockHashVal(LPCWSTR pstrStockCode, StockMktType eMkt) = 0; 

	/**
	* 填充基础报价 
	*/ 
	virtual bool   FillPriceBase(INT64 ddwStockHash,  Quote_PriceBase* pInfo) = 0; 

	/**
	* 填充十档数据
	*/ 
	virtual bool   FillOrderQueue(INT64 ddwStockHash, Quote_OrderQueue* parQuote, int nCount) = 0; 
}; 

/**
* 行情数据回调
*/
interface IQuoteInfoCallback
{ 
	/**
	* 基础报价信息变化 
	*/ 
	virtual void  OnChanged_PriceBase(INT64  ddwStockHash) = 0; 

	/**
	* 十档数据变化
	*/ 
	virtual void  OnChanged_OrderQueue(INT64 ddwStockHash) = 0; 
};

