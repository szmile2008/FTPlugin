#pragma once
#include <vector>
#include "ProtoDataStruct.h"

//////////////////////////////////////////////////////////////////////////
//推送港股订单实时信息, PROTO_ID_TDHK_PUSH_ORDER_UPDATE
struct	OrderUpdatePushHKReqBody
{
};

struct OrderUpdatePushHKAckBody
{
	int nEnvType;
	INT64 nLocalID;
	INT64 nOrderID;
	int   nOrderDir;
	int	  nOrderTypeHK;
	int   nOrderStatusHK;	
	int   nPrice;
	INT64 nQTY;
	INT64 nDealQTY;
	int   nSubmitTime;
	int   nUpdateTime;
	std::string strStockName;
	std::string strStockCode;
};

struct	OrderUpdatePushHK_Req
{
	ProtoHead					head;
	OrderUpdatePushHKReqBody	body;
};

struct	OrderUpdatePushHK_Ack
{
	ProtoHead					head;
	OrderUpdatePushHKAckBody	body;
};



//////////////////////////////////////////////////////////////////////////
//推送港股订单错误信息, PROTO_ID_TDHK_PUSH_ORDER_ERROR
struct	OrderErrorPushHKReqBody
{
};

struct OrderErrorPushHKAckBody
{	
	int nEnvType;
	INT64 nOrderID;
	int   nOrderErrNotifyHK;
	int	  nOrderErrCode;
	std::string  strOrderErrDesc;
};

struct	OrderErrorPushHK_Req
{
	ProtoHead				head;
	OrderErrorPushHKReqBody	body;
};

struct	OrderErrorPushHK_Ack
{
	ProtoHead				head;
	OrderErrorPushHKAckBody	body;
};


//////////////////////////////////////////////////////////////////////////
//下订单 PROTO_ID_TDHK_PLACE_ORDER 
struct	PlaceOrderReqBody
{
	int nEnvType;
	int nCookie;
	int nOrderDir;
	int nOrderType;
	int nPrice;
	INT64 nQty;
	std::string strCode;
};

struct PlaceOrderAckBody
{	
	int nEnvType;
	int nCookie;
	INT64 nLocalID;
	int nSvrResult;	
};

struct	PlaceOrder_Req
{
	ProtoHead			head;
	PlaceOrderReqBody	body;
};

struct	PlaceOrder_Ack
{
	ProtoHead				head;
	PlaceOrderAckBody		body;
};


//////////////////////////////////////////////////////////////////////////
//设置订单状态 PROTO_ID_TDHK_SET_ORDER_STATUS
struct	SetOrderStatusReqBody
{
	int		nEnvType;
	int		nCookie;
	int		nSetOrderStatus;
	INT64	nSvrOrderID;
	INT64	nLocalOrderID;

};

struct SetOrderStatusAckBody
{	
	int		nEnvType;
	int		nCookie;
	INT64	nSvrOrderID;
	INT64	nLocalOrderID;
	int		nSvrResult;	
};

struct	SetOrderStatus_Req
{
	ProtoHead				head;
	SetOrderStatusReqBody	body;
};

struct	SetOrderStatus_Ack
{
	ProtoHead				head;
	SetOrderStatusAckBody	body;
};


//////////////////////////////////////////////////////////////////////////
//解锁交易
struct UnlockTradeReqBody
{
	int			nCookie;
	std::string strPasswd;
};

struct UnlockTradeAckBody
{
	int	nCookie;
	int nSvrResult;	
};

struct UnlockTrade_Req
{
	ProtoHead				head;
	UnlockTradeReqBody		body;
};

struct UnlockTrade_Ack
{
	ProtoHead				head;
	UnlockTradeAckBody		body;
};


//////////////////////////////////////////////////////////////////////////
//港股改单 PROTO_ID_TDHK_CHANGE_ORDER
struct	ChangeOrderReqBody
{
	int		nEnvType;
	int		nCookie;
	INT64	nSvrOrderID;
	INT64	nLocalOrderID;
	int		nPrice;
	INT64	nQty;
};

struct ChangeOrderAckBody
{	
	int		nEnvType;
	int		nCookie;
	INT64	nSvrOrderID;
	INT64	nLocalOrderID;
	int		nSvrResult;	
};

struct	ChangeOrder_Req
{
	ProtoHead			head;
	ChangeOrderReqBody	body;
};

struct	ChangeOrder_Ack
{
	ProtoHead				head;
	ChangeOrderAckBody	body;
};

//////////////////////////////////////////////////////////////////////////
//获取用户港股帐户信息
struct	QueryHKAccInfoReqBody
{
	int		nEnvType;
	int		nCookie;	
};

struct QueryHKAccInfoAckBody
{	
	int		nEnvType;
	int		nCookie;
	
	//以下与 Trade_AccInfo 同步
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

struct	QueryHKAccInfo_Req
{
	ProtoHead			head;
	QueryHKAccInfoReqBody	body;
};

struct	QueryHKAccInfo_Ack
{
	ProtoHead			head;
	QueryHKAccInfoAckBody	body;
};



//////////////////////////////////////////////////////////////////////////
//获取用户美股帐户信息
struct	QueryUSAccInfoReqBody
{
	int		nEnvType;
	int		nCookie;	
};

struct QueryUSAccInfoAckBody
{	
	int		nEnvType;
	int		nCookie;

