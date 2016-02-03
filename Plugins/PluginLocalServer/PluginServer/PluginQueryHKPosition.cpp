#include "stdafx.h"
#include "PluginQueryHKPosition.h"
#include "PluginHKTradeServer.h"
#include "Protocol/ProtoQueryHKPosition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_ID_HANDLE_TIMEOUT_REQ	355
#define EVENT_ID_ACK_REQUEST		368

//tomodify 2
#define PROTO_ID_QUOTE		PROTO_ID_TDHK_QUERY_POSITION
typedef CProtoQueryHKPosition	CProtoQuote;

//////////////////////////////////////////////////////////////////////////

CPluginQueryHKPosition::CPluginQueryHKPosition()
{	
	m_pTradeOp = NULL;
	m_pTradeServer = NULL;
	m_bStartTimerHandleTimeout = FALSE;
}

CPluginQueryHKPosition::~CPluginQueryHKPosition()
{
	Uninit();
}

void CPluginQueryHKPosition::Init(CPluginHKTradeServer* pTradeServer, ITrade_HK*  pTradeOp)
{
	if ( m_pTradeServer != NULL )
		return;

	if ( pTradeServer == NULL || pTradeOp == NULL )
	{
		ASSERT(false);
		return;
	}

	m_pTradeServer = pTradeServer;
	m_pTradeOp = pTradeOp;
	m_TimerWnd.SetEventInterface(this);
	m_TimerWnd.Create();

	m_MsgHandler.SetEventInterface(this);
	m_MsgHandler.Create();
}

void CPluginQueryHKPosition::Uninit()
{
	if ( m_pTradeServer != NULL )
	{
		m_pTradeServer = NULL;
		m_pTradeOp = NULL;

		m_TimerWnd.Destroy();
		m_TimerWnd.SetEventInterface(NULL);

		m_MsgHandler.Close();
		m_MsgHandler.SetEventInterface(NULL);

		ClearAllReqAckData();
	}
}

void CPluginQueryHKPosition::SetTradeReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock)
{
	CHECK_RET(nCmdID == PROTO_ID_QUOTE && sock != INVALID_SOCKET, NORET);
	CHECK_RET(m_pTradeOp && m_pTradeServer, NORET);
	
	CProtoQuote proto;
	CProtoQuote::ProtoReqDataType	req;
	proto.SetProtoData_Req(&req);
	if ( !proto.ParseJson_Req(jsnVal) )
	{
		CHECK_OP(false, NORET);
		return;
	}

	CHECK_RET(req.head.nProtoID == nCmdID && req.body.nCookie, NORET);

	StockDataReq *pReq = new StockDataReq;
	CHECK_RET(pReq, NORET);
	pReq->sock = sock;
	pReq->dwReqTick = ::GetTickCount();
	pReq->req = req;	
	
	m_vtReqData.push_back(pReq);

	//tomodify 3
	QueryPositionReqBody &body = req.body;	
	bool bRet = m_pTradeOp->QueryPositionList((Trade_Env)body.nEnvType, (UINT32*)&pReq->dwLocalCookie);

	if ( !bRet )
	{
		TradeAckType ack;
		ack.head = req.head;
		ack.head.ddwErrCode = PROTO_ERR_UNKNOWN_ERROR;
		CA::Unicode2UTF(L"发送失败", ack.head.strErrDesc);

		memset(&ack.body, 0, sizeof(ack.body));
		ack.body.nEnvType = body.nEnvType;
		ack.body.nCookie = body.nCookie;		
		HandleTradeAck(&ack, sock);

		DoDeleteReqData(pReq);

		return ;
	}
	SetTimerHandleTimeout(true);
}

bool CPluginQueryHKPosition::DoDeleteReqData(StockDataReq* pReq)
{
	VT_REQ_TRADE_DATA::iterator it = m_vtReqData.end();
	while (it != m_vtReqData.end())
	{
		if (*it  == pReq)
		{ 
			SAFE_DELETE(pReq);
			it = m_vtReqData.erase(it);
			return true; 
		}
		++it;
	}
	return false;
}

