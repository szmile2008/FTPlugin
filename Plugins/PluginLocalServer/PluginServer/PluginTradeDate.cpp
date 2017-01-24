#include "stdafx.h"
#include "PluginTradeDate.h"
#include "PluginQuoteServer.h"
#include "Protocol/ProtoTradeDate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EVENT_ID_ACK_REQUEST	368

//tomodify 2
#define PROTO_ID_QUOTE		PROTO_ID_QT_GET_TRADE_DATE

typedef CProtoTradeDate		CProtoQuote;


//////////////////////////////////////////////////////////////////////////

CPluginTradeDate::CPluginTradeDate()
{	
	m_pQuoteData = NULL;
	m_pQuoteServer = NULL;
}

CPluginTradeDate::~CPluginTradeDate()
{
	Uninit();
}

void CPluginTradeDate::Init(CPluginQuoteServer* pQuoteServer, IFTQuoteData*  pQuoteData)
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

void CPluginTradeDate::Uninit()
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

void CPluginTradeDate::SetQuoteReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock)
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

void CPluginTradeDate::NotifyQuoteDataUpdate(int nCmdID, INT64 nStockID)
{
	CHECK_OP(false, NOOP);
	//CHECK_RET(nCmdID == PROTO_ID_QUOTE && nStockID, NORET);
	//CHECK_RET(m_pQuoteData, NORET);
}

void CPluginTradeDate::OnMsgEvent(int nEvent,WPARAM wParam,LPARAM lParam)
{
	if ( EVENT_ID_ACK_REQUEST == nEvent )
	{
		ReplyAllRequest();
	}	
}

void CPluginTradeDate::ReplyAllRequest()
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
		
		LPCWSTR pszDateFrom = NULL, pszDateTo = NULL;
		std::wstring strDateFrom, strDateTo;
		if ( !reqBody.strStartDate.empty() )
		{
			CA::UTF2Unicode(reqBody.strStartDate.c_str(), strDateFrom);
			pszDateFrom = strDateFrom.c_str();
		}
		if ( !reqBody.strEndDate.empty() )
		{
			CA::UTF2Unicode(reqBody.strEndDate.c_str(), strDateTo);
			pszDateTo = strDateTo.c_str();
		}
		int *parDateList = NULL;
		int nDateListLen = 0;
		m_pQuoteData->GetTradeDateList((StockMktType)reqBody.nStockMarket, pszDateFrom, pszDateTo, parDateList, nDateListLen);
		if ( parDateList == NULL )
			nDateListLen = 0;

		CProtoQuote::ProtoAckBodyType ackBody;
		ackBody.nStockMarket = reqBody.nStockMarket;
		ackBody.strStartDate = reqBody.strStartDate;
		ackBody.strEndDate = reqBody.strEndDate;
		int nTimezone = GetMarketTimezone((StockMktType)reqBody.nStockMarket);
		for ( int  n = 0; n < nDateListLen; n++ )
		{	
			int nDate = parDateList[n];			
			//std::string strFmtTime;
			//FormatTimestampToDate(parDateList[n], nTimezone, strFmtTime);
			char buf[16] = {0};
			sprintf_s(buf, "%d-%02d-%02d", nDate / 10000, nDate / 100 % 100, nDate % 100);
			ackBody.vtTradeDate.push_back(buf);
		}	
		
		ReplyStockDataReq(pReqData, ackBody);
	}
}

void CPluginTradeDate::ReplyStockDataReq(StockDataReq *pReq, const QuoteAckDataBody &data)
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

void CPluginTradeDate::ReplyDataReqError(StockDataReq *pReq, int nErrCode, LPCWSTR pErrDesc)
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

void CPluginTradeDate::ReleaseAllReqData()
{
	VT_STOCK_DATA_REQ::iterator it = m_vtReqData.begin();
	for ( ; it != m_vtReqData.end(); ++it )
	{
		StockDataReq *pReqData = *it;
		delete pReqData;
	}
	m_vtReqData.clear();
}

int  CPluginTradeDate::GetMarketTimezone(StockMktType eMkt)
{
	switch (eMkt)
	{
	case StockMkt_US:
		return -5;
		break;
	default:
		return 8;
		break;
	}
}

void CPluginTradeDate::FormatTimestampToDate(int nTimestamp, int nTimezone, std::string &strFmtTime)
{
	time_t nTimezoneTimestamp = nTimestamp + nTimezone * 3600;		
	struct tm *stTime = gmtime(&nTimezoneTimestamp);
	char szBuf[32];
	sprintf_s(szBuf, "%d-%02d-%02d", stTime->tm_year + 1900, stTime->tm_mon + 1, stTime->tm_mday);
	strFmtTime = szBuf;
}