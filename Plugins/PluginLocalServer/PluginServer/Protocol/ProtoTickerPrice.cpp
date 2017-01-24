#include "stdafx.h"
#include "ProtoTickerPrice.h"
#include "CppJsonConv.h"
#include "../JsonCpp/json_op.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CProtoTickerPrice::CProtoTickerPrice()
{
	m_pReqData = NULL;
	m_pAckData = NULL;
}

CProtoTickerPrice::~CProtoTickerPrice()
{

}

bool CProtoTickerPrice::ParseJson_Req(const Json::Value &jsnVal)
{
	CHECK_RET(m_pReqData != NULL, false);

	bool bSuc = ParseJsonProtoStruct(jsnVal, true, "", m_pReqData);
	return bSuc;
}

bool CProtoTickerPrice::ParseJson_Ack(const Json::Value &jsnVal)
{
	CHECK_RET(m_pAckData != NULL, false);

	bool bSuc = ParseJsonProtoStruct(jsnVal, false, "", m_pAckData);
	return bSuc;
}


bool CProtoTickerPrice::MakeJson_Req(Json::Value &jsnVal)
{
	CHECK_RET(m_pReqData != NULL, false);

	bool bSuc = MakeJsonProtoStruct(jsnVal, true, "", m_pReqData);
	return bSuc;
}

bool CProtoTickerPrice::MakeJson_Ack(Json::Value &jsnVal)
{
	CHECK_RET(m_pAckData != NULL, false);

	bool bSuc = MakeJsonProtoStruct(jsnVal, false, "", m_pAckData);
	return bSuc;
}

void CProtoTickerPrice::SetProtoData_Req(ProtoReqDataType *pData)
{
	m_pReqData = pData;
}

void CProtoTickerPrice::SetProtoData_Ack(ProtoAckDataType *pData)
{
	m_pAckData = pData;
}

void CProtoTickerPrice::GetStructField4ParseJson_v0(bool bReqOrAck, int nLevel, const std::string &strStructName, VT_PROTO_FIELD &vtField, void *pStruct)
{
	CHECK_RET(nLevel >= 0 && pStruct != NULL && vtField.empty(), NORET);

	if ( nLevel == 0 && bReqOrAck )
	{
		ProtoReqDataType *pReqData = (ProtoReqDataType *)pStruct;		
		CProtoParseBase::GetProtoHeadField_Req(vtField, pReqData->head);

		PROTO_FIELD field;
		CProtoParseBase::FillFieldMembers(field, FALSE, ProtoFild_Struct, KEY_REQ_PARAM, &pReqData->body);
		vtField.push_back(field);
	}
	else if ( nLevel == 0 && !bReqOrAck )
	{
		ProtoReqDataType *pAckData = (ProtoReqDataType *)pStruct;		
		CProtoParseBase::GetProtoHeadField_Ack(vtField, pAckData->head);

		PROTO_FIELD field;
		CProtoParseBase::FillFieldMembers(field, FALSE, ProtoFild_Struct, KEY_ACK_DATA, &pAckData->body);
		vtField.push_back(field);
	}
	else if ( nLevel == 1 && bReqOrAck && strStructName == KEY_REQ_PARAM )
	{
		GetProtoReqBodyFields(vtField, pStruct);
	}	
	else if ( nLevel == 1 && !bReqOrAck && strStructName == KEY_ACK_DATA )
	{
		GetProtoAckBodyFields(vtField, pStruct);
	}
	else if ( nLevel == 3 && !bReqOrAck && strStructName == "TickerArr" )
	{
		TickerAckItem *pTicker = (TickerAckItem*)pStruct;
		CHECK_RET(pTicker != NULL, NORET);

		static BOOL arOptional[] = {
			FALSE, FALSE, 
			FALSE, FALSE, 
			FALSE, FALSE,
		};
		static EProtoFildType arFieldType[] = {
			ProtoFild_StringA, ProtoFild_Int32,
			ProtoFild_Int32, ProtoFild_Int64, 
			ProtoFild_Int64, ProtoFild_Int64,
		};
		static LPCSTR arFieldKey[] = {
			"Time",		"Price",
			"Direction","Sequence",
			"Volume",	"Turnover",
		};

		void *arPtr[] = {
			&pTicker->strTickTime,  &pTicker->nPrice,
			&pTicker->nDealType,	&pTicker->nSequence, 
			&pTicker->nVolume,		&pTicker->nTurnover,		
		};

		CHECK_OP(_countof(arOptional) == _countof(arFieldType), NOOP);
		CHECK_OP(_countof(arOptional) == _countof(arFieldKey), NOOP);
		CHECK_OP(_countof(arOptional) == _countof(arPtr), NOOP);

		PROTO_FIELD field;
		for ( int n = 0; n < _countof(arOptional); n++ )
		{
			bool bFill = CProtoParseBase::FillFieldMembers(field, arOptional[n], arFieldType[n], arFieldKey[n], arPtr[n]);
			CHECK_OP(bFill, continue);
			vtField.push_back(field);
		}
	}
	//tomodify 3 -v0 如果协议有其它内嵌结构，在这继续添加	
}

void CProtoTickerPrice::GetStructField4ParseJson(bool bReqOrAck, int nLevel, const std::string &strStructName, VT_PROTO_FIELD &vtField, void *pStruct)
{
	GetStructField4ParseJson_v0(bReqOrAck, nLevel, strStructName, vtField, pStruct);
	return ;
}

