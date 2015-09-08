#pragma once
#include <vector>
#include "../JsonCpp/json.h"
#include "ProtoDataStruct.h"

enum EProtoFildType
{
	ProtoFild_Int32		= 1,
	ProtoFild_Int64		= 2,
	ProtoFild_StringA	= 3,
	ProtoFild_StringW	= 4,
};

typedef struct tagProtoField
{	
	BOOL		   bOptional;
	EProtoFildType eFieldType;
	union
	{
		int		*pInt32;
		INT64	*pInt64;
		std::string *pStrA;
		std::wstring *pStrW;
	};
	std::string strFieldKey;
}PROTO_FIELD, *LP_PROTO_FIELD;

typedef std::vector<PROTO_FIELD>	VT_PROTO_FIELD;

//////////////////////////////////////////////////////////////////////////

class CProtoParseBase
{
public:
	CProtoParseBase();
	virtual ~CProtoParseBase();

	static bool	 ConvBuffer2Json(const char *pBuf, int nBufLen, Json::Value &jsnVal);
	static bool  ConvJson2String(const Json::Value &jsnVal, std::string &strOut, bool bAppendCRLF);
	static int	 GetProtoID(const Json::Value &jsnVal);
	
	virtual bool ParseJson_Req(const Json::Value &jsnVal) = 0;
	virtual bool ParseJson_Ack(const Json::Value &jsnVal) = 0;
	virtual bool MakeJson_Req(Json::Value &jsnVal) = 0;
	virtual bool MakeJson_Ack(Json::Value &jsnVal) = 0;

protected:	
	bool ParseProtoHead_Req(const Json::Value &jsnVal, ProtoHead &head);
	bool ParseProtoHead_Ack(const Json::Value &jsnVal, ProtoHead &head);
	bool MakeProtoHead_Req(Json::Value &jsnVal, const ProtoHead &head); 
	bool MakeProtoHead_Ack(Json::Value &jsnVal, const ProtoHead &head); 

protected:
	bool ParseProtoFields(const Json::Value &jsnVal, const VT_PROTO_FIELD &vtField);
	bool MakeProtoFields(Json::Value &jsnVal, const VT_PROTO_FIELD &vtField);

private:
	void GetProtoHeadField_Req(VT_PROTO_FIELD &vtField, const ProtoHead &head);
	void GetProtoHeadField_Ack(VT_PROTO_FIELD &vtField, const ProtoHead &head);
};