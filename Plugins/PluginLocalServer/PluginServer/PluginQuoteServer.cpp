#include "stdafx.h"
#include "PluginQuoteServer.h"
#include "PluginNetwork.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern GUID PLUGIN_GUID;

//////////////////////////////////////////////////////////////////////////

CPluginQuoteServer::CPluginQuoteServer()
{
	m_pPluginCore = NULL;
	m_pQuoteData = NULL;
	m_pQuoteOp = NULL;
	m_pNetwork = NULL;
}

CPluginQuoteServer::~CPluginQuoteServer()
{
	UninitQuoteSvr();
}

void CPluginQuoteServer::InitQuoteSvr(IFTPluginCore* pPluginCore, CPluginNetwork *pNetwork)
{
	if ( m_pPluginCore != NULL )
		return;

	if ( pPluginCore == NULL || pNetwork == NULL )
	{
		ASSERT(false);
		return;
	}

	m_pNetwork = pNetwork;
	m_pPluginCore = pPluginCore;
	pPluginCore->QueryFTInterface(IID_IFTQuoteData, (void**)&m_pQuoteData);
	pPluginCore->QueryFTInterface(IID_IFTQuoteOperation, (void**)&m_pQuoteOp);

	if ( m_pQuoteData == NULL || m_pQuoteOp == NULL )
	{
		ASSERT(false);
		m_pQuoteData = NULL;
		m_pQuoteOp = NULL;
		m_pPluginCore = NULL;
		m_pNetwork = NULL;
		return;
	}

	m_BasicPrice.Init(this, m_pQuoteData);
	m_GearPrice.Init(this, m_pQuoteData);
}

void CPluginQuoteServer::UninitQuoteSvr()
{
	if ( m_pPluginCore != NULL )
	{	
		m_BasicPrice.Uninit();
		m_GearPrice.Uninit();

		m_pQuoteData = NULL;
		m_pQuoteOp = NULL;
		m_pPluginCore = NULL;
		m_pNetwork = NULL;
	}
}

void  CPluginQuoteServer::OnChanged_PriceBase(INT64  ddwStockHash)
{
	m_BasicPrice.NotifyQuoteDataUpdate(PROTO_ID_QT_GET_BASIC_PRICE, ddwStockHash);
}

void  CPluginQuoteServer::OnChanged_OrderQueue(INT64 ddwStockHash)
{
	m_GearPrice.NotifyQuoteDataUpdate(PROTO_ID_QT_GET_GEAR_PRICE, ddwStockHash);
}

void CPluginQuoteServer::SetQuoteReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock)
{
	switch (nCmdID)
	{
	case PROTO_ID_QT_GET_BASIC_PRICE:
		m_BasicPrice.SetQuoteReqData(nCmdID, jsnVal, sock);
		break;

	case PROTO_ID_QT_GET_GEAR_PRICE:
		m_GearPrice.SetQuoteReqData(nCmdID, jsnVal, sock);
		break;

	default:
		CHECK_OP(false, NOOP);
		break;
	}
}

void CPluginQuoteServer::ReplyQuoteReq(int nCmdID, const char *pBuf, int nLen, SOCKET sock)
{
	CHECK_RET(nCmdID && pBuf && nLen && sock != INVALID_SOCKET, NORET);
	CHECK_RET(m_pNetwork, NORET);
	m_pNetwork->SendData(sock, pBuf, nLen);
}

StockSubErrCode CPluginQuoteServer::SubscribeQuote(const std::string &strCode, StockMktType nMarketType, QuoteServerType type, bool bSubOrUnsub)
{
	CHECK_RET(m_pQuoteOp, StockSub_FailUnknown);

	StockSubErrCode err_code = StockSub_Suc;
	std::wstring strUnicode;
	CA::UTF2Unicode(strCode.c_str(), strUnicode);

	switch (type)
	{
	case QuoteServer_BasicPrice:
		err_code = m_pQuoteOp->Subscribe_PriceBase(PLUGIN_GUID, strUnicode.c_str(), nMarketType, bSubOrUnsub);
		break;

	case QuoteServer_GearPrice:
		err_code = m_pQuoteOp->Subscribe_OrderQueue(PLUGIN_GUID, strUnicode.c_str(), nMarketType, bSubOrUnsub);
		break;

	default:
		err_code = StockSub_FailUnknown;
		break;
	}

	return err_code;
}