void CProtoTickerPrice::GetStructField4MakeJson(bool bReqOrAck, int nLevel, const std::string &strStructName, VT_PROTO_FIELD &vtField, void *pStruct)
{
	GetStructField4ParseJson_v0(bReqOrAck, nLevel, strStructName, vtField, pStruct);
	return ;	
}

//tomodify 5 如果协议有其它内嵌数组，在这更改实现
void CProtoTickerPrice::GetArrayField4ParseJson(bool bReqOrAck, int nLevel, const std::string &strArrayName, int nJsnArrSize, VT_PROTO_FIELD &vtField, void *pVector)
{
	CHECK_RET(nLevel >= 2 && nJsnArrSize >= 0 && pVector != NULL, NORET);

	if ( !bReqOrAck && nLevel == 2 && strArrayName == "TickerArr" )
	{
		vtField.clear();

		VT_TICKER_DATA &vtTickData = *(VT_TICKER_DATA*)pVector;
		vtTickData.resize(nJsnArrSize);
		for ( int n = 0; n < nJsnArrSize; n++ )
		{
			PROTO_FIELD field;
			CProtoParseBase::FillFieldMembers(field, FALSE, ProtoFild_Struct, strArrayName, &vtTickData[n]);
			vtField.push_back(field);
		}
	}
}

//tomodify 6 如果协议有其它内嵌数组，在这更改实现
void CProtoTickerPrice::GetArrayField4MakeJson(bool bReqOrAck, int nLevel, const std::string &strArrayName, VT_PROTO_FIELD &vtField, void *pVector)
{
	CHECK_RET(nLevel >= 2 && pVector != NULL, NORET);

	if ( !bReqOrAck && nLevel == 2 && strArrayName == "TickerArr" )
	{
		vtField.clear();

		VT_TICKER_DATA &vtTickData = *(VT_TICKER_DATA*)pVector;		
		for ( int n = 0; n < (int)vtTickData.size(); n++ )
		{
			PROTO_FIELD field;
			CProtoParseBase::FillFieldMembers(field, FALSE, ProtoFild_Struct, strArrayName, &vtTickData[n]);
			vtField.push_back(field);
		}
	}	
}

//tomodify 7 得到req body的所有字段
void  CProtoTickerPrice::GetProtoReqBodyFields(VT_PROTO_FIELD &vtField, void *pStruct)
{
	ProtoReqBodyType *pBody = (ProtoReqBodyType *)pStruct;
	CHECK_RET(pBody != NULL, NORET);

	static BOOL arOptional[] = {
		FALSE, FALSE, 
		FALSE, TRUE, 
	};
	static EProtoFildType arFieldType[] = {
		ProtoFild_Int32, ProtoFild_StringA,
		ProtoFild_Int32, ProtoFild_Int64,
	};
	static LPCSTR arFieldKey[] = {
		"Market", "StockCode",  
		"Num",  "Sequence",
	};
	
	void *arPtr[] = {
		&pBody->nStockMarket, &pBody->strStockCode,
		&pBody->nGetTickNum, &pBody->nSequence,
	};

	CHECK_OP(_countof(arOptional) == _countof(arFieldType), NOOP);
	CHECK_OP(_countof(arOptional) == _countof(arFieldKey), NOOP);
	CHECK_OP(_countof(arOptional) == _countof(arPtr), NOOP);

	PROTO_FIELD field;
	for ( int n = 0; n < _countof(arOptional); n++ )
	{
		bool bFill = CProtoParseBase::FillFieldMembers(field, arOptional[n], arFieldType[n], arFieldKey[n], arPtr[n]);
		CHECK_OP(bFill, continue);
		vtField.push_back(field);
	}
}

//tomodify 8 得到ack body的所有字段
void  CProtoTickerPrice::GetProtoAckBodyFields(VT_PROTO_FIELD &vtField, void *pStruct)
{
	ProtoAckBodyType *pBody = (ProtoAckBodyType*)pStruct;
	CHECK_RET(pBody != NULL, NORET);

	static BOOL arOptional[] = {
		FALSE, FALSE, 
		FALSE, TRUE, 
	};
	static EProtoFildType arFieldType[] = {
		ProtoFild_Int32,  ProtoFild_StringA,
		ProtoFild_Vector, ProtoFild_Int64,
	};
	static LPCSTR arFieldKey[] = {
		"Market", "StockCode",
		"TickerArr", "NextSequence"
	};
	
	void *arPtr[] = {
		&pBody->nStockMarket, &pBody->strStockCode, 
		&pBody->vtTicker, &pBody->nNextSequence,
	};

	CHECK_OP(_countof(arOptional) == _countof(arFieldType), NOOP);
	CHECK_OP(_countof(arOptional) == _countof(arFieldKey), NOOP);
	CHECK_OP(_countof(arOptional) == _countof(arPtr), NOOP);

	PROTO_FIELD field;
	for ( int n = 0; n < _countof(arOptional); n++ )
	{
		bool bFill = CProtoParseBase::FillFieldMembers(field, arOptional[n], arFieldType[n], arFieldKey[n], arPtr[n]);
		CHECK_OP(bFill, continue);
		vtField.push_back(field);
	}
}