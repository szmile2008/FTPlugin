#pragma once
#include "FTPluginQuoteDefine.h"
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
 *	行情操纵接口 IFTQuoteOperation，插件宿主实现，通过查询IFTPluginCore::QueryFTInterface得到
 */
static const GUID IID_IFTQuoteOperation = 
{ 0x9c65990c, 0x903, 0x4185, { 0x97, 0x12, 0x3e, 0xa7, 0xab, 0x34, 0xd, 0xc5 } };

interface IFTQuoteOperation 
{
	//行情定阅，返回错误码
	virtual StockSubErrCode Subscribe_PriceBase(const GUID &guidPlugin, LPCWSTR wstrStockCode,  StockMktType eType, bool bSubb) = 0;  
	virtual StockSubErrCode Subscribe_OrderQueue(const GUID &guidPlugin, LPCWSTR wstrStockCode, StockMktType eType, bool bSubb) = 0;
	virtual StockSubErrCode Subscribe_Ticker(const GUID &guidPlugin, LPCWSTR wstrStockCode,  StockMktType eType, bool bSubb) = 0;  
	virtual StockSubErrCode Subscribe_RTData(const GUID &guidPlugin, LPCWSTR wstrStockCode, StockMktType eType, bool bSubb) = 0; 
	virtual StockSubErrCode Subscribe_KLData(const GUID &guidPlugin, LPCWSTR wstrStockCode, StockMktType eType, bool bSubb, StockSubType eStockSubType) = 0; 
	virtual QueryDataErrCode QueryStockRTData(const GUID &guidPlugin, DWORD* pCookie, LPCWSTR wstrStockCode, StockMktType eType) = 0;
	virtual QueryDataErrCode QueryStockKLData(const GUID &guidPlugin, DWORD* pCookie, LPCWSTR wstrStockCode, StockMktType eType, int nKLType) = 0;

	//请求股票快照，最多一次200个,通过 IQuoteInfoCallback::OnReqStockSnapshot返回
	virtual QueryDataErrCode QueryStockSnapshot(const GUID &guidPlugin, INT64 *arStockHash, int nStockNum, DWORD &dwReqCookie) = 0;
};


/**
 *	行情数据接口 IFTQuoteData，插件宿主实现，通过查询IFTPluginCore::QueryFTInterface得到
 */
static const GUID IID_IFTQuoteData = 
{ 0xb75073e3, 0xaa3a, 0x4717, { 0xac, 0xa2, 0x11, 0x94, 0xa1, 0x3, 0x78, 0xc7 } };

interface IFTQuoteData
{ 
	/**
	* 当前是否订阅某只股票某个订阅位
	*/
	virtual bool   IsSubStockOneType(INT64 ddwStockHash, StockSubType eStockSubType) = 0; 

	/**
	* 当前是否是实时行情
	*/
	virtual bool   IsRealTimeQuotes(INT64 ddwStockHash) = 0; 

	/**
	* stock 的hash值, 回调接口方便 
	*/ 
	virtual INT64  GetStockHashVal(LPCWSTR pstrStockCode, StockMktType eMkt) = 0; 

	/**
	* stock 的hash值, 回调接口方便 
	*/ 
	virtual void  GetStockInfoByHashVal(INT64 ddwStockID, StockMktType& eMkt, wchar_t szStockCode[16]) = 0; 

	/**
	* 填充基础报价 
	*/ 
	virtual bool   FillPriceBase(INT64 ddwStockHash,  Quote_PriceBase* pInfo) = 0; 

	/**
	* 填充十档数据
	*/ 
	virtual bool   FillOrderQueue(INT64 ddwStockHash, Quote_OrderItem* parOrder, int nCount) = 0; 

	/**
	 *	填充内存逐笔数据，不会去server拉新的数据，返回实际fill的个数
	 */
	virtual int    FillTickArr(INT64 ddwStockHash, PluginTickItem *parTickBuf, int nTickBufCount) = 0;

	/**
	* 填充分时数据
	*/ 
	virtual bool   FillRTData(INT64 ddwStockHash, Quote_StockRTData* &pQuoteRT, int& nCount) = 0;

	virtual BOOL   IsRTDataExist(INT64 ddwStockHash) = 0;

	virtual void   DeleteRTDataPointer(Quote_StockRTData* pRTData) = 0; 

