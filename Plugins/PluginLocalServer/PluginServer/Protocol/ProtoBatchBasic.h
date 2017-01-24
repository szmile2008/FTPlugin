#pragma once
#include "ProtoDataStruct_Quote.h"
#include "ProtoParseBase.h"


class CProtoBatchBasic : public CProtoParseBase
{
public:
	//tomodify 1
	typedef BatchBasic_Req		ProtoReqDataType;
	typedef BatchBasic_Ack		ProtoAckDataType;

	//tomodify 2
	typedef BatchBasicReqBody	ProtoReqBodyType;
	typedef BatchBasicAckBody	ProtoAckBodyType;


public:
	CProtoBatchBasic();
	virtual ~CProtoBatchBasic();

	virtual bool ParseJson_Req(const Json::Value &jsnVal);
	virtual bool ParseJson_Ack(const Json::Value &jsnVal);
	virtual bool MakeJson_Req(Json::Value &jsnVal);
	virtual bool MakeJson_Ack(Json::Value &jsnVal);

	void	SetProtoData_Req(ProtoReqDataType *pData);
	void	SetProtoData_Ack(ProtoAckDataType *pData);

private:	
	bool ParseProtoBody_Req(const Json::Value &jsnVal, ProtoReqDataType &data);
	bool ParseProtoBody_Ack(const Json::Value &jsnVal, ProtoAckDataType &data);
	bool MakeProtoBody_Req(Json::Value &jsnVal, const ProtoReqDataType &data); 
	bool MakeProtoBody_Ack(Json::Value &jsnVal, const ProtoAckDataType &data);

	void GetProtoBodyField_Req(VT_PROTO_FIELD &vtField, const ProtoReqBodyType &reqData);
	void GetProtoBodyField_Ack(VT_PROTO_FIELD &vtField, const ProtoAckBodyType &ackData);

private:

	bool ParseAckBatchBasicArr(const Json::Value &jsnRetData, ProtoAckBodyType &data);
	bool MakeAckBatchBasicArr(Json::Value &jsnRetData, const ProtoAckBodyType &data);
	void GetAckBatchBasicArrField(VT_PROTO_FIELD &vtField, const BatchBasicAckItem &ackItem);

	bool ParseReqBatchBasicArr(const Json::Value &jsnRetData, ProtoReqBodyType &data);
	bool MakeReqBatchBasicArr(Json::Value &jsnRetData, const ProtoReqBodyType &data);
	void GetReqBatchBasicArrField(VT_PROTO_FIELD &vtField, const BatchBasicReqItem &ackItem);

private:
	ProtoReqDataType	*m_pReqData;
	ProtoAckDataType	*m_pAckData;
};