#pragma once

#ifndef interface
#define interface struct
#endif

/************************************************* 
Copyright: FUTU
Author: Lin
Date: 2015-03-18
Description: 交易API和回调接口定义
Ver: 1.2

更新记录: 
1. 2015-12-28 增加查询订单、账户、持仓接口，版本升级为1.1
2. 2016-01-07 增加美股API，版本升级为1.2
	特别提醒！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	交易API中价格、金额类的数据若为浮点型，即是原始数据没有被放大；若是整型，则是浮点值×1000，即最小单位是0.001元
**************************************************/  

enum Trade_Env
{
	Trade_Env_Real = 0, //真实环境（实盘交易）
	Trade_Env_Virtual = 1, //虚拟环境（仿真交易或模拟交易）
};

enum Trade_SvrResult
{
	Trade_SvrResult_Succeed = 0, //服务器返回请求处理成功
	Trade_SvrResult_Failed = -1, //服务器返回请求处理失败（失败的原因可能很多，包括网络超时，暂时统一返回失败）
};

enum Trade_OrderSide
{
	Trade_OrderSide_Buy = 0, //买入
	Trade_OrderSide_Sell = 1, //卖出
	Trade_OrderSide_SellShort = 2, //卖空(目前仅美股)
	Trade_OrderSide_BuyBack = 3, //卖空补回(目前仅美股)
};

enum Trade_OrderStatus
{
	Trade_OrderStatus_Processing = 0, //服务器处理中...
	Trade_OrderStatus_WaitDeal = 1, //等待成交
	Trade_OrderStatus_PartDealt = 2, //部分成交
	Trade_OrderStatus_AllDealt = 3, //全部成交
	Trade_OrderStatus_Disabled = 4, //已失效
	Trade_OrderStatus_Failed = 5, //下单失败（订单已产生，后续的失败）
	Trade_OrderStatus_Cancelled = 6, //已撤单
	Trade_OrderStatus_Deleted = 7, //已删除
	Trade_OrderStatus_WaitOpen = 8, //等待开盘
	Trade_OrderStatus_LocalSent = 21, //本地已发送
	Trade_OrderStatus_LocalFailed = 22, //本地已发送，服务器返回下单失败，没产生订单
	Trade_OrderStatus_LocalTimeOut = 23, //本地已发送，等待服务器返回超时
};

//港股的一些枚举定义
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum Trade_OrderType_HK
{
	//目前只支持0、1、3

	Trade_OrderType_HK_EnhancedLimit = 0, //增强限价单(普通交易)
	Trade_OrderType_HK_Auction = 1, //竞价单(竞价交易)
	Trade_OrderType_HK_Limit = 2, //限价单
	Trade_OrderType_HK_AuctionLimit = 3, //竞价限价单(竞价限价)
	Trade_OrderType_HK_SpecialLimit = 4, //特别限价单
};

enum Trade_SetOrderStatus
{
	Trade_SetOrderStatus_Cancel = 0, //撤单
	Trade_SetOrderStatus_Disable = 1, //失效
	Trade_SetOrderStatus_Enable = 2, //生效
	Trade_SetOrderStatus_Delete = 3, //删除

	//以下并不是真正状态、是收到OnOrderErrNotify后对对应的错误提示作出确认

	Trade_SetOrderStatus_HK_SplitLargeOrder = 11, //确认拆分大单
	Trade_SetOrderStatus_HK_PriceTooFar = 12, //确认价格偏离太大也继续
	Trade_SetOrderStatus_HK_BuyWolun = 13, //确认买卖窝轮
	Trade_SetOrderStatus_HK_BuyGuQuan = 14, //确认买卖股权
	Trade_SetOrderStatus_HK_BuyLowPriceStock = 15, //确认买卖低价股
};

enum Trade_OrderErrNotify_HK
{
	Trade_OrderErrNotify_HK_Normal = 0, //普通错误

	//以下错误，是提示性的，如果不确认订单保持失败状态，如果通过SetOrderStatus对对应错误做出确认，则订单重新生效

