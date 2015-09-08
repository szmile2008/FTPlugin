#pragma once
#include <vector>

//参考"需求.txt"

//协议ID
enum
{
	PROTO_ID_QUOTE_MIN				= 1001,
	PROTO_ID_QT_GET_BASIC_PRICE		= 1001,
	PROTO_ID_QT_GET_GEAR_PRICE		= 1002,	
	PROTO_ID_QUOTE_MAX				= 1999,

	PROTO_ID_TRADE_HK_MIN			= 6001,	
	PROTO_ID_TDHK_PUSH_ORDER_UPDATE	= 6001,
	PROTO_ID_TDHK_PUSH_ORDER_ERROR	= 6002,
	PROTO_ID_TDHK_PLACE_ORDER		= 6003,
	PROTO_ID_TDHK_SET_ORDER_STATUS	= 6004,
	PROTO_ID_TDHK_CHANGE_ORDER		= 6005,
	PROTO_ID_TRADE_HK_MAX			= 6999,
};

#define KEY_REQ_PARAM	"ReqParam"
#define KEY_ACK_DATA	"RetData"

enum ProtoErrCode
{
	PROTO_ERR_NO_ERROR	= 0,

	PROTO_ERR_UNKNOWN_ERROR = 400,
	PROTO_ERR_VER_NOT_SUPPORT = 401,
	PROTO_ERR_STOCK_NOT_FIND = 402,
	PROTO_ERR_COMMAND_NOT_SUPPORT = 403,
	PROTO_ERR_PARAM_ERR = 404,

	PROTO_ERR_SERVER_BUSY	= 501,
	PROTO_ERR_SERVER_TIMEROUT = 502,
};

//////////////////////////////////////////////////////////////////////////
//通用协议头部

struct ProtoHead
{
	int nProtoVer;
	int nProtoID;
	int nErrCode;
	std::string strErrDesc;
};
