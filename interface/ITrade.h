#pragma once

/************************************************* 
Copyright: FUTU
Author: Lin
Date: 2015-03-18
Description: 交易API和回调接口定义
**************************************************/  

enum Trade_SvrResult
{
	Trade_SvrResult_Succeed = 0, //服务器返回请求处理成功
	Trade_SvrResult_Failed = -1, //服务器返回请求处理失败（失败的原因可能很多，包括网络超时，暂时统一返回失败）
};

enum Trade_OrderSide
{
	Trade_OrderSide_Buy = 1, //买入
	Trade_OrderSide_Sell = 2, //卖出
};

enum Trade_OrderType_HK
{
	Trade_OrderType_HK_EnhancedLimit = 1, //普通交易（增强限价）
	Trade_OrderType_HK_Auction = 2, //竞价交易
	Trade_OrderType_HK_AuctionLimit = 3, //竞价限价
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
	Trade_SetOrderStatus_HK_Cancel = 1, //撤单
	Trade_SetOrderStatus_HK_Disable = 2, //失效
	Trade_SetOrderStatus_HK_Enable = 3, //生效
	Trade_SetOrderStatus_HK_Delete = 4, //删除
};

struct Trade_OrderItem_HK
{
	UINT64 nLocalID; //客户端产生的订单ID，非订单真正的ID，用于关联
	UINT64 nOrderID; //订单号，服务器产生的订单真正的ID

	Trade_OrderSide enSide;
	Trade_OrderType_HK enType;
	Trade_OrderStatus_HK enStatus;
	WCHAR szCode[16];
	WCHAR szName[128];
	UINT64 nPrice;
	UINT64 nQty;
	UINT64 nDealtQty; //成交数量
	
	UINT64 nSubmitedTime; //服务器收到的订单提交时间
	UINT64 nUpdatedTime; //订单最后更新的时间

	UINT16 nErrCode; //错误码
};

interface ITrade_HK
{
	/**
	* 下单

	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param enSide 订单方向，买或卖.
	* @param enType 订单类型.
	* @param lpszCode 股票代码.
	* @param nPrice 订单价格.
	* @param nQty 订单数量.

	* @return true发送成功，false发送失败.
	*/
	virtual bool PlaceOrder(UINT* pCookie, Trade_OrderSide enSide, Trade_OrderType_HK enType, LPCWSTR lpszCode, UINT64 nPrice, UINT64 nQty) = 0;

	/**
	* 设置订单状态

	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param enStatus 设置为何种状态.
	* @param nOrderID 订单真正的ID.

	* @return true发送成功，false发送失败.
	*/
	virtual bool SetOrderStatus(UINT* pCookie, Trade_SetOrderStatus_HK enStatus, UINT64 nOrderID) = 0;

	/**
	* 改单

	* @param pCookie 接收本次调用对应的Cookie值，用于服务器返回时做对应关系判断.
	* @param nOrderID 订单真正的ID.
	* @param nPrice 新的订单价格.
	* @param nQty 新的订单数量.

	* @return true发送成功，false发送失败.
	*/
	virtual bool ChangeOrder(UINT* pCookie, UINT64 nOrderID, UINT64 nPrice, UINT64 nQty) = 0;
};

interface ITradeCallBack_HK
{
	/**
	* 下单请求返回

	* @param nCookie 请求时的Cookie.
	* @param enRet 服务器处理结果.
	* @param nLocalID 客户端产生的订单ID.
	* @param nErrCode 错误码.
	*/
	virtual void OnPlaceOrder(UINT nCookie, Trade_SvrResult enRet, UINT64 nLocalID, UINT16 nErrCode) = 0;

	/**
	* 订单更新推送

	* @param orderItem 订单结构体.
	*/
	virtual void OnOrderUpdated(const Trade_OrderItem_HK& orderItem) = 0;

	/**
	* 设置订单状态请求返回

	* @param nCookie 请求时的Cookie.
	* @param enRet 服务器处理结果.
	* @param nOrderID 订单号.
	* @param nErrCode 错误码.
	*/
	virtual void OnSetOrderStatus(UINT nCookie, Trade_SvrResult enRet, UINT64 nOrderID, UINT16 nErrCode) = 0;

	/**
	* 改单请求返回

	* @param nCookie 请求时的Cookie.
	* @param enRet 服务器处理结果.
	* @param nOrderID 订单号.
	* @param nErrCode 错误码.
	*/
	virtual void OnChangeOrder(UINT nCookie, Trade_SvrResult enRet, UINT64 nOrderID, UINT16 nErrCode) = 0;
};
