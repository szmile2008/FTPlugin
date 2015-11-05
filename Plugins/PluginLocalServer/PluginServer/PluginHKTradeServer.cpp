#include "stdafx.h"
#include "PluginHKTradeServer.h"
#include "PluginNetwork.h"
#include "Protocol/ProtoOrderErrorPush.h"
#include "Protocol/ProtoOrderUpdatePush.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern GUID PLUGIN_GUID;

//////////////////////////////////////////////////////////////////////////

CPluginHKTradeServer::CPluginHKTradeServer()
{
	m_pPluginCore = NULL;
	m_pTradeOp = NULL;	
	m_pNetwork = NULL;
}

CPluginHKTradeServer::~CPluginHKTradeServer()
{
	UninitTradeSvr();
}

void CPluginHKTradeServer::InitTradeSvr(IFTPluginCore* pPluginCore, CPluginNetwork *pNetwork)
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
	pPluginCore->QueryFTInterface(IID_IFTTrade, (void**)&m_pTradeOp);

	if ( m_pTradeOp == NULL )
	{
		ASSERT(false);		
		m_pTradeOp = NULL;
		m_pPluginCore = NULL;
		m_pNetwork = NULL;
		return;
	}	

	m_PlaceOrder.Init(this, m_pTradeOp);
	m_ChangeOrder.Init(this, m_pTradeOp);
	m_SetOrderStatus.Init(this, m_pTradeOp);
}

void CPluginHKTradeServer::UninitTradeSvr()
{
	if ( m_pPluginCore != NULL )
	{
		m_PlaceOrder.Uninit();
		m_ChangeOrder.Uninit();
		m_SetOrderStatus.Uninit();

		m_pTradeOp = NULL;
		m_pPluginCore = NULL;
		m_pNetwork = NULL;
	}	
}

void CPluginHKTradeServer::SetTradeReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock)
{
	switch (nCmdID)
	{
	case PROTO_ID_TDHK_PUSH_ORDER_UPDATE:
		CHECK_OP(false, NOOP);
		break;

	case PROTO_ID_TDHK_PUSH_ORDER_ERROR:
		CHECK_OP(false, NOOP);
		break;

	case PROTO_ID_TDHK_PLACE_ORDER:
		m_PlaceOrder.SetTradeReqData(nCmdID, jsnVal, sock);
		break;

	case PROTO_ID_TDHK_SET_ORDER_STATUS:
		m_SetOrderStatus.SetTradeReqData(nCmdID, jsnVal, sock);
		break;

	case PROTO_ID_TDHK_CHANGE_ORDER:
		m_ChangeOrder.SetTradeReqData(nCmdID, jsnVal, sock);
		break;

	default:
		CHECK_OP(false, NOOP);
		break;
	}
}

void CPluginHKTradeServer::ReplyTradeReq(int nCmdID, const char *pBuf, int nLen, SOCKET sock)
{
	CHECK_RET(nCmdID && pBuf && nLen && sock != INVALID_SOCKET, NORET);
	CHECK_RET(m_pNetwork, NORET);
	m_pNetwork->SendData(sock, pBuf, nLen);
}

void CPluginHKTradeServer::OnPlaceOrder(Trade_Env enEnv, UINT nCookie, Trade_SvrResult enSvrRet, UINT64 nLocalID, UINT16 nErrCode)
{
	m_PlaceOrder.NotifyOnPlaceOrder(enEnv, nCookie, enSvrRet, nLocalID, nErrCode);
}

void CPluginHKTradeServer::OnOrderUpdate(Trade_Env enEnv, const Trade_OrderItem_HK& orderItem)
{
	CHECK_RET(m_pNetwork, NORET);

	OrderUpdatePushHK_Ack ack;
	ack.head.nErrCode = 0;
	ack.head.nProtoID = PROTO_ID_TDHK_PUSH_ORDER_UPDATE;
	ack.head.nProtoVer = 1;

	ack.body.nEnvType = enEnv;
	ack.body.nLocalID = orderItem.nLocalID;
	ack.body.nOrderID = orderItem.nOrderID;
	ack.body.nOrderDir = orderItem.enSide;
	ack.body.nOrderTypeHK = orderItem.enType;
	ack.body.nOrderStatusHK = orderItem.enStatus;
	ack.body.nPrice = (int)orderItem.nPrice;
	ack.body.nQTY = orderItem.nQty;
	ack.body.nDealQTY = orderItem.nDealtQty;
	ack.body.nSubmitTime = (int)orderItem.nSubmitedTime;
	ack.body.nUpdateTime = (int)orderItem.nUpdatedTime;

	CA::Unicode2UTF(orderItem.szName, ack.body.strStockName);
	CA::Unicode2UTF(orderItem.szCode, ack.body.strStockCode);	 

	CProtoOrderUpdatePush proto;
	proto.SetProtoData_Ack(&ack);

	Json::Value jsnValue;
	bool bRet = proto.MakeJson_Ack(jsnValue);
	CHECK_RET(bRet, NORET);

	std::string strBuf;
	CProtoParseBase::ConvJson2String(jsnValue, strBuf, true);
	m_pNetwork->PushData(strBuf.c_str(), (int)strBuf.size());
}

void CPluginHKTradeServer::OnSetOrderStatus(Trade_Env enEnv, UINT nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, UINT16 nErrCode)
{
	m_SetOrderStatus.NotifyOnSetOrderStatus(enEnv, nCookie, enSvrRet, nOrderID, nErrCode);
}

void CPluginHKTradeServer::OnChangeOrder(Trade_Env enEnv, UINT nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, UINT16 nErrCode)
{
	m_ChangeOrder.NotifyOnPlaceOrder(enEnv, nCookie, enSvrRet, nOrderID, nErrCode);
}

void CPluginHKTradeServer::OnOrderErrNotify(Trade_Env enEnv, UINT64 nOrderID, Trade_OrderErrNotify_HK enErrNotify, UINT16 nErrCode)
{
	CHECK_RET(m_pNetwork && m_pTradeOp, NORET);

	OrderErrorPushHK_Ack ack;
	ack.head.nErrCode = 0;
	ack.head.nProtoID = PROTO_ID_TDHK_PUSH_ORDER_ERROR;
	ack.head.nProtoVer = 1;

	ack.body.nEnvType = enEnv;
	ack.body.nOrderID = nOrderID;
	ack.body.nOrderErrNotifyHK = enErrNotify;
	ack.body.nOrderErrCode = nErrCode;

	wchar_t szErrDesc[256] = L"";
	m_pTradeOp->GetErrDesc(nErrCode, szErrDesc);
	CA::Unicode2UTF(szErrDesc, ack.body.strOrderErrDesc);
	
	CProtoOrderErrorPush proto;
	proto.SetProtoData_Ack(&ack);

	Json::Value jsnValue;
	bool bRet = proto.MakeJson_Ack(jsnValue);
	CHECK_RET(bRet, NORET);

	std::string strBuf;
	CProtoParseBase::ConvJson2String(jsnValue, strBuf, true);
	m_pNetwork->PushData(strBuf.c_str(), (int)strBuf.size());
}