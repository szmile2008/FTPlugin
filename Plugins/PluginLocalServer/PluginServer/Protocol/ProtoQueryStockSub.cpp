#include "stdafx.h"
#include "ProtoQueryStockSub.h"
#include "CppJsonConv.h"
#include "../JsonCpp/json_op.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CProtoQueryStockSub::CProtoQueryStockSub()
{
	m_pReqData = NULL;
	m_pAckData = NULL;
}

CProtoQueryStockSub::~CProtoQueryStockSub()
{

}

bool CProtoQueryStockSub::ParseJson_Req(const Json::Value &jsnVal)
{
	CHECK_RET(m_pReqData != NULL, false);

	bool bSuc = true;
	do 
	{
		bSuc &= ParseProtoHead_Req(jsnVal, m_pReqData->head);
		CHECK_OP(bSuc, break);
		bSuc &= ParseProtoBody_Req(jsnVal, *m_pReqData);
		CHECK_OP(bSuc, break);
	} while (0);

	return bSuc;
}

bool CProtoQueryStockSub::ParseJson_Ack(const Json::Value &jsnVal)
{
	CHECK_RET(m_pAckData != NULL, false);

	bool bSuc = true;
	do 
	{
		bSuc &= ParseProtoHead_Ack(jsnVal, m_pAckData->head);
		CHECK_OP(bSuc, break);

		if ( m_pAckData->head.ddwErrCode == PROTO_ERR_NO_ERROR )
		{
			bSuc &= ParseProtoBody_Ack(jsnVal, *m_pAckData);
			CHECK_OP(bSuc, break);
		}
	} while (0);

	return bSuc;
}


bool CProtoQueryStockSub::MakeJson_Req(Json::Value &jsnVal)
{
	CHECK_RET(m_pReqData != NULL, false);

	bool bSuc = true;
	do 
	{
		bSuc &= MakeProtoHead_Req(jsnVal, m_pReqData->head);
		CHECK_OP(bSuc, break);
		bSuc &= MakeProtoBody_Req(jsnVal, *m_pReqData);
		CHECK_OP(bSuc, break);
	} while (0);

	return bSuc;
}

bool CProtoQueryStockSub::MakeJson_Ack(Json::Value &jsnVal)
{
	CHECK_RET(m_pAckData != NULL, false);

	bool bSuc = true;
	do 
	{
		bSuc &= MakeProtoHead_Ack(jsnVal, m_pAckData->head);
		CHECK_OP(bSuc, break);

		if ( m_pAckData->head.ddwErrCode == PROTO_ERR_NO_ERROR )
		{
			bSuc &= MakeProtoBody_Ack(jsnVal, *m_pAckData);
			CHECK_OP(bSuc, break);
		}
	} while (0);

	return bSuc;
}

void CProtoQueryStockSub::SetProtoData_Req(ProtoReqDataType *pData)
{
	m_pReqData = pData;
}

void CProtoQueryStockSub::SetProtoData_Ack(ProtoAckDataType *pData)
{
	m_pAckData = pData;
}

//tomodify 3(数组等复杂结构或单层的结构体)
bool CProtoQueryStockSub::ParseProtoBody_Req(const Json::Value &jsnVal, ProtoReqDataType &data)
{	
	if ( !warn_if_prop_not_set(jsnVal, KEY_REQ_PARAM) )
		return true;

	VT_PROTO_FIELD vtField;
	GetProtoBodyField_Req(vtField, data.body);

	const Json::Value &jsnBody = jsnVal[KEY_REQ_PARAM];
	bool bSuc = CProtoParseBase::ParseProtoFields(jsnBody, vtField);
	CHECK_OP(bSuc, NOOP);

	return true;
}

//tomodify 4(数组等复杂结构或单层的结构体)
bool CProtoQueryStockSub::ParseProtoBody_Ack(const Json::Value &jsnVal, ProtoAckDataType &data)
{
	CHECK_RET(warn_if_prop_not_set(jsnVal, KEY_ACK_DATA), false);	

	VT_PROTO_FIELD vtField;
	GetProtoBodyField_Ack(vtField, data.body);

	const Json::Value &jsnBody = jsnVal[KEY_ACK_DATA];
	bool bSuc = CProtoParseBase::ParseProtoFields(jsnBody, vtField);
	CHECK_OP(bSuc, NOOP);
	
	if ( bSuc )
	{
		bSuc &= ParseSubInfoArr(jsnBody, data.body);
	}

	return bSuc;
}

//tomodify 5(数组等复杂结构或单层的结构体)
bool CProtoQueryStockSub::MakeProtoBody_Req(Json::Value &jsnVal, const ProtoReqDataType &data)
{
	CHECK_RET(warn_if_prop_exists(jsnVal, KEY_REQ_PARAM), false);

	VT_PROTO_FIELD vtField;
	GetProtoBodyField_Req(vtField, data.body);

	Json::Value &jsnBody = jsnVal[KEY_REQ_PARAM];
	bool bSuc = CProtoParseBase::MakeProtoFields(jsnBody, vtField);
	CHECK_OP(bSuc, NOOP);

	return true;
}

