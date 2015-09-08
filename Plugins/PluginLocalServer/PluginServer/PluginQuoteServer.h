#pragma once
#include "Include/Define_Struct_Plugin.h"
#include "Include/IQuote.h"
#include "Include/ITrade.h"
#include "Protocol/ProtoDataStruct.h"
#include "JsonCpp/json.h"
#include "PluginBasicPrice.h"
#include "PluginGearPrice.h"

class CPluginNetwork;

enum QuoteServerType
{
	QuoteServer_BasicPrice = 1,
	QuoteServer_GearPrice = 2,
};

class CPluginQuoteServer: 	
	public IQuoteInfoCallback
{
public:
	CPluginQuoteServer();
	virtual ~CPluginQuoteServer();
	
	void InitQuoteSvr(IFTPluginCore* pPluginCore, CPluginNetwork *pNetwork);
	void UninitQuoteSvr();	
	void SetQuoteReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock);
	void ReplyQuoteReq(int nCmdID, const char *pBuf, int nLen, SOCKET sock);	
	StockSubErrCode SubscribeQuote(const std::string &strCode, StockMktType nMarketType, QuoteServerType type, bool bSubOrUnsub);	


protected:
	//IQuoteInfoCallback
	virtual void  OnChanged_PriceBase(INT64  ddwStockHash); 
	virtual void  OnChanged_OrderQueue(INT64 ddwStockHash); 

protected:
	IFTPluginCore		*m_pPluginCore;
	IFTQuoteData		*m_pQuoteData;
	IFTQuoteOperation	*m_pQuoteOp;
	CPluginNetwork		*m_pNetwork;

	CPluginBasicPrice	m_BasicPrice;
	CPluginGearPrice	m_GearPrice;
};