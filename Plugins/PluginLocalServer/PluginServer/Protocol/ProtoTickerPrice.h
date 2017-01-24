#pragma once
#include "ProtoDataStruct_Quote.h"
#include "ProtoParseBase.h"


class CProtoTickerPrice: public CProtoParseBase
{
public:
	//tomodify 1 定义协议请求、协议响应结构体类型
	typedef Ticker_Req		ProtoReqDataType;
	typedef Ticker_Ack		ProtoAckDataType;

	//tomodify 2 定义协议请求、协议响应包体类型
	typedef TickerReqBody	ProtoReqBodyType;
	typedef TickerAckBody	ProtoAckBodyType;


public:
	CProtoTickerPrice();
	virtual ~CProtoTickerPrice();

	virtual bool ParseJson_Req(const Json::Value &jsnVal);
	virtual bool ParseJson_Ack(const Json::Value &jsnVal);
	virtual bool MakeJson_Req(Json::Value &jsnVal);
	virtual bool MakeJson_Ack(Json::Value &jsnVal);

	void	SetProtoData_Req(ProtoReqDataType *pData);
	void	SetProtoData_Ack(ProtoAckDataType *pData);
		
private:
	void GetStructField4ParseJson_v0(bool bReqOrAck, int nLevel, const std::string &strStructName, VT_PROTO_FIELD &vtField, void *pStruct);

	virtual void GetStructField4ParseJson(bool bReqOrAck, int nLevel, const std::string &strStructName, VT_PROTO_FIELD &vtField, void *pStruct);
	virtual void GetStructField4MakeJson(bool bReqOrAck, int nLevel, const std::string &strStructName, VT_PROTO_FIELD &vtField, void *pStruct);
	
	virtual void GetArrayField4ParseJson(bool bReqOrAck, int nLevel, const std::string &strArrayName, int nJsnArrSize, VT_PROTO_FIELD &vtField, void *pVector);
	virtual void GetArrayField4MakeJson(bool bReqOrAck, int nLevel, const std::string &strArrayName, VT_PROTO_FIELD &vtField, void *pVector);

private:
	void  GetProtoReqBodyFields(VT_PROTO_FIELD &vtField, void *pStruct);
	void  GetProtoAckBodyFields(VT_PROTO_FIELD &vtField, void *pStruct);

private:
	ProtoReqDataType	*m_pReqData;
	ProtoAckDataType	*m_pAckData;
};