//tomodify 6(数组等复杂结构或单层的结构体)
bool CProtoQueryStockSub::MakeProtoBody_Ack(Json::Value &jsnVal, const ProtoAckDataType &data)
{
	CHECK_RET(warn_if_prop_exists(jsnVal, KEY_ACK_DATA), false);	

	VT_PROTO_FIELD vtField;
	GetProtoBodyField_Ack(vtField, data.body);

	Json::Value &jsnBody = jsnVal[KEY_ACK_DATA];
	bool bSuc = CProtoParseBase::MakeProtoFields(jsnBody, vtField);
	CHECK_OP(bSuc, NOOP);
	
	if ( bSuc )
	{
		bSuc &= MakeSubInfoArr(jsnBody, data.body);
	}

	return bSuc;
}

//tomodify 7
void CProtoQueryStockSub::GetProtoBodyField_Req(VT_PROTO_FIELD &vtField, const ProtoReqBodyType &reqData)
{

}

//tomodify 8
void CProtoQueryStockSub::GetProtoBodyField_Ack(VT_PROTO_FIELD &vtField, const ProtoAckBodyType &ackData)
{

}
bool CProtoQueryStockSub::ParseSubInfoArr(const Json::Value &jsnRetData, ProtoAckBodyType &data)
{
	CHECK_RET(warn_if_prop_not_set(jsnRetData, "SubInfoArr"), false);	

	const Json::Value &jsnSubInfoArr = jsnRetData["SubInfoArr"];
	CHECK_RET(jsnSubInfoArr.isArray(), false);

	bool bSuc = true;
	int nArrSize = jsnSubInfoArr.size();
	for ( int n = 0; n < nArrSize; n++ )
	{		
		SubInfoAckItem item;
		VT_PROTO_FIELD vtField;
		GetSubInfoArrField(vtField, item);

		const Json::Value jsnItem = jsnSubInfoArr[n];
		CHECK_OP(!jsnItem.isNull(), continue);
		bSuc = CProtoParseBase::ParseProtoFields(jsnItem, vtField);
		CHECK_OP(bSuc, break);
		data.vtSubInfo.push_back(item);
	}

	return bSuc;
}

bool CProtoQueryStockSub::MakeSubInfoArr(Json::Value &jsnRetData, const ProtoAckBodyType &data)
{
	CHECK_RET(warn_if_prop_exists(jsnRetData, "SubInfoArr"), false);	

	Json::Value &jsnSubInfoArr = jsnRetData["SubInfoArr"];	

	bool bSuc = true;
	for ( int n = 0; n < (int)data.vtSubInfo.size(); n++ )
	{
		const SubInfoAckItem &item = data.vtSubInfo[n];
		VT_PROTO_FIELD vtField;
		GetSubInfoArrField(vtField, item);

		Json::Value &jsnItem = jsnSubInfoArr[n];
		bSuc = CProtoParseBase::MakeProtoFields(jsnItem, vtField);
		CHECK_OP(bSuc, break);
	}

	return bSuc;
}

void CProtoQueryStockSub::GetSubInfoArrField(VT_PROTO_FIELD &vtField, const SubInfoAckItem &ackItem)
{
	static BOOL arOptional[] = {
		FALSE, FALSE, FALSE,
	};
	static EProtoFildType arFieldType[] = {
		ProtoFild_Int32, ProtoFild_Int32, ProtoFild_StringW,
	};
	static LPCSTR arFieldKey[] = {
		"StockSubType",	  "Market",	"StockCode", 
	};

	SubInfoAckItem &item = const_cast<SubInfoAckItem &>(ackItem);
	void *arPtr[] = {
		&item.nStockSubType, &item.nStockMarket, &item.strStockCode, 
	};

	CHECK_OP(_countof(arOptional) == _countof(arFieldType), NOOP);
	CHECK_OP(_countof(arOptional) == _countof(arFieldKey), NOOP);
	CHECK_OP(_countof(arOptional) == _countof(arPtr), NOOP);

	vtField.clear();
	PROTO_FIELD field;
	for ( int n = 0; n < _countof(arOptional); n++ )
	{
		field.bOptional = arOptional[n];
		field.eFieldType = arFieldType[n];
		field.strFieldKey = arFieldKey[n];
		switch (field.eFieldType)
		{
		case ProtoFild_Int32:
			field.pInt32 = (int*)arPtr[n];
			break;
		case ProtoFild_Int64:
			field.pInt64 = (INT64*)arPtr[n];
			break;
		case ProtoFild_StringA:
			field.pStrA = (std::string*)arPtr[n];
			break;
		case ProtoFild_StringW:
			field.pStrW = (std::wstring*)arPtr[n];
			break;
		default:
			CHECK_OP(FALSE, NOOP);
			break;
		}

		vtField.push_back(field);
	}	
}