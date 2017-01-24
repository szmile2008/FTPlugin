#include "stdafx.h"
#include "ProtoTradeDate.h"
#include "CppJsonConv.h"
#include "../JsonCpp/json_op.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CProtoTradeDate::CProtoTradeDate()
{
	m_pReqData = NULL;
	m_pAckData = NULL;
}

CProtoTradeDate::~CProtoTradeDate()
{

}

bool CProtoTradeDate::ParseJson_Req(const Json::Value &jsnVal)
{
	CHECK_RET(m_pReqData != NULL, false);

	bool bSuc = ParseJsonProtoStruct(jsnVal, true, "", m_pReqData);
	return bSuc;
}

bool CProtoTradeDate::ParseJson_Ack(const Json::Value &jsnVal)
{
	CHECK_RET(m_pAckData != NULL, false);

	bool bSuc = ParseJsonProtoStruct(jsnVal, false, "", m_pAckData);
	return bSuc;
}


bool CProtoTradeDate::MakeJson_Req(Json::Value &jsnVal)
{
	CHECK_RET(m_pReqData != NULL, false);

	bool bSuc = MakeJsonProtoStruct(jsnVal, true, "", m_pReqData);
	return bSuc;
}

bool CProtoTradeDate::MakeJson_Ack(Json::Value &jsnVal)
{
	CHECK_RET(m_pAckData != NULL, false);

	bool bSuc = MakeJsonProtoStruct(jsnVal, false, "", m_pAckData);
	return bSuc;
}

void CProtoTradeDate::SetProtoData_Req(ProtoReqDataType *pData)
{
	m_pReqData = pData;
}

void CProtoTradeDate::SetProtoData_Ack(ProtoAckDataType *pData)
{
	m_pAckData = pData;
}

void CProtoTradeDate::GetStructField4ParseJson_v0(bool bReqOrAck, int nLevel, const std::string &strStructName, VT_PROTO_FIELD &vtField, void *pStruct)
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
	//tomodify 3 -v0 如果协议有其它内嵌结构，在这继续添加	
}