	//以下与 Trade_AccInfo 同步
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

struct	QueryUSAccInfo_Req
{
	ProtoHead			head;
	QueryUSAccInfoReqBody	body;
};

struct	QueryUSAccInfo_Ack
{
	ProtoHead			head;
	QueryUSAccInfoAckBody	body;
};


//////////////////////////////////////////////////////////////////////////
//查询所有港股订单
struct	QueryHKOrderReqBody
{
	int		nEnvType;
	int		nCookie;
};

//与 Trade_OrderItem 同步
struct QueryHKOrderAckItem
{
	INT64 nLocalID; //客户端产生的订单ID，非订单真正的ID，用于关联
	INT64 nOrderID; //订单号，服务器产生的订单真正的ID

	int nOrderType; //不同市场、取值对应具体的枚举定义 Trade_OrderType_HK 或 Trade_OrderType_US
	int/*Trade_OrderSide*/ enSide;
	int nStatus; //取值对应具体的枚举定义Trade_OrderStatus
	std::wstring strStockCode;
	std::wstring strStockName;	
	INT64 nPrice;
	INT64 nQty;
	INT64 nDealtQty; //成交数量
	int nDealtAvgPrice; //成交均价，没有放大

	INT64 nSubmitedTime; //服务器收到的订单提交时间
	INT64 nUpdatedTime; //订单最后更新的时间

	int   nErrCode; //错误码，仅支持港股
};

typedef std::vector<QueryHKOrderAckItem>	VT_HK_ORDER;

struct QueryHKOrderAckBody
{	
	int		nEnvType;
	int		nCookie;
	VT_HK_ORDER vtOrder;
};

struct	QueryHKOrder_Req
{
	ProtoHead			head;
	QueryHKOrderReqBody	body;
};

struct	QueryHKOrder_Ack
{
	ProtoHead			head;
	QueryHKOrderAckBody	body;
};




//////////////////////////////////////////////////////////////////////////
//查询所有美股订单
struct	QueryUSOrderReqBody
{
	int		nEnvType;
	int		nCookie;
};

//与 Trade_OrderItem 同步_
struct QueryUSOrderAckItem
{
	INT64 nLocalID; //客户端产生的订单ID，非订单真正的ID，用于关联
	INT64 nOrderID; //订单号，服务器产生的订单真正的ID

	int nOrderType; //不同市场、取值对应具体的枚举定义 Trade_OrderType_US 或 Trade_OrderType_US
	int/*Trade_OrderSide*/ enSide;
	int nStatus; //取值对应具体的枚举定义Trade_OrderStatus
	std::wstring strStockCode;
	std::wstring strStockName;	
	INT64 nPrice;
	INT64 nQty;
	INT64 nDealtQty; //成交数量
	int   nDealtAvgPrice; //成交均价，没有放大

	INT64 nSubmitedTime; //服务器收到的订单提交时间
	INT64 nUpdatedTime; //订单最后更新的时间

	int   nErrCode; //错误码，仅支持美股
};

typedef std::vector<QueryUSOrderAckItem>	VT_US_ORDER;

struct QueryUSOrderAckBody
{	
	int		nEnvType;
	int		nCookie;
	VT_US_ORDER vtOrder;
};

struct	QueryUSOrder_Req
{
	ProtoHead			head;
	QueryUSOrderReqBody	body;
};

struct	QueryUSOrder_Ack
{
	ProtoHead			head;
	QueryUSOrderAckBody	body;
};


//////////////////////////////////////////////////////////////////////////
//查询订单列表
struct	QueryPositionReqBody
{
	int		nEnvType;
	int		nCookie;
};

//与 Trade_PositionItem 同步_
struct QueryPositionAckItem
{
	std::wstring strStockCode;
	std::wstring strStockName;	

	INT64 nQty; //持有数量
	INT64 nCanSellQty; //可卖数量
	INT64 nNominalPrice; //市价
	INT64 nMarketVal; //市值

	int  nCostPrice; //成本价
	int  nCostPriceValid; //成本价是否有效
	INT64 nPLVal; //盈亏金额
	int  nPLValValid; //盈亏金额是否有效
	int nPLRatio; //盈亏比例
	int nPLRatioValid; //盈亏比例是否有效

	INT64 nToday_PLVal; //今日盈亏金额
	INT64 nToday_BuyQty; //今日买入成交量
	INT64 nToday_BuyVal; //今日买入成交额
	INT64 nToday_SellQty; //今日卖出成交量
	INT64 nToday_SellVal; //今日卖出成交额
};

typedef std::vector<QueryPositionAckItem>	VT_Position;

struct QueryPositionAckBody
{	
	int		nEnvType;
	int		nCookie;
	VT_Position  vtPosition;
};

struct	QueryPosition_Req
{
	ProtoHead				head;
	QueryPositionReqBody	body;
};

struct	QueryPosition_Ack
{
	ProtoHead				head;
	QueryPositionAckBody	body;
};
