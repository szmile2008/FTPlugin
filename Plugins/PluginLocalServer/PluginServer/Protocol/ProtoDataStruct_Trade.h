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
//获取用户帐户信息
struct	QueryAccInfoReqBody
{
	int		nEnvType;
	int		nCookie;	
};

struct QueryAccInfoAckBody
{	
	int		nEnvType;
	int		nCookie;
	
	//以下与Trade_AccInfo同步
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

struct	QueryAccInfo_Req
{
	ProtoHead			head;
	QueryAccInfoReqBody	body;
};

struct	QueryAccInfo_Ack
{
	ProtoHead			head;
	QueryAccInfoAckBody	body;
};