	Trade_OrderErrNotify_HK_LargeOrder = 1, //订单数量太大
	Trade_OrderErrNotify_HK_PriceTooFar = 2, //订单价格偏离太大
	Trade_OrderErrNotify_HK_FengXian_Wolun = 3, //买卖窝轮有风险
	Trade_OrderErrNotify_HK_FengXian_GuQuan = 4, //买卖股权有风险
	Trade_OrderErrNotify_HK_FengXian_LowPriceStock = 5, //买卖低价股有风险
};

//美股的一些枚举定义
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum Trade_OrderType_US
{
	Trade_OrderType_US_Market = 1, //市价单
	Trade_OrderType_US_Limit = 2, //限价

	Trade_OrderType_US_PreMarket = 51, //盘前交易，限价
	Trade_OrderType_US_PostMarket = 52 //盘后交易，限价
};

//订单、成交记录、账户、持仓数据结构
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Trade_OrderItem
{
	//特别提醒！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	//交易API中价格、金额类的数据若为浮点型，即是原始数据没有被放大；若是整型，则是浮点值×1000，即最小单位是0.001元

	UINT64 nLocalID; //客户端产生的订单ID，非订单真正的ID，用于关联
	UINT64 nOrderID; //订单号，服务器产生的订单真正的ID

	UINT8 nType; //不同市场、取值对应具体的枚举定义 Trade_OrderType_HK 或 Trade_OrderType_US
	Trade_OrderSide enSide;
	UINT8 nStatus; //取值对应具体的枚举定义Trade_OrderStatus
	WCHAR szCode[16];
	WCHAR szName[128];
	UINT64 nPrice;
	UINT64 nQty;
	UINT64 nDealtQty; //成交数量
	double fDealtAvgPrice; //成交均价，没有放大
	
	UINT64 nSubmitedTime; //服务器收到的订单提交时间
	UINT64 nUpdatedTime; //订单最后更新的时间

	//只支持港股调用GetErrDesc传nErrCode来得到错误描述，后续请都调用GetErrDescV2传nErrCode或nErrDescStrHash
	UINT16 nErrCode; //错误码，仅支持港股
	INT64 nErrDescStrHash; //错误描述字符串的hash
};

struct Trade_DealItem
{
	//特别提醒！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	//交易API中价格、金额类的数据若为浮点型，即是原始数据没有被放大；若是整型，则是浮点值×1000，即最小单位是0.001元

	UINT64 nOrderID; //订单号，服务器产生的订单真正的ID
	UINT64 nDealID; //成交号

	Trade_OrderSide enSide; //方向

	WCHAR szCode[16]; //代码
	WCHAR szName[128]; //名称
	UINT64 nPrice; //成交价格
	UINT64 nQty; //成交数量

	UINT64 nTime;	//成交时间
};

struct Trade_AccInfo
{
	//特别提醒！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	//交易API中价格、金额类的数据若为浮点型，即是原始数据没有被放大；若是整型，则是浮点值×1000，即最小单位是0.001元

	INT64 nPower; //购买力
	INT64 nZcjz; //资产净值
	INT64 nZqsz; //证券市值
	INT64 nXjjy; //现金结余
	INT64 nKqxj; //可取现金
	INT64 nDjzj; //冻结资金
	INT64 nZsje; //追收金额

	INT64 nZgjde; //最高借贷额
	INT64 nYyjde; //已用信贷额
	INT64 nGpbzj; //股票保证金
};

struct Trade_PositionItem
{
	//特别提醒！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	//交易API中价格、金额类的数据若为浮点型，即是原始数据没有被放大；若是整型，则是浮点值×1000，即最小单位是0.001元

	WCHAR szCode[16];
	WCHAR szName[128];

	INT64 nQty; //持有数量
	INT64 nCanSellQty; //可卖数量
	INT64 nNominalPrice; //市价
	INT64 nMarketVal; //市值

	double fCostPrice; //成本价
	bool bCostPriceValid; //成本价是否有效
	INT64 nPLVal; //盈亏金额
	bool bPLValValid; //盈亏金额是否有效
	double fPLRatio; //盈亏比例
	bool bPLRatioValid; //盈亏比例是否有效

	INT64 nToday_PLVal; //今日盈亏金额
	INT64 nToday_BuyQty; //今日买入成交量
	INT64 nToday_BuyVal; //今日买入成交额
	INT64 nToday_SellQty; //今日卖出成交量
	INT64 nToday_SellVal; //今日卖出成交额
};

