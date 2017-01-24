#include "stdafx.h"
#include "PluginBatchBasic.h"
#include "PluginQuoteServer.h"
#include "Protocol/ProtoBatchBasic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_ID_CLEAR_CACHE		354
#define TIMER_ID_HANDLE_TIMEOUT_REQ	355

#define EVENT_ID_ACK_REQUEST	368

//tomodify 2
#define PROTO_ID_QUOTE		PROTO_ID_QT_GET_BATCHBASIC
#define QUOTE_SERVER_TYPE	QuoteServer_QueryBatchBasic
typedef CProtoBatchBasic	CProtoQuote;


//////////////////////////////////////////////////////////////////////////

CPluginBatchBasic::CPluginBatchBasic()
{	
	m_pQuoteData = NULL;
	m_pQuoteServer = NULL;

	m_bStartTimerClearCache = FALSE;
	m_bStartTimerHandleTimeout = FALSE;
}

CPluginBatchBasic::~CPluginBatchBasic()
{
	Uninit();
}

void CPluginBatchBasic::Init(CPluginQuoteServer* pQuoteServer, IFTQuoteData*  pQuoteData)
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
	m_TimerWnd.SetEventInterface(this);
	m_TimerWnd.Create();

	m_MsgHandler.SetEventInterface(this);
	m_MsgHandler.Create();
}

void CPluginBatchBasic::Uninit()
{
	if ( m_pQuoteServer != NULL )
	{
		m_pQuoteServer = NULL;
		m_pQuoteData = NULL;
		m_TimerWnd.Destroy();
		m_TimerWnd.SetEventInterface(NULL);

		m_MsgHandler.Close();
		m_MsgHandler.SetEventInterface(NULL);

		ClearAllReqCache();
	}
}

