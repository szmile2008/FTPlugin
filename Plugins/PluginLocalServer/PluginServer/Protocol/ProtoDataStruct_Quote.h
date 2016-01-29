#pragma once
#include <vector>
#include "ProtoDataStruct.h"


//////////////////////////////////////////////////////////////////////////
//拉取基本报价协议, PROTO_ID_GET_BASIC_PRICE

struct	BasicPriceReqBody
{
	int nStockMarket;
	std::string strStockCode;
};

struct BasicPriceAckBody
{
	int nHigh;
	int nOpen;
	int nClose;
	int nLastClose;
	int nLow;
	int nCur;
	INT64 nVolume;
	INT64 nTurnover;

	int nStockMarket;
	std::string strStockCode;
	DWORD dwTime;
};

struct	BasicPrice_Req
{
	ProtoHead			head;
	BasicPriceReqBody	body;
};

struct	BasicPrice_Ack
{
	ProtoHead				head;
	BasicPriceAckBody		body;
};


//////////////////////////////////////////////////////////////////////////
//拉取摆盘信息协议, PROTO_ID_GET_GEAR_PRICE

struct	GearPriceReqBody
{
	int nGetGearNum;
	int nStockMarket;	
	std::string strStockCode;	
};

struct GearPriceAckItem
{
	int nBuyOrder;
	int nSellOrder;
	int nBuyPrice;
	int nSellPrice;
	INT64 nBuyVolume;
	INT64 nSellVolume;
};

typedef std::vector<GearPriceAckItem>	VT_GEAR_PRICE;

struct GearPriceAckBody 
{
	int nStockMarket;
	std::string strStockCode;
	VT_GEAR_PRICE vtGear;
};

struct	GearPrice_Req
{
	ProtoHead			head;
	GearPriceReqBody	body;
};

struct	GearPrice_Ack
{
	ProtoHead			head;
	GearPriceAckBody	body;
};