//港股交易API调用/回调接口定义
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	港股交易接口 ITrade_HK, 插件宿主实现，插件通过查询IFTPluginCore::QueryFTInterface得到
 */
static const GUID IID_IFTTrade_HK = 
{ 0x69a88049, 0x252e, 0x4a12, { 0x83, 0x41, 0xdd, 0x4c, 0x6e, 0x84, 0x8b, 0x27 } };

interface ITrade_HK
{
	/**
	* 解锁

	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param lpszPassword 密码.

	* @return true发送成功，false发送失败.
	*/
	virtual bool UnlockTrade(UINT32* pCookie, LPCWSTR lpszPassword) = 0;

	/**
	* 下单

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param enType 订单类型.
	* @param enSide 订单方向，买或卖.
	* @param lpszCode 股票代码.
	* @param nPrice 订单价格. 注意：是浮点值×1000，即最小单位是0.001元
	* @param nQty 订单数量.

	* @return true发送成功，false发送失败.
	*/
	virtual bool PlaceOrder(Trade_Env enEnv, UINT32* pCookie, Trade_OrderType_HK enType, Trade_OrderSide enSide, LPCWSTR lpszCode, UINT64 nPrice, UINT64 nQty) = 0;

	/**
	* 设置订单状态

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param nOrderID 订单真正的ID.
	* @param enStatus 设置为何种状态.

	* @return true发送成功，false发送失败.
	*/
	virtual bool SetOrderStatus(Trade_Env enEnv, UINT32* pCookie, UINT64 nOrderID, Trade_SetOrderStatus enStatus) = 0;

	/**
	* 改单

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param nOrderID 订单真正的ID.
	* @param nPrice 新的订单价格. 注意：是浮点值×1000，即最小单位是0.001元
	* @param nQty 新的订单数量.

	* @return true发送成功，false发送失败.
	*/
	virtual bool ChangeOrder(Trade_Env enEnv, UINT32* pCookie, UINT64 nOrderID, UINT64 nPrice, UINT64 nQty) = 0;

	/**
	* 通过错误码得到错误描述

	* @param nErrCode 错误码.
	* @param szErrDesc 错误描述.

	* @return true获取成功，false获取失败.
	*/
	virtual bool GetErrDesc(UINT16 nErrCode, WCHAR szErrDesc[256]) = 0;

	/**
	* 通过错误码或错误描述Hash得到错误描述

	* @param nErrCodeOrHash 错误码或错误描述Hash.
	* @param szErrDesc 错误描述.

	* @return true获取成功，false获取失败.
	*/
	virtual bool GetErrDescV2(INT64 nErrCodeOrHash, WCHAR szErrDesc[256]) = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//以下为查询运行时交易内存数据，即返回数据与是牛牛客户端显示数据一致，虽不会请求服务器，但服务器与客户端是TCP长连接，服务器在实时推送最新数据

	/**
	* 查询订单列表

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于查询结果回调时做对应关系判断.

	* @return true查询成功，false查询失败.
	*/
	virtual bool QueryOrderList(Trade_Env enEnv, UINT32* pCookie) = 0;

	/**
	* 查询成交记录列表

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于查询结果回调时做对应关系判断.

	* @return true查询成功，false查询失败.
	*/
	virtual bool QueryDealList(Trade_Env enEnv, UINT32* pCookie) = 0;

	/**
	* 查询账户信息

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于查询结果回调时做对应关系判断.

	* @return true查询成功，false查询失败.
	*/
	virtual bool QueryAccInfo(Trade_Env enEnv, UINT32* pCookie) = 0;

	/**
	* 查询持仓列表

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于查询结果回调时做对应关系判断.

	* @return true查询成功，false查询失败.
	*/
	virtual bool QueryPositionList(Trade_Env enEnv, UINT32* pCookie) = 0;

	/**
	* 通过本地订单id得到svr订单id  

	* @param Trade_Env  交易环境(实盘交易或仿真交易).
	* @param nLocalID   订单本地id.

	* @return  订单ServerID , 如果还没生成或者查找不到返回0
	*/
	virtual INT64 FindOrderSvrID(Trade_Env enEnv, INT64 nLocalID)=0; 

};

interface ITradeCallBack_HK
{
	/**
	* 解锁交易请求返回

	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nErrCode 错误码.
	*/
	virtual void OnUnlockTrade(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nErrCode) = 0;