void CProtoTradeDate::GetStructField4ParseJson(bool bReqOrAck, int nLevel, const std::string &strStructName, VT_PROTO_FIELD &vtField, void *pStruct)
{
	GetStructField4ParseJson_v0(bReqOrAck, nLevel, strStructName, vtField, pStruct);
	return ;

	CHECK_RET(nLevel >= 0 && pStruct != NULL && vtField.empty(), NORET);

	if ( nLevel == 0 && bReqOrAck )
	{
		static BOOL arOptional[] = {
			FALSE, FALSE, 
		};
		static EProtoFildType arFieldType[] = {
			ProtoFild_Struct, ProtoFild_Struct,
		};
		static LPCSTR arFieldKey[] = {
			FIELD_KEY_HEAD,	FIELD_KEY_BODY,
		};

		ProtoReqDataType *pReqData = (ProtoReqDataType *)pStruct;		
		void *arPtr[] = {
			&pReqData->head, &pReqData->body,
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
	else if ( nLevel == 0 && !bReqOrAck )
	{
		static BOOL arOptional[] = {
			FALSE, TRUE, 
		};
		static EProtoFildType arFieldType[] = {
			ProtoFild_Struct, ProtoFild_Struct,
		};
		static LPCSTR arFieldKey[] = {
			FIELD_KEY_HEAD,	FIELD_KEY_BODY,
		};

		ProtoReqDataType *pAckData = (ProtoReqDataType *)pStruct;		
		void *arPtr[] = {
			&pAckData->head, &pAckData->body,
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
	else if ( nLevel == 1 && bReqOrAck && strStructName == FIELD_KEY_HEAD )
	{
		ProtoHead *pHead = (ProtoHead *)pStruct;
		CProtoParseBase::GetProtoHeadField_Req(vtField, *pHead);
	}
	else if ( nLevel == 1 && !bReqOrAck && strStructName == FIELD_KEY_HEAD )
	{
		ProtoHead *pHead = (ProtoHead *)pStruct;
		CProtoParseBase::GetProtoHeadField_Ack(vtField, *pHead);
	}	
	else if ( nLevel == 1 && bReqOrAck && strStructName == FIELD_KEY_BODY )
	{
		GetProtoReqBodyFields(vtField, pStruct);
	}	
	else if ( nLevel == 1 && !bReqOrAck && strStructName == FIELD_KEY_BODY )
	{
		GetProtoAckBodyFields(vtField, pStruct);
	}
	//tomodify 3 如果协议有其它内嵌结构，在这继续添加
}

void CProtoTradeDate::GetStructField4MakeJson(bool bReqOrAck, int nLevel, const std::string &strStructName, VT_PROTO_FIELD &vtField, void *pStruct)
{
	GetStructField4ParseJson_v0(bReqOrAck, nLevel, strStructName, vtField, pStruct);
	return ;

	CHECK_RET(nLevel >= 0 && pStruct != NULL && vtField.empty(), NORET);

	if ( nLevel == 0 && bReqOrAck )
	{
		static BOOL arOptional[] = {
			FALSE, FALSE, 
		};
		static EProtoFildType arFieldType[] = {
			ProtoFild_Struct, ProtoFild_Struct,
		};
		static LPCSTR arFieldKey[] = {
			FIELD_KEY_HEAD,	FIELD_KEY_BODY,
		};

		ProtoReqDataType *pReqData = (ProtoReqDataType *)pStruct;		
		void *arPtr[] = {
			&pReqData->head, &pReqData->body,
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
	else if ( nLevel == 0 && !bReqOrAck )
	{
		static BOOL arOptional[] = {
			FALSE, TRUE, 
		};
		static EProtoFildType arFieldType[] = {
			ProtoFild_Struct, ProtoFild_Struct,
		};
		static LPCSTR arFieldKey[] = {
			FIELD_KEY_HEAD,	FIELD_KEY_BODY,
		};

		ProtoReqDataType *pAckData = (ProtoReqDataType *)pStruct;		
		void *arPtr[] = {
			&pAckData->head, &pAckData->body,
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
	else if ( nLevel == 1 && bReqOrAck && strStructName == FIELD_KEY_HEAD )
	{
		ProtoHead *pHead = (ProtoHead *)pStruct;
		CProtoParseBase::GetProtoHeadField_Req(vtField, *pHead);
	}
	else if ( nLevel == 1 && !bReqOrAck && strStructName == FIELD_KEY_HEAD )
	{
		ProtoHead *pHead = (ProtoHead *)pStruct;
		CProtoParseBase::GetProtoHeadField_Ack(vtField, *pHead);
	}
	else if ( nLevel == 1 && bReqOrAck && strStructName == FIELD_KEY_BODY )
	{
		GetProtoReqBodyFields(vtField, pStruct);
	}	
	else if ( nLevel == 1 && !bReqOrAck && strStructName == FIELD_KEY_BODY )
	{
		GetProtoAckBodyFields(vtField, pStruct);
	}
	//tomodify 4 如果协议有其它内嵌结构，在这继续添加
}

//tomodify 5 如果协议有其它内嵌数组，在这更改实现
void CProtoTradeDate::GetArrayField4ParseJson(bool bReqOrAck, int nLevel, const std::string &strArrayName, int nJsnArrSize, VT_PROTO_FIELD &vtField, void *pVector)
{
	CHECK_RET(nLevel >= 2 && nJsnArrSize >= 0 && pVector != NULL, NORET);

	if ( !bReqOrAck && nLevel == 2 && strArrayName == "TradeDateArr" )
	{
		vtField.clear();

		VT_TRADE_DATE &vtDateList = *(VT_TRADE_DATE*)pVector;
		vtDateList.resize(nJsnArrSize);
		for ( int n = 0; n < nJsnArrSize; n++ )
		{
			PROTO_FIELD field;
			CProtoParseBase::FillFieldMembers(field, FALSE, ProtoFild_StringA, strArrayName, &vtDateList[n]);
			vtField.push_back(field);
		}
	}
}

//tomodify 6 如果协议有其它内嵌数组，在这更改实现
void CProtoTradeDate::GetArrayField4MakeJson(bool bReqOrAck, int nLevel, const std::string &strArrayName, VT_PROTO_FIELD &vtField, void *pVector)
{
	CHECK_RET(nLevel >= 2 && pVector != NULL, NORET);

	if ( !bReqOrAck && nLevel == 2 && strArrayName == "TradeDateArr" )
	{
		vtField.clear();

		VT_TRADE_DATE &vtDateList = *(VT_TRADE_DATE*)pVector;		
		for ( int n = 0; n < (int)vtDateList.size(); n++ )
		{
			PROTO_FIELD field;
			CProtoParseBase::FillFieldMembers(field, FALSE, ProtoFild_StringA, strArrayName, &vtDateList[n]);
			vtField.push_back(field);
		}
	}	
}

//tomodify 7 得到req body的所有字段
void  CProtoTradeDate::GetProtoReqBodyFields(VT_PROTO_FIELD &vtField, void *pStruct)
{
	ProtoReqBodyType *pBody = (ProtoReqBodyType *)pStruct;
	CHECK_RET(pBody != NULL, NORET);

	static BOOL arOptional[] = {
		FALSE, FALSE, FALSE, 
	};
	static EProtoFildType arFieldType[] = {
		ProtoFild_Int32, ProtoFild_StringA, ProtoFild_StringA,
	};
	static LPCSTR arFieldKey[] = {
		"Market", "start_date", "end_date"
	};
	
	void *arPtr[] = {
		&pBody->nStockMarket, &pBody->strStartDate, &pBody->strEndDate
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
void  CProtoTradeDate::GetProtoAckBodyFields(VT_PROTO_FIELD &vtField, void *pStruct)
{
	ProtoAckBodyType *pBody = (ProtoAckBodyType*)pStruct;
	CHECK_RET(pBody != NULL, NORET);

	static BOOL arOptional[] = {
		FALSE, FALSE, 
		FALSE, FALSE, 
	};
	static EProtoFildType arFieldType[] = {
		ProtoFild_Int32, ProtoFild_Vector,
		ProtoFild_StringA, ProtoFild_StringA
	};
	static LPCSTR arFieldKey[] = {
		"Market",	  "TradeDateArr",
		"start_date", "end_date" 
	};
	
	void *arPtr[] = {
		&pBody->nStockMarket, &pBody->vtTradeDate,
		&pBody->strStartDate, &pBody->strEndDate
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