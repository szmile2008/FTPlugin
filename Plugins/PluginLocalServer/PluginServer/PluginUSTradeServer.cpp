#include "stdafx.h"
#include "PluginUSTradeServer.h"
#include "PluginNetwork.h"
#include "Protocol/ProtoOrderErrorPush.h"
#include "Protocol/ProtoOrderUpdatePush.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern GUID PLUGIN_GUID;

//////////////////////////////////////////////////////////////////////////

CPluginUSTradeServer::CPluginUSTradeServer()
{
	m_pPluginCore = NULL;
	m_pTradeOp = NULL;	
	m_pNetwork = NULL;
}

CPluginUSTradeServer::~CPluginUSTradeServer()
{
	UninitTradeSvr();
}

void CPluginUSTradeServer::InitTradeSvr(IFTPluginCore* pPluginCore, CPluginNetwork *pNetwork)
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
	pPluginCore->QueryFTInterface(IID_IFTTrade_US, (void**)&m_pTradeOp);

	if ( m_pTradeOp == NULL )
	{
		ASSERT(false);		
		m_pTradeOp = NULL;
		m_pPluginCore = NULL;
		m_pNetwork = NULL;
		return;
	}	

	m_QueryPos.Init(this, m_pTradeOp);
	m_QueryUSAcc.Init(this, m_pTradeOp);
	m_QueryUSOrder.Init(this, m_pTradeOp);
	m_PlaceOrder.Init(this, m_pTradeOp);
	m_ChangeOrder.Init(this, m_pTradeOp);
	m_SetOrderStatus.Init(this, m_pTradeOp);
}

void CPluginUSTradeServer::UninitTradeSvr()
{
	if ( m_pPluginCore != NULL )
	{
		m_QueryPos.Uninit();
		m_QueryUSAcc.Uninit();
		m_QueryUSOrder.Uninit();
		m_PlaceOrder.Uninit();
		m_ChangeOrder.Uninit();
		m_SetOrderStatus.Uninit();

		m_pTradeOp = NULL;
		m_pPluginCore = NULL;
		m_pNetwork = NULL;
	}	
}

void CPluginUSTradeServer::SetTradeReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock)
{
	switch (nCmdID)
	{ 
	case PROTO_ID_TDUS_PLACE_ORDER:
		m_PlaceOrder.SetTradeReqData(nCmdID, jsnVal, sock);
		break;

	case PROTO_ID_TDUS_SET_ORDER_STATUS:
		m_SetOrderStatus.SetTradeReqData(nCmdID, jsnVal, sock);
		break;

	case PROTO_ID_TDUS_CHANGE_ORDER:
		m_ChangeOrder.SetTradeReqData(nCmdID, jsnVal, sock);
		break;

	case PROTO_ID_TDUS_QUERY_ORDER:
		m_QueryUSOrder.SetTradeReqData(nCmdID, jsnVal, sock);
		break;

	case PROTO_ID_TDUS_QUERY_ACC_INFO:
		m_QueryUSAcc.SetTradeReqData(nCmdID, jsnVal, sock);
		break;

	case PROTO_ID_TDUS_QUERY_POSITION:
		m_QueryPos.SetTradeReqData(nCmdID, jsnVal, sock);
		break;

	default:
		CHECK_OP(false, NOOP);
		break;
	}
}

void CPluginUSTradeServer::ReplyTradeReq(int nCmdID, const char *pBuf, int nLen, SOCKET sock)
{
	CHECK_RET(nCmdID && pBuf && nLen && sock != INVALID_SOCKET, NORET);
	CHECK_RET(m_pNetwork, NORET);
	m_pNetwork->SendData(sock, pBuf, nLen);
}

void CPluginUSTradeServer::OnPlaceOrder(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nLocalID, INT64 nErrHash)
{
	m_PlaceOrder.NotifyOnPlaceOrder(Trade_Env_Real, nCookie, enSvrRet, nLocalID, nErrHash);
}

void CPluginUSTradeServer::OnOrderUpdate(const Trade_OrderItem& orderItem)
{ 
	 
}

void CPluginUSTradeServer::OnCancelOrder(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, INT64 nErrHash)
{
	m_SetOrderStatus.NotifyOnSetOrderStatus(Trade_Env_Real, nCookie, enSvrRet, nOrderID, nErrHash);
}

void CPluginUSTradeServer::OnChangeOrder(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, INT64 nErrHash)
{
	m_ChangeOrder.NotifyOnChangeOrder(Trade_Env_Real, nCookie, enSvrRet, nOrderID, nErrHash);
}
 
void CPluginUSTradeServer::OnQueryOrderList(UINT32 nCookie, INT32 nCount, const Trade_OrderItem* pArrOrder)
{
	m_QueryUSOrder.NotifyOnQueryUSOrder(nCookie, nCount, pArrOrder);
}

void CPluginUSTradeServer::OnQueryAccInfo(UINT32 nCookie, const Trade_AccInfo& accInfo)
{
	m_QueryUSAcc.NotifyOnQueryUSAccInfo(Trade_Env_Real, nCookie, accInfo);
}

void CPluginUSTradeServer::OnQueryPositionList( UINT32 nCookie, INT32 nCount, const Trade_PositionItem* pArrPosition )
{
	m_QueryPos.NotifyOnQueryPosition(Trade_Env_Real, nCookie, nCount, pArrPosition);
}