	/**
	* 下单请求返回

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nLocalID 客户端产生的订单ID，用于与后续推送订单关联.
	* @param nErrCode 错误码.
	*/
	virtual void OnPlaceOrder(Trade_Env enEnv, UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nLocalID, UINT16 nErrCode) = 0;

	/**
	* 订单更新推送

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param orderItem 订单结构体.
	*/
	virtual void OnOrderUpdate(Trade_Env enEnv, const Trade_OrderItem& orderItem) = 0;

	/**
	* 设置订单状态请求返回

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nOrderID 订单号.
	* @param nErrCode 错误码.
	*/
	virtual void OnSetOrderStatus(Trade_Env enEnv, UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, UINT16 nErrCode) = 0;

	/**
	* 改单请求返回

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nOrderID 订单号.
	* @param nErrCode 错误码.
	*/
	virtual void OnChangeOrder(Trade_Env enEnv, UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, UINT16 nErrCode) = 0;

	/**
	* 订单错误推送
	
	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nOrderID 订单号.
	* @param enErrNotify 订单错误类型.
	* @param nErrCode 错误码.
	*/
	virtual void OnOrderErrNotify(Trade_Env enEnv, UINT64 nOrderID, Trade_OrderErrNotify_HK enErrNotify, UINT16 nErrCode) = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//以下为查询运行时交易内存数据的回调，返回数据与是牛牛客户端显示数据一致，虽不会请求服务器，但服务器与客户端是TCP长连接，服务器在实时推送最新数据

	/**
	* 查询订单列表回调

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param nCount 订单个数.
	* @param pArrOrder 订单数组指针.
	*/
	virtual void OnQueryOrderList(Trade_Env enEnv, UINT32 nCookie, INT32 nCount, const Trade_OrderItem* pArrOrder) = 0;

	/**
	* 查询成交记录列表回调

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param nCount 成交记录个数.
	* @param pArrDeal 成交记录数组指针.
	*/
	virtual void OnQueryDealList(Trade_Env enEnv, UINT32 nCookie, INT32 nCount, const Trade_DealItem* pArrDeal) = 0;

	/**
	* 查询账户信息回调

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param accInfo 账户信息结构体.
	*/
	virtual void OnQueryAccInfo(Trade_Env enEnv, UINT32 nCookie, const Trade_AccInfo& accInfo) = 0;

	/**
	* 查询持仓列表回调

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param nCount 持仓个数.
	* @param pArrPosition 持仓数组指针.
	*/
	virtual void OnQueryPositionList(Trade_Env enEnv, UINT32 nCookie, INT32 nCount, const Trade_PositionItem* pArrPosition) = 0;
};

//美股交易API调用/回调接口定义
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 *	美股交易接口 ITrade_US, 插件宿主实现，通过查询IFTPluginCore::QueryFTInterface得到
 */
static const GUID IID_IFTTrade_US = 
{ 0x66c2e76d, 0x8786, 0x4bf0, { 0x95, 0x34, 0xd2, 0x86, 0x4d, 0x53, 0x9, 0xc6 } };
interface ITrade_US
{
	/**
	* 下单

	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param enType 订单类型.
	* @param enSide 订单方向，买或卖.
	* @param lpszCode 股票代码.
	* @param nPrice 订单价格. 注意：是浮点值×1000，即最小单位是0.001元
	* @param nQty 订单数量.

	* @return true发送成功，false发送失败.
	*/
	virtual bool PlaceOrder(UINT32* pCookie, Trade_OrderType_US enType, Trade_OrderSide enSide, LPCWSTR lpszCode, UINT64 nPrice, UINT64 nQty) = 0;

	/**
	* 撤销订单

	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param nOrderID 订单真正的ID.

	* @return true发送成功，false发送失败.
	*/
	virtual bool CancelOrder(UINT32* pCookie, UINT64 nOrderID) = 0;

	/**
	* 改单

	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param nOrderID 订单真正的ID.
	* @param nPrice 新的订单价格.  注意：是浮点值×1000，即最小单位是0.001元
	* @param nQty 新的订单数量.

	* @return true发送成功，false发送失败.
	*/
	virtual bool ChangeOrder(UINT32* pCookie, UINT64 nOrderID, UINT64 nPrice, UINT64 nQty) = 0;

