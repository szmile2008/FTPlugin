#include "stdafx.h"
#include "PluginStockList.h"
#include "PluginQuoteServer.h"
#include "Protocol/ProtoStockList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EVENT_ID_ACK_REQUEST	368

//tomodify 2
#define PROTO_ID_QUOTE		PROTO_ID_QT_GET_STOCK_LIST

typedef CProtoStockList		CProtoQuote;


//////////////////////////////////////////////////////////////////////////

CPluginStockList::CPluginStockList()
{	
	m_pQuoteData = NULL;
	m_pQuoteServer = NULL;
}

CPluginStockList::~CPluginStockList()
{
	Uninit();
}

void CPluginStockList::Init(CPluginQuoteServer* pQuoteServer, IFTQuoteData*  pQuoteData)
{
	if ( m_pQuoteServer != NULL )
		return;

	if ( pQuoteServer == NULL || pQuoteData == NULL )
	{
		ASSERT(false);
		return;
	}

	m_pQuoteServer = pQuoteServer;
	m_pQuoteData = pQuoteData;

	m_MsgHandler.SetEventInterface(this);
	m_MsgHandler.Create();
}

void CPluginStockList::Uninit()
{
	ReleaseAllReqData();

	if ( m_pQuoteServer != NULL )
	{
		m_pQuoteServer = NULL;
		m_pQuoteData = NULL;		

		m_MsgHandler.Close();
		m_MsgHandler.SetEventInterface(NULL);
	}
}

void CPluginStockList::SetQuoteReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock)
{
	CHECK_RET(nCmdID == PROTO_ID_QUOTE && sock != INVALID_SOCKET, NORET);
	CHECK_RET(m_pQuoteData && m_pQuoteServer, NORET);
	
	CProtoQuote proto;
	CProtoQuote::ProtoReqDataType  req;	
	proto.SetProtoData_Req(&req);
	if ( !proto.ParseJson_Req(jsnVal) )
	{
		CHECK_OP(false, NORET);
		return;
	}

	CHECK_RET(req.head.nProtoID == nCmdID, NORET);
	
	StockDataReq *pReqInfo = new StockDataReq;
	CHECK_RET(pReqInfo, NORET);	
	pReqInfo->sock = sock;
	pReqInfo->req = req;
	pReqInfo->dwReqTick = ::GetTickCount();
	m_vtReqData.push_back(pReqInfo);

	m_MsgHandler.RaiseEvent(EVENT_ID_ACK_REQUEST, 0, 0);
}

void CPluginStockList::NotifyQuoteDataUpdate(int nCmdID, INT64 nStockID)
{
	CHECK_OP(false, NOOP);
	//CHECK_RET(nCmdID == PROTO_ID_QUOTE && nStockID, NORET);
	//CHECK_RET(m_pQuoteData, NORET);
}

void CPluginStockList::OnMsgEvent(int nEvent,WPARAM wParam,LPARAM lParam)
{
	if ( EVENT_ID_ACK_REQUEST == nEvent )
	{
		ReplyAllRequest();
	}	
}

void CPluginStockList::ReplyAllRequest()
{
	CHECK_RET(m_pQuoteData && m_pQuoteServer, NORET);
		
	VT_STOCK_DATA_REQ vtReqData;
	vtReqData.swap(m_vtReqData);

	//tomodify 3
	VT_STOCK_DATA_REQ::iterator it = vtReqData.begin();
	for ( ; it != vtReqData.end(); ++it )
	{
		StockDataReq *pReqData = *it;
		CHECK_OP(pReqData, continue);
		CProtoQuote::ProtoReqBodyType &reqBody = pReqData->req.body;	

		LPPluginStockInfo *parStocksInfo = NULL;
		int nStocksInfoNum = 0;
		m_pQuoteData->GetStocksInfo((StockMktType)reqBody.nStockMarket, (PluginSecurityType)reqBody.nSecurityType, parStocksInfo, nStocksInfoNum);
		if ( parStocksInfo == NULL )
			nStocksInfoNum = 0;

		CProtoQuote::ProtoAckBodyType ackBody;
		ackBody.nStockMarket = reqBody.nStockMarket;
		for ( int n = 0; n < nStocksInfoNum; n++ )
		{
			LPPluginStockInfo pInfo = parStocksInfo[n];
			CHECK_OP(pInfo, continue);			
			StockListAckItem stkItem;
			stkItem.nStockID = pInfo->nStockID;
			stkItem.nLotSize = pInfo->nLotSize;
			stkItem.nSecurityType = pInfo->nSecType;
			std::string strCode, strName;
			CA::Unicode2UTF(pInfo->chCodeSig, strCode);
			CA::Unicode2UTF(pInfo->chSimpName, strName);
			stkItem.strStockCode = strCode;
			stkItem.strSimpName = strName;
			ackBody.vtStockList.push_back(stkItem);
		}
		
		ReplyStockDataReq(pReqData, ackBody);
	}
}

void CPluginStockList::ReplyStockDataReq(StockDataReq *pReq, const QuoteAckDataBody &data)
{
	CHECK_RET(pReq && m_pQuoteServer, NORET);

	CProtoQuote::ProtoAckDataType ack;
	ack.head = pReq->req.head;
	ack.head.ddwErrCode = 0;
	ack.body = data;

	//tomodify 4
	CProtoQuote proto;	
	proto.SetProtoData_Ack(&ack);

	Json::Value jsnAck;
	if ( proto.MakeJson_Ack(jsnAck) )
	{
		std::string strOut;
		CProtoParseBase::ConvJson2String(jsnAck, strOut, true);
		m_pQuoteServer->ReplyQuoteReq(pReq->req.head.nProtoID, strOut.c_str(), (int)strOut.size(), pReq->sock);
	}
	else
	{
		CHECK_OP(false, NOOP);
	}
}

void CPluginStockList::ReplyDataReqError(StockDataReq *pReq, int nErrCode, LPCWSTR pErrDesc)
{
	CHECK_RET(pReq && m_pQuoteServer, NORET);

	CProtoQuote::ProtoAckDataType ack;
	ack.head = pReq->req.head;
	ack.head.ddwErrCode = nErrCode;

	if ( pErrDesc )
	{
		CA::Unicode2UTF(pErrDesc, ack.head.strErrDesc);		 
	}

	CProtoQuote proto;	
	proto.SetProtoData_Ack(&ack);

	Json::Value jsnAck;
	if ( proto.MakeJson_Ack(jsnAck) )
	{
		std::string strOut;
		CProtoParseBase::ConvJson2String(jsnAck, strOut, true);
		m_pQuoteServer->ReplyQuoteReq(pReq->req.head.nProtoID, strOut.c_str(), (int)strOut.size(), pReq->sock);
	}
	else
	{
		CHECK_OP(false, NOOP);
	}
}

void CPluginStockList::ReleaseAllReqData()
{
	VT_STOCK_DATA_REQ::iterator it = m_vtReqData.begin();
	for ( ; it != m_vtReqData.end(); ++it )
	{
		StockDataReq *pReqData = *it;
		delete pReqData;
	}
	m_vtReqData.clear();
}