void CPluginQueryHKPosition::NotifyOnQueryPosition(Trade_Env enEnv, UINT32 nCookie, INT32 nCount, const Trade_PositionItem* pArrPosition)
{
	CHECK_RET(nCookie, NORET);
	CHECK_RET(m_pTradeOp && m_pTradeServer, NORET);

	VT_REQ_TRADE_DATA::iterator itReq = m_vtReqData.begin();
	StockDataReq *pFindReq = NULL;
	for ( ; itReq != m_vtReqData.end(); ++itReq )
	{
		StockDataReq *pReq = *itReq;
		CHECK_OP(pReq, continue);
		if ( pReq->dwLocalCookie == nCookie )
		{
			pFindReq = pReq;
			break;
		}
	}
	if (!pFindReq)
		return;

	TradeAckType ack;
	ack.head = pFindReq->req.head;
	ack.head.ddwErrCode = PROTO_ERR_NO_ERROR;
// 	if ( nErrCode )
// 	{
// 		WCHAR szErr[256] = L"";
// 		if ( m_pTradeOp->GetErrDescV2(nErrCode, szErr) )
// 			CA::Unicode2UTF(szErr, ack.head.strErrDesc);
// 	}

	//tomodify 4
	ack.body.nEnvType = enEnv;
	ack.body.nCookie = pFindReq->req.body.nCookie;

	if ( nCount > 0 && pArrPosition )
	{
		for ( int n = 0; n < nCount; n++ )
		{			
			const Trade_PositionItem &pos = pArrPosition[n];
			QueryPositionAckItem item;
			item.strStockCode = pos.szCode;
			item.strStockName = pos.szName;
			item.nQty = pos.nQty;
			item.nCanSellQty = pos.nCanSellQty;
			item.nNominalPrice = pos.nNominalPrice;
			item.nMarketVal = pos.nMarketVal;
			item.nCostPrice = int(pos.fCostPrice * 1000);
			item.nCostPriceValid = pos.bCostPriceValid;
			item.nPLVal = pos.nPLVal;
			item.nPLValValid = pos.bPLValValid;
			item.nPLRatio = int(pos.fPLRatio * 100000);
			item.nPLRatioValid = pos.bPLRatioValid;
			
			item.nToday_PLVal = pos.nToday_PLVal;
			item.nToday_BuyQty = pos.nToday_BuyQty;
			item.nToday_BuyVal = pos.nToday_BuyVal;
			item.nToday_SellQty = pos.nToday_SellQty;
			item.nToday_SellVal = pos.nToday_SellVal;
			ack.body.vtPosition.push_back(item);
		}
	}	 
	
	HandleTradeAck(&ack, pFindReq->sock);

	m_vtReqData.erase(itReq);
	delete pFindReq;
}

void CPluginQueryHKPosition::OnTimeEvent(UINT nEventID)
{
	if ( TIMER_ID_HANDLE_TIMEOUT_REQ == nEventID )
	{
		HandleTimeoutReq();
	}
}

void CPluginQueryHKPosition::OnMsgEvent(int nEvent,WPARAM wParam,LPARAM lParam)
{
	if ( EVENT_ID_ACK_REQUEST == nEvent )
	{		
	}	
}

void CPluginQueryHKPosition::HandleTimeoutReq()
{
	if ( m_vtReqData.empty() )
	{
		SetTimerHandleTimeout(false);
		return;
	}

	DWORD dwTickNow = ::GetTickCount();	
	VT_REQ_TRADE_DATA::iterator it_req = m_vtReqData.begin();
	for ( ; it_req != m_vtReqData.end(); )
	{
		StockDataReq *pReq = *it_req;	
		if ( pReq == NULL )
		{
			CHECK_OP(false, NOOP);
			++it_req;
			continue;
		}		

		if ( int(dwTickNow - pReq->dwReqTick) > 8000 )
		{
			TradeAckType ack;
			ack.head = pReq->req.head;
			ack.head.ddwErrCode= PROTO_ERR_SERVER_TIMEROUT;
			CA::Unicode2UTF(L"协议超时", ack.head.strErrDesc);

			//tomodify 5
			memset(&ack.body, 0, sizeof(ack.body));
			ack.body.nEnvType = pReq->req.body.nEnvType;
			ack.body.nCookie = pReq->req.body.nCookie;			
			
			HandleTradeAck(&ack, pReq->sock);
			
			it_req = m_vtReqData.erase(it_req);
			delete pReq;
		}
		else
		{
			++it_req;
		}
	}

	if ( m_vtReqData.empty() )
	{
		SetTimerHandleTimeout(false);
		return;
	}
}

void CPluginQueryHKPosition::HandleTradeAck(TradeAckType *pAck, SOCKET sock)
{
	CHECK_RET(pAck && pAck->body.nCookie && sock != INVALID_SOCKET, NORET);
	CHECK_RET(m_pTradeServer, NORET);

	CProtoQuote proto;
	proto.SetProtoData_Ack(pAck);

	Json::Value jsnValue;
	bool bRet = proto.MakeJson_Ack(jsnValue);
	CHECK_RET(bRet, NORET);
	
	std::string strBuf;
	CProtoParseBase::ConvJson2String(jsnValue, strBuf, true);
	m_pTradeServer->ReplyTradeReq(PROTO_ID_QUOTE, strBuf.c_str(), (int)strBuf.size(), sock);
}

void CPluginQueryHKPosition::SetTimerHandleTimeout(bool bStartOrStop)
{
	if ( m_bStartTimerHandleTimeout )
	{
		if ( !bStartOrStop )
		{			
			m_TimerWnd.StopTimer(TIMER_ID_HANDLE_TIMEOUT_REQ);
			m_bStartTimerHandleTimeout = FALSE;
		}
	}
	else
	{
		if ( bStartOrStop )
		{
			m_TimerWnd.StartMillionTimer(500, TIMER_ID_HANDLE_TIMEOUT_REQ);
			m_bStartTimerHandleTimeout = TRUE;
		}
	}
}

void CPluginQueryHKPosition::ClearAllReqAckData()
{
	VT_REQ_TRADE_DATA::iterator it_req = m_vtReqData.begin();
	for ( ; it_req != m_vtReqData.end(); )
	{
		StockDataReq *pReq = *it_req;
		delete pReq;
	}

	m_vtReqData.clear();
}