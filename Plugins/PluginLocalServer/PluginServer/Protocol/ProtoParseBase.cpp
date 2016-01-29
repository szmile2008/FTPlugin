#include "stdafx.h"
#include "ProtoParseBase.h"
#include "CppJsonConv.h"
#include "../JsonCpp/json.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CProtoParseBase::CProtoParseBase()
{

}

CProtoParseBase::~CProtoParseBase()	
{

}

bool  CProtoParseBase::ConvBuffer2Json(const char *pBuf, int nBufLen, Json::Value &jsnVal)
{
	CHECK_RET(pBuf && nBufLen >= 2, false);

	int nValidLen = nBufLen;
	char chTmp = pBuf[nValidLen - 1];
	while ( chTmp == 0xd || chTmp == 0xA )
	{
		nValidLen--;
		if ( nValidLen < 2 )
		{
			CHECK_RET(false, false);
		}
		chTmp = pBuf[nValidLen - 1];
	}

	Json::Reader reader;
	bool bRet = reader.parse(pBuf, pBuf + nValidLen, jsnVal);
	return bRet;
}

bool  CProtoParseBase::ConvJson2String(const Json::Value &jsnVal, std::string &strOut, bool bAppendCRLF)
{
	Json::FastWriter writer;
	strOut = writer.write(jsnVal);

	if ( strOut.empty() )
		return false;
	
	strOut.append("\r\n\r\n");
	return true;
}

int	CProtoParseBase::GetProtoID(const Json::Value &jsnVal)
{
	CppJsonConv conv;
	conv.SetJsonValue(jsnVal);
	int nProID = 0;
	bool bSuc = conv.GetInt32Value("Protocol", nProID);
	CHECK_OP(bSuc, NOOP);
	return nProID;
}

bool CProtoParseBase::ParseProtoHead_Req(const Json::Value &jsnVal, ProtoHead &head)
{
	//将不用解析的项初始化
	head.ddwErrCode = 0;
	head.strErrDesc = "";

	VT_PROTO_FIELD vtField;	
	GetProtoHeadField_Req(vtField, head);
	
	return ParseProtoFields(jsnVal, vtField);
}

bool CProtoParseBase::ParseProtoHead_Ack(const Json::Value &jsnVal, ProtoHead &head)
{	
	VT_PROTO_FIELD vtField;
	GetProtoHeadField_Ack(vtField, head);

	return ParseProtoFields(jsnVal, vtField);
}

bool CProtoParseBase::MakeProtoHead_Req(Json::Value &jsnVal, const ProtoHead &head)
{
	VT_PROTO_FIELD vtField;
	ProtoHead hd = const_cast<ProtoHead&>(head);
	GetProtoHeadField_Req(vtField, hd);

	return MakeProtoFields(jsnVal, vtField);
}

bool CProtoParseBase::MakeProtoHead_Ack(Json::Value &jsnVal, const ProtoHead &head)
{
	VT_PROTO_FIELD vtField;
	ProtoHead hd = const_cast<ProtoHead&>(head);
	GetProtoHeadField_Ack(vtField, hd);

	return MakeProtoFields(jsnVal, vtField);
}

bool CProtoParseBase::ParseProtoFields(const Json::Value &jsnVal, const VT_PROTO_FIELD &vtField)
{
	CppJsonConv conv;
	conv.SetJsonValue(jsnVal);

	bool bRet = true;
	VT_PROTO_FIELD::const_iterator it = vtField.begin();
	for ( ; it != vtField.end(); ++it )
	{
		const PROTO_FIELD &field = *it;
		bool bFieldSuc = false;	

		switch ( field.eFieldType )
		{
		case ProtoFild_Int32:
			if ( field.pInt32 )
				bFieldSuc = conv.GetInt32Value(field.strFieldKey, *field.pInt32);
			break;

		case ProtoFild_Int64:
			if ( field.pInt32 )
				bFieldSuc = conv.GetInt64Value(field.strFieldKey, *field.pInt64);
			break;

		case ProtoFild_StringA:
			if ( field.pStrA )
				bFieldSuc = conv.GetStringValueA(field.strFieldKey, *field.pStrA);
			break;

		case ProtoFild_StringW:
			if ( field.pStrW )
				bFieldSuc = conv.GetStringValueW(field.strFieldKey, *field.pStrW);
			break;

		default:
			CHECK_OP(FALSE, NOOP);
			break;
		}

		if ( !field.bOptional && !bFieldSuc )
		{
			bRet = false;
			CHECK_OP(false, break);
		}		
	}

	return bRet;
}

bool CProtoParseBase::MakeProtoFields(Json::Value &jsnVal, const VT_PROTO_FIELD &vtField)
{
	CppJsonConv conv;
	conv.SetJsonValue(jsnVal);

	bool bRet = true;
	VT_PROTO_FIELD::const_iterator it = vtField.begin();
	for ( ; it != vtField.end(); ++it )
	{
		const PROTO_FIELD &field = *it;
		bool bFieldSuc = false;	

		switch ( field.eFieldType )
		{
		case ProtoFild_Int32:
			if ( field.pInt32 )
				bFieldSuc = conv.SetInt32Value(field.strFieldKey, *field.pInt32);
			break;

		case ProtoFild_Int64:
			if ( field.pInt32 )
				bFieldSuc = conv.SetInt64Value(field.strFieldKey, *field.pInt64);
			break;

		case ProtoFild_StringA:
			if ( field.pStrA )
				bFieldSuc = conv.SetStringValueA(field.strFieldKey, *field.pStrA);
			break;

		case ProtoFild_StringW:
			if ( field.pStrW )
				bFieldSuc = conv.SetStringValueW(field.strFieldKey, *field.pStrW);
			break;

		default:
			CHECK_OP(FALSE, NOOP);
			break;
		}

		if ( !field.bOptional && !bFieldSuc )
		{
			bRet = false;
			CHECK_OP(false, break);
		}		
	}

	return bRet;
}

void CProtoParseBase::GetProtoHeadField_Ack(VT_PROTO_FIELD &vtField, const ProtoHead &head)
{
	static BOOL arOptional[] = {
		FALSE, FALSE, 
		FALSE, TRUE,
	};
	static EProtoFildType arFieldType[] = {
		ProtoFild_Int32, ProtoFild_Int32, 
		ProtoFild_Int64, ProtoFild_StringA,
	};
	static LPCSTR arFieldKey[] = {
		"Version",	"Protocol",		
		"ErrCode",	"ErrDesc",
	};

	ProtoHead &hd = const_cast<ProtoHead &>(head);
	void *arPtr[] = {
		&hd.nProtoVer,	&hd.nProtoID, 
		&hd.ddwErrCode, &hd.strErrDesc,
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


void CProtoParseBase::GetProtoHeadField_Req(VT_PROTO_FIELD &vtField, const ProtoHead &head)
{
	static BOOL arOptional[] = {
		FALSE, FALSE, 		
	};
	static EProtoFildType arFieldType[] = {
		ProtoFild_Int32, ProtoFild_Int32, 
	};
	static LPCSTR arFieldKey[] = {
		"Version",	"Protocol",
	};

	ProtoHead &hd = const_cast<ProtoHead &>(head);
	void *arPtr[] = {
		&hd.nProtoVer,	&hd.nProtoID, 
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