	/**
	* 填充K线数据
	*/ 
	virtual BOOL   FillKLData(INT64 ddwStockHash, Quote_StockKLData* &pQuoteKL, int& nCount, int nKLType, int nRehabType) = 0;

	virtual BOOL   IsKLDataExist(INT64 ddwStockHash, int nKLType) = 0;

	virtual void   DeleteKLDataPointer(Quote_StockKLData* pRTData) = 0; 

	/**
	* 
	*/ 
	virtual void   CheckRemoveQuoteRT(INT64 ddwStockID) = 0;

	virtual void   CheckRemoveQuoteKL(INT64 ddwStockID, int nKLType) = 0;

	/**
	 * 得到交易日列表 
	 * @pszDateFrom: "YYYYMMDD"格式，为NULL则默认为pszDateTo往前推一年
	 * @pszDateTo: "YYYYMMDD"格式，为NULL则默认为当前日历时间
	 * @nDateArr:  返回YYYYMMDD格式的整数日期数组，接收方必须将返回的数据copy一份保存起来
	 * @nDateLen:  nDateArr数组长度
	 * @return:    返回true或false表示成功或失败，注意即使成功nDateLen也有可能为0
	 */	
	virtual bool GetTradeDateList(StockMktType mkt, LPCWSTR pszDateFrom, LPCWSTR pszDateTo, int* &narDateArr, int &nDateArrLen) = 0;

	//得到股票列表
	virtual bool GetStocksInfo(StockMktType mkt, PluginSecurityType eSecurityType, LPPluginStockInfo *&parInfo, int &nInfoArrLen) = 0;

	//得到除权除息信息
	//返回值：完全成功返回true, 部分成功或全部失败都返回false
	virtual bool  GetExRightInfo(INT64 *arStockHash, int nStockNum, PluginExRightItem *&arExRightInfo, int &nExRightInfoNum) = 0;

	//得到历史K线 
	//返回值： 如果参数错误或者数据未下载，则返回false；返回的数量满足或不足都返回true
	//pszDateTimeFrom,pszDateTimeTo: 不能为null, 日期字符串格式为YYYY-MM-DD HH:MM:SS
	virtual bool  GetHistoryKLineTimeStr(INT64 ddwStockHash, int nKLType, int nRehabType, LPCWSTR pszDateTimeFrom, LPCWSTR pszDateTimeTo, Quote_StockKLData *&arKLData, int &nKLNum) = 0;
	virtual bool  GetHistoryKLineTimestamp(INT64 ddwStockHash, int nKLType, int nRehabType, INT64 nTimestampFrom, INT64 nTimestampTo, Quote_StockKLData *&arKLData, int &nKLNum) = 0;

	/**
	* dwTime转成wstring 日期+时间
	*/
	virtual void TimeStampToStr(INT64 ddwStockHash, DWORD dwTime, wchar_t szTime[64]) = 0;

	/**
	* dwTime转成wstring 日期
	*/
	virtual void TimeStampToStrDate(INT64 ddwStockHash, DWORD dwTime, wchar_t szData[64]) = 0;

	/**
	* dwTime转成wstring 时间
	*/
	virtual void TimeStampToStrTime(INT64 ddwStockHash, DWORD dwTime, wchar_t szTime[64]) = 0;

	/**
	 * 得到股票订阅情况
	 */	
	virtual bool GetStockSubInfoList(Quote_SubInfo* &pSubInfoArr, int &nSubInfoLen) = 0;

	/**
	* 填充批量报价数据
	*/
	virtual bool FillBatchBasic(INT64 ddwStockHash, Quote_BatchBasic* pInfo) = 0;
}; 

/**
 *  插件宿主通知插件行情数据变化接口
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

	/**
	* 分时数据变化
	*/ 
	virtual void  OnChanged_RTData(INT64 ddwStockHash) = 0; 

		/**
	* 分时数据变化
	*/ 
	virtual void  OnChanged_KLData(INT64 ddwStockHash, int nKLType) = 0; 

	//请求股票快照返回
	virtual void OnReqStockSnapshot(DWORD dwCookie, PluginStockSnapshot *arSnapshot, int nSnapshotNum) = 0;
};

interface IQuoteKLCallback
{
	/**
	* 请求分时回调
	*/ 
	virtual void OnQueryStockRTData(DWORD dwCookie, int nCSResult) = 0; 

	/**
	* 请求K线回调
	*/ 
	virtual void OnQueryStockKLData(DWORD dwCookie, int nCSResult) = 0; 
};