void CPluginBatchBasic::SetQuoteReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock)
{
	CHECK_RET(nCmdID == PROTO_ID_QUOTE && sock != INVALID_SOCKET, NORET);
	CHECK_RET(m_pQuoteData && m_pQuoteServer, NORET);

	CProtoQuote proto;
	CProtoQuote::ProtoReqDataType	req;
	proto.SetProtoData_Req(&req);
	if ( !proto.ParseJson_Req(jsnVal) )
	{
		CHECK_OP(false, NORET);
		StockDataReq req_info;
		req_info.sock = sock;
		req_info.req = req;
		ReplyDataReqError(&req_info, PROTO_ERR_PARAM_ERR, L"参数错误！");
		return;
	}

	CHECK_RET(req.head.nProtoID == nCmdID, NORET);

	if ( (int)req.body.vtReqBatchBasic.size() > 50 || (int)req.body.vtReqBatchBasic.size() < 1)
	{
		//////参数错误
		StockDataReq req_info;
		//req_info.nStockID = nStockID;
		req_info.sock = sock;
		req_info.req = req;
		ReplyDataReqError(&req_info, PROTO_ERR_PARAM_ERR, L"参数错误！");
		return;
	}


	for (int i = 0; i < (int)req.body.vtReqBatchBasic.size(); i++)
	{
		std::wstring strCode;
		CA::UTF2Unicode(req.body.vtReqBatchBasic[i].strStockCode.c_str(), strCode);
		INT64 nStockID = m_pQuoteData->GetStockHashVal(strCode.c_str(), (StockMktType)req.body.vtReqBatchBasic[i].nStockMarket);
		if ( nStockID == 0 )
		{
			CHECK_OP(false, NOOP);
			StockDataReq req_info;

			req_info.sock = sock;
			req_info.req = req;
			ReplyDataReqError(&req_info, PROTO_ERR_STOCK_NOT_FIND, L"找不到股票！");
			return;
		}

		if ( m_mapStockIDCode.find(nStockID) == m_mapStockIDCode.end() )
		{
			StockMktCode &mkt_code = m_mapStockIDCode[nStockID];
			mkt_code.nMarketType = req.body.vtReqBatchBasic[i].nStockMarket;
			mkt_code.strCode = req.body.vtReqBatchBasic[i].strStockCode;
		}
	}

	StockDataReq *pReqInfo = new StockDataReq;
	CHECK_RET(pReqInfo, NORET);

	pReqInfo->sock = sock;
	pReqInfo->req = req;

	DWORD dwTime = ::GetTickCount();
	VT_STOCK_DATA_REQ &vtReq = m_mapReqInfo[std::make_pair<SOCKET, DWORD>(sock, dwTime)];
	bool bNeedSub = vtReq.empty();	
	vtReq.push_back(pReqInfo);

	if ( bNeedSub )
	{
		bool bIsSub = false;
		for (int i = 0; i < (int)req.body.vtReqBatchBasic.size(); i++)
		{
			std::wstring strCode;
			CA::UTF2Unicode(req.body.vtReqBatchBasic[i].strStockCode.c_str(), strCode);
			INT64 nStockID = m_pQuoteData->GetStockHashVal(strCode.c_str(), (StockMktType)req.body.vtReqBatchBasic[i].nStockMarket);
			bIsSub = m_pQuoteData->IsSubStockOneType(nStockID, StockSubType_Simple);
			if ( bIsSub == false )
			{
				break;
			}
		}

		if ( bIsSub )
		{
			QuoteAckDataBody &ack = m_mapCacheData[std::make_pair<SOCKET, DWORD>(sock, dwTime)];
			Quote_BatchBasic batchprice;
			bool bFillSuccess = false;
			for (int i = 0; i < (int)req.body.vtReqBatchBasic.size(); i++)
			{
				std::wstring strCode;
				CA::UTF2Unicode(req.body.vtReqBatchBasic[i].strStockCode.c_str(), strCode);
				INT64 nStockID = m_pQuoteData->GetStockHashVal(strCode.c_str(), (StockMktType)req.body.vtReqBatchBasic[i].nStockMarket);
				if ( m_pQuoteData->FillBatchBasic(nStockID, &batchprice) )
				{
					bFillSuccess = true;
					BatchBasicAckItem Item;
					Item.nStockMarket = req.body.vtReqBatchBasic[i].nStockMarket;
					Item.strStockCode = req.body.vtReqBatchBasic[i].strStockCode;
					Item.nHigh = batchprice.dwHigh;
					Item.nOpen = batchprice.dwLastClose;
					Item.nLastClose = batchprice.dwLastClose;
					Item.nLow = batchprice.dwLow;
					Item.nCur = batchprice.dwCur;
					Item.nSuspension = batchprice.nSuspension;
					Item.nVolume = batchprice.ddwVolume;
					Item.nValue = batchprice.ddwTurnover;
					Item.nAmpli = batchprice.dwAmpli;
					Item.nTurnoverRate = batchprice.nTurnoverRate;
					wchar_t szListDate[64] = {};
					m_pQuoteData->TimeStampToStrDate(nStockID, batchprice.dwListTime, szListDate);
					Item.strListTime = szListDate;
					wchar_t szDate[64] = {}; 
					wchar_t szTime[64] = {}; 
					m_pQuoteData->TimeStampToStrDate(nStockID, batchprice.dwTime, szDate);
					m_pQuoteData->TimeStampToStrTime(nStockID, batchprice.dwTime, szTime);
					Item.strDate = szDate;
					Item.strTime = szTime;

					ack.vtAckBatchBasic.push_back(Item);
				}
				else
				{
					bFillSuccess = false;
					break;
				}
			}

			if ( bFillSuccess == true )
			{
				m_mapCacheData[std::make_pair<SOCKET, DWORD>(sock, dwTime)] = ack;
			}
			else
			{
				//对vtReq中的每一个
				for (size_t i = 0; i < vtReq.size(); i++)
				{
					StockDataReq *pReqAnswer = vtReq[i];
					ReplyDataReqError(pReqInfo, PROTO_ERR_UNKNOWN_ERROR, L"拉取数据失败！");
				}
				MAP_STOCK_DATA_REQ::iterator it_iterator = m_mapReqInfo.find(std::make_pair<SOCKET, DWORD>(sock, dwTime));
				if ( it_iterator != m_mapReqInfo.end() )
				{
					it_iterator = m_mapReqInfo.erase(it_iterator);
				}
				MAP_STOCK_CACHE_DATA::iterator it_iteratorcache = m_mapCacheData.find(std::make_pair<SOCKET, DWORD>(sock, dwTime));
				if ( it_iteratorcache != m_mapCacheData.end() )
				{
					it_iteratorcache = m_mapCacheData.erase(it_iteratorcache);
				}
				return;
			}
		}
		else
		{
			//对vtReq中的每一个
			for (size_t i = 0; i < vtReq.size(); i++)
			{
				StockDataReq *pReqAnswer = vtReq[i];
				ReplyDataReqError(pReqAnswer, PROTO_ERR_UNSUB_ERR, L"股票未订阅！");
			}
			MAP_STOCK_DATA_REQ::iterator it_iterator = m_mapReqInfo.find(std::make_pair<SOCKET, DWORD>(sock, dwTime));
			if ( it_iterator != m_mapReqInfo.end() )
			{
				it_iterator = m_mapReqInfo.erase(it_iterator);
			}
			return;
		}
	}

	m_MsgHandler.RaiseEvent(EVENT_ID_ACK_REQUEST, 0, 0);
	SetTimerHandleTimeout(true);
}

