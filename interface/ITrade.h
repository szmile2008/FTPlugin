#pragma once

#ifndef interface
#define interface struct
#endif


/************************************************* 
Copyright: FUTU
Author: Lin
Date: 2015-03-18
Description: 交易API和回调接口定义
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
};

enum Trade_OrderType_HK
{
	//目前只支持0、1、3
	Trade_OrderType_HK_EnhancedLimit = 0, //增强限价单(普通交易)
	Trade_OrderType_HK_Auction = 1, //竞价单(竞价交易)
	Trade_OrderType_HK_Limit = 2, //限价单
	Trade_OrderType_HK_AuctionLimit = 3, //竞价限价单(竞价限价)
	Trade_OrderType_HK_SpecialLimit = 4, //特别限价单
};

enum Trade_OrderStatus_HK
{
	Trade_OrderStatus_HK_Processing = 0, //服务器处理中...
	Trade_OrderStatus_HK_WaitDeal = 1, //等待成交
	Trade_OrderStatus_HK_PartDealt = 2, //部分成交
	Trade_OrderStatus_HK_AllDealt = 3, //全部成交
	Trade_OrderStatus_HK_Disabled = 4, //已失效
	Trade_OrderStatus_HK_Failed = 5, //下单失败（订单已产生，后续的失败）
	Trade_OrderStatus_HK_Cancelled = 6, //已撤单
	Trade_OrderStatus_HK_Deleted = 7, //已删除
	Trade_OrderStatus_HK_WaitOpen = 8, //等待开盘
	Trade_OrderStatus_HK_LocalSent = 21, //本地已发送
	Trade_OrderStatus_HK_LocalFailed = 22, //本地已发送，服务器返回下单失败，没产生订单
	Trade_OrderStatus_HK_LocalTimeOut = 23, //本地已发送，等待服务器返回超时
};

enum Trade_SetOrderStatus_HK
{
	Trade_SetOrderStatus_HK_Cancel = 0, //撤单
	Trade_SetOrderStatus_HK_Disable = 1, //失效
	Trade_SetOrderStatus_HK_Enable = 2, //生效
	Trade_SetOrderStatus_HK_Delete = 3, //删除

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

	//以下错误，是提示性的，如果不确认订单保持失败状态，如果通过
	//SetOrderStatus对对应错误做出确认，则订单重新生效
	Trade_OrderErrNotify_HK_LargeOrder = 1, //订单数量太大
	Trade_OrderErrNotify_HK_PriceTooFar = 2, //订单价格偏离太大
	Trade_OrderErrNotify_HK_FengXian_Wolun = 3, //买卖窝轮有风险
	Trade_OrderErrNotify_HK_FengXian_GuQuan = 4, //买卖股权有风险
	Trade_OrderErrNotify_HK_FengXian_LowPriceStock = 5, //买卖低价股有风险
};

struct Trade_OrderItem_HK
{
	UINT64 nLocalID; //客户端产生的订单ID，非订单真正的ID，用于关联
	UINT64 nOrderID; //订单号，服务器产生的订单真正的ID

	Trade_OrderType_HK enType;
	Trade_OrderSide enSide;
	Trade_OrderStatus_HK enStatus;
	WCHAR szCode[16];
	WCHAR szName[128];
	UINT64 nPrice;
	UINT64 nQty;
	UINT64 nDealtQty; //成交数量
	double fDealtAvgPrice; //成交均价
	
	UINT64 nSubmitedTime; //服务器收到的订单提交时间
	UINT64 nUpdatedTime; //订单最后更新的时间

	UINT16 nErrCode; //错误码
};

interface ITrade_HK
{
	/**
	* 下单

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param enType 订单类型.
	* @param enSide 订单方向，买或卖.
	* @param lpszCode 股票代码.
	* @param nPrice 订单价格.
	* @param nQty 订单数量.

	* @return true发送成功，false发送失败.
	*/
	virtual bool PlaceOrder(Trade_Env enEnv, UINT* pCookie, Trade_OrderType_HK enType, Trade_OrderSide enSide, LPCWSTR lpszCode, UINT64 nPrice, UINT64 nQty) = 0;

	/**
	* 设置订单状态

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param nOrderID 订单真正的ID.
	* @param enStatus 设置为何种状态.

	* @return true发送成功，false发送失败.
	*/
	virtual bool SetOrderStatus(Trade_Env enEnv, UINT* pCookie, UINT64 nOrderID, Trade_SetOrderStatus_HK enStatus) = 0;

	/**
	* 改单

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param nOrderID 订单真正的ID.
	* @param nPrice 新的订单价格.
	* @param nQty 新的订单数量.

	* @return true发送成功，false发送失败.
	*/
	virtual bool ChangeOrder(Trade_Env enEnv, UINT* pCookie, UINT64 nOrderID, UINT64 nPrice, UINT64 nQty) = 0;

	/**
	* 通过错误码得到错误描述

	* @param nErrCode 错误码.
	* @param szErrDesc 错误描述.

	* @return true获取成功，false获取失败.
	*/
	virtual bool GetErrDesc(UINT16 nErrCode, WCHAR szErrDesc[256]) = 0;
};

interface ITradeCallBack_HK
{
	/**
	* 下单请求返回

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nLocalID 客户端产生的订单ID，用于与后续推送订单关联.
	* @param nErrCode 错误码.
	*/
	virtual void OnPlaceOrder(Trade_Env enEnv, UINT nCookie, Trade_SvrResult enSvrRet, UINT64 nLocalID, UINT16 nErrCode) = 0;

	/**
	* 订单更新推送

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param orderItem 订单结构体.
	*/
	virtual void OnOrderUpdate(Trade_Env enEnv, const Trade_OrderItem_HK& orderItem) = 0;

	/**
	* 设置订单状态请求返回

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nOrderID 订单号.
	* @param nErrCode 错误码.
	*/
	virtual void OnSetOrderStatus(Trade_Env enEnv, UINT nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, UINT16 nErrCode) = 0;

	/**
	* 改单请求返回

	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nCookie 请求时的Cookie.
	* @param enSvrRet 服务器处理结果.
	* @param nOrderID 订单号.
	* @param nErrCode 错误码.
	*/
	virtual void OnChangeOrder(Trade_Env enEnv, UINT nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, UINT16 nErrCode) = 0;

	/**
	* 订单错误推送
	
	* @param enEnv 交易环境(实盘交易或仿真交易).
	* @param nOrderID 订单号.
	* @param enErrNotify 订单错误类型.
	* @param nErrCode 错误码.
	*/
	virtual void OnOrderErrNotify(Trade_Env enEnv, UINT64 nOrderID, Trade_OrderErrNotify_HK enErrNotify, UINT16 nErrCode) = 0;
};