	/**
	* 通过错误描述Hash得到错误描述

	* @param nErrHash 错误描述Hash.
	* @param szErrDesc 错误描述.

	* @return true获取成功，false获取失败.
	*/
	virtual bool GetErrDescV2(INT64 nErrHash, WCHAR szErrDesc[256]) = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//以下为查询运行时交易内存数据，即返回数据与是牛牛客户端显示数据一致，虽不会请求服务器，但服务器与客户端是TCP长连接，服务器在实时推送最新数据

	/**
	* 查询订单列表

	* @param pCookie 接收本次调用对应的Cookie值，用于查询结果回调时做对应关系判断.

	* @return true查询成功，false查询失败.
	*/
	virtual bool QueryOrderList(UINT32* pCookie) = 0;

	/**
	* 查询成交记录列表

	* @param pCookie 接收本次调用对应的Cookie值，用于查询结果回调时做对应关系判断.

	* @return true查询成功，false查询失败.
	*/
	virtual bool QueryDealList(UINT32* pCookie) = 0;

	/**
	* 查询账户信息

	* @param pCookie 接收本次调用对应的Cookie值，用于查询结果回调时做对应关系判断.

	* @return true查询成功，false查询失败.
	*/
	virtual bool QueryAccInfo(UINT32* pCookie) = 0;

	/**
	* 查询持仓列表

	* @param pCookie 接收本次调用对应的Cookie值，用于查询结果回调时做对应关系判断.

	* @return true查询成功，false查询失败.
	*/
	virtual bool QueryPositionList(UINT32* pCookie) = 0;

	/**
	* 通过本地订单id得到svr订单id  

	* @param nLocalID   订单本地id.

	* @return  订单ServerID , 如果还没生成或者查找不到返回0
	*/
	virtual INT64 FindOrderSvrID(INT64 nLocalID)=0; 
};

interface ITradeCallBack_US
{
	/**
	* 下单请求返回

	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nLocalID 客户端产生的订单ID，用于与后续推送订单关联.
	* @param nErrHash 错误描述Hash.
	*/
	virtual void OnPlaceOrder(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nLocalID, INT64 nErrHash) = 0;

	/**
	* 订单更新推送

	* @param orderItem 订单结构体.
	*/
	virtual void OnOrderUpdate(const Trade_OrderItem& orderItem) = 0;

	/**
	* 设置订单状态请求返回

	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nOrderID 订单号.
	* @param nErrHash 错误描述Hash.
	*/
	virtual void OnCancelOrder(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, INT64 nErrHash) = 0;

	/**
	* 改单请求返回

	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nOrderID 订单号.
	* @param nErrHash 错误描述Hash.
	*/
	virtual void OnChangeOrder(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, INT64 nErrHash) = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//以下为查询运行时交易内存数据的回调，返回数据与是牛牛客户端显示数据一致，虽不会请求服务器，但服务器与客户端是TCP长连接，服务器在实时推送最新数据

	/**
	* 查询订单列表回调

	* @param nCookie 请求时的Cookie.
	* @param nCount 订单个数.
	* @param pArrOrder 订单数组指针.
	*/
	virtual void OnQueryOrderList(UINT32 nCookie, INT32 nCount, const Trade_OrderItem* pArrOrder) = 0;

	/**
	* 查询交易记录列表回调

	* @param nCookie 请求时的Cookie.
	* @param nCount 交易记录个数.
	* @param pArrDeal 交易记录数组指针.
	*/
	virtual void OnQueryDealList(UINT32 nCookie, INT32 nCount, const Trade_DealItem* pArrDeal) = 0;

	/**
	* 查询账户信息回调

	* @param nCookie 请求时的Cookie.
	* @param accInfo 账户信息结构体.
	*/
	virtual void OnQueryAccInfo(UINT32 nCookie, const Trade_AccInfo& accInfo) = 0;

	/**
	* 查询持仓列表回调

	* @param nCookie 请求时的Cookie.
	* @param nCount 持仓个数.
	* @param pArrPosition 持仓数组指针.
	*/
	virtual void OnQueryPositionList(UINT32 nCookie, INT32 nCount, const Trade_PositionItem* pArrPosition) = 0;
};