void CPluginBatchBasic::NotifyQuoteDataUpdate(int nCmdID, INT64 nStockID)
{
	CHECK_RET(nCmdID == PROTO_ID_QUOTE && nStockID, NORET);
	CHECK_RET(m_pQuoteData, NORET);
}

void CPluginBatchBasic::OnTimeEvent(UINT nEventID)
{
	if ( TIMER_ID_CLEAR_CACHE == nEventID )
	{
		ClearQuoteDataCache();
	}
	else if ( TIMER_ID_HANDLE_TIMEOUT_REQ == nEventID )
	{
		HandleTimeoutReq();
	}
}

void CPluginBatchBasic::OnMsgEvent(int nEvent,WPARAM wParam,LPARAM lParam)
{
	if ( EVENT_ID_ACK_REQUEST == nEvent )
	{
		ReplyAllReadyReq();
	}	
}

void CPluginBatchBasic::ClearQuoteDataCache()
{
	if ( m_mapCacheToDel.empty() )
	{
		SetTimerClearCache(false);
		return ;
	}

	DWORD dwTickNow = ::GetTickCount();

	MAP_CACHE_TO_DESTROY::iterator it_todel = m_mapCacheToDel.begin();
	for ( ; it_todel != m_mapCacheToDel.end(); )
	{
		SOCKET sock = it_todel->first.first;
		DWORD dwTime = it_todel->first.second;
		DWORD dwToDelTick = it_todel->second;

		MAP_STOCK_DATA_REQ::iterator it_req = m_mapReqInfo.find(std::make_pair<SOCKET, DWORD>(sock, dwTime));
		if ( it_req != m_mapReqInfo.end() )
		{
			it_todel = m_mapCacheToDel.erase(it_todel);
		}
		else
		{
			if ( int(dwTickNow - dwToDelTick) > 60*1000  )
			{
				m_mapCacheData.erase(std::make_pair<SOCKET, DWORD>(sock, dwTime));
				it_todel = m_mapCacheToDel.erase(it_todel);
			}
			else
			{
				++it_todel;
			}			
		}
	}

	if ( m_mapCacheToDel.empty() )
	{
		SetTimerClearCache(false);		
	}
}

void CPluginBatchBasic::HandleTimeoutReq()
{
	if ( m_mapReqInfo.empty() )
	{
		SetTimerHandleTimeout(false);
		return;
	}

	ReplyAllReadyReq();

	DWORD dwTickNow = ::GetTickCount();	
	MAP_STOCK_DATA_REQ::iterator it_stock = m_mapReqInfo.begin();
	for ( ; it_stock != m_mapReqInfo.end(); )
	{
		VT_STOCK_DATA_REQ &vtReq = it_stock->second;
		VT_STOCK_DATA_REQ::iterator it_req = vtReq.begin();

		for ( ; it_req != vtReq.end(); )
		{
			StockDataReq *pReq = *it_req;
			if ( pReq == NULL )
			{
				CHECK_OP(false, NOOP);
				it_req = vtReq.erase(it_req);
				continue;
			}

			if ( int(dwTickNow - pReq->dwReqTick) > 5000 )
			{
				CStringA strTimeout;
				strTimeout.Format("BatchPrice req timeout");
				OutputDebugStringA(strTimeout.GetString());				
				ReplyDataReqError(pReq, PROTO_ERR_SERVER_TIMEROUT, L"请求超时！");
				it_req = vtReq.erase(it_req);
				delete pReq;
			}
			else
			{
				++it_req;
			}
		}

		if ( vtReq.empty() )
		{
			//这里不用启动清缓定时器，因为这时没有缓存当前股票的数据
			it_stock = m_mapReqInfo.erase(it_stock);			
		}
		else
		{
			++it_stock;
		}
	}

	if ( m_mapReqInfo.empty() )
	{
		SetTimerHandleTimeout(false);
		return;
	}
}

