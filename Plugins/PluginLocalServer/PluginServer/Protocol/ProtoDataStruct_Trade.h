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
//下港股订单 PROTO_ID_TDHK_PLACE_ORDER
struct	PlaceOrderHKReqBody
{
	int nCookie;
	int nOrderDir;
	int nOrderTypeHK;
	int nPrice;
	INT64 nQty;
	std::string strCode;
};

struct PlaceOrderHKAckBody
{	
	int nCookie;
	INT64 nLocalID;
	int nSvrResult;	
};

struct	PlaceOrderHK_Req
{
	ProtoHead			head;
	PlaceOrderHKReqBody	body;
};

struct	PlaceOrderHK_Ack
{
	ProtoHead				head;
	PlaceOrderHKAckBody		body;
};


//////////////////////////////////////////////////////////////////////////
//设置港股订单状态 PROTO_ID_TDHK_SET_ORDER_STATUS
struct	SetOrderStatusHKReqBody
{
	int nCookie;
	int nSetOrderStatusHK;
	INT64 nOrderID;
};

struct SetOrderStatusHKAckBody
{	
	int nCookie;
	INT64 nOrderID;
	int nSvrResult;	
};

struct	SetOrderStatusHK_Req
{
	ProtoHead				head;
	SetOrderStatusHKReqBody	body;
};

struct	SetOrderStatusHK_Ack
{
	ProtoHead				head;
	SetOrderStatusHKAckBody	body;
};


//////////////////////////////////////////////////////////////////////////
//港股改单 PROTO_ID_TDHK_CHANGE_ORDER
struct	ChangeOrderHKReqBody
{
	int nCookie;
	INT64 nOrderID;
	int nPrice;
	INT64 nQty;
};

struct ChangeOrderHKAckBody
{	
	int nCookie;
	INT64 nOrderID;
	int nSvrResult;	
};

struct	ChangeOrderHK_Req
{
	ProtoHead				head;
	ChangeOrderHKReqBody	body;
};

struct	ChangeOrderHK_Ack
{
	ProtoHead				head;
	ChangeOrderHKAckBody	body;
};