void CPluginBatchBasic::ReplyAllReadyReq()
{
	DWORD dwTickNow = ::GetTickCount();
	MAP_STOCK_DATA_REQ::iterator it_stock = m_mapReqInfo.begin();
	for ( ; it_stock != m_mapReqInfo.end(); )
	{
		SOCKET sock = it_stock->first.first;
		DWORD dwTime = it_stock->first.second;
		VT_STOCK_DATA_REQ &vtReq = it_stock->second;
		MAP_STOCK_CACHE_DATA::iterator it_data = m_mapCacheData.find(std::make_pair<SOCKET, DWORD>(sock, dwTime));

		if ( it_data == m_mapCacheData.end() )
		{
			++it_stock;
			continue;
		}
		
		VT_STOCK_DATA_REQ::iterator it_req = vtReq.begin();
		for ( ; it_req != vtReq.end(); ++it_req )
		{
			StockDataReq *pReq = *it_req;
			CHECK_OP(pReq, NOOP);
			ReplyStockDataReq(pReq, it_data->second);
			delete pReq;
		}

		vtReq.clear();

		it_stock = m_mapReqInfo.erase(it_stock);
		m_mapCacheToDel[std::make_pair<SOCKET, DWORD>(sock, dwTime)] = dwTickNow;
		SetTimerClearCache(true);
	}

	if ( m_mapReqInfo.empty() )
	{
		SetTimerHandleTimeout(false);
		return;
	}
}

void CPluginBatchBasic::ReplyStockDataReq(StockDataReq *pReq, const QuoteAckDataBody &data)
{
	CHECK_RET(pReq && m_pQuoteServer, NORET);

	CProtoQuote::ProtoAckDataType ack;
	ack.head = pReq->req.head;
	ack.head.ddwErrCode = 0;
	ack.body = data;

	////tomodify 4
	//ack.body.nStockMarket = pReq->req.body.nStockMarket;
	//ack.body.strStockCode = pReq->req.body.strStockCode;
	//int nNumToGet = pReq->req.body.nGetGearNum;
	//if ( (int)ack.body.vtGear.size() > nNumToGet )
	//{
	//	ack.body.vtGear.resize(nNumToGet);
	//}

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

void CPluginBatchBasic::ReplyDataReqError(StockDataReq *pReq, int nErrCode, LPCWSTR pErrDesc)
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

void CPluginBatchBasic::SetTimerHandleTimeout(bool bStartOrStop)
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

void CPluginBatchBasic::SetTimerClearCache(bool bStartOrStop)
{
	if ( m_bStartTimerClearCache )
	{
		if ( !bStartOrStop )
		{
			m_TimerWnd.StopTimer(TIMER_ID_CLEAR_CACHE);
			m_bStartTimerClearCache = FALSE;
		}
	}
	else
	{
		if ( bStartOrStop )
		{
			m_TimerWnd.StartMillionTimer(50, TIMER_ID_CLEAR_CACHE);
			m_bStartTimerClearCache = TRUE;
		}
	}
}

bool CPluginBatchBasic::GetStockMktCode(INT64 nStockID, StockMktCode &stkMktCode)
{
	MAP_STOCK_ID_CODE::iterator it_find = m_mapStockIDCode.find(nStockID);
	if ( it_find != m_mapStockIDCode.end())
	{
		stkMktCode = it_find->second;
		return true;
	}

	CHECK_OP(false, NOOP);
	return false;
}

void CPluginBatchBasic::ClearAllReqCache()
{
	MAP_STOCK_DATA_REQ::iterator it_stock = m_mapReqInfo.begin();
	for ( ; it_stock != m_mapReqInfo.end(); ++it_stock )
	{
		VT_STOCK_DATA_REQ &vtReq = it_stock->second;
		VT_STOCK_DATA_REQ::iterator it_req = vtReq.begin();
		for ( ; it_req != vtReq.end(); ++it_req )
		{
			StockDataReq *pReq = *it_req;
			delete pReq;
		}
	}

	m_mapReqInfo.clear();
	m_mapCacheData.clear();
	m_mapCacheToDel.clear();
	m_mapStockIDCode.clear();
}