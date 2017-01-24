#pragma once
#include "Include/FTPluginCore.h"
#include "Include/FTPluginQuoteInterface.h"
#include "Protocol/ProtoDataStruct_Quote.h"
#include "TimerWnd.h"
#include "MsgHandler.h"
#include "JsonCpp/json.h"

class CPluginQuoteServer;

class CPluginStockList : public CMsgHandlerEventInterface
{
public:
	CPluginStockList();
	virtual ~CPluginStockList();
	
	void Init(CPluginQuoteServer* pQuoteServer, IFTQuoteData*  pQuoteData);
	void Uninit();	
	void SetQuoteReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock);
	void NotifyQuoteDataUpdate(int nCmdID, INT64 nStockID);

protected:	
	//CMsgHandlerEventInterface
	virtual void OnMsgEvent(int nEvent,WPARAM wParam,LPARAM lParam);

protected:
	//tomodify 1
	struct	StockDataReq
	{
		SOCKET	sock;
		DWORD	dwReqTick;
		StockList_Req req;
	};
	typedef std::vector<StockDataReq*>	VT_STOCK_DATA_REQ;
	typedef StockListAckBody	QuoteAckDataBody;	

protected:	
	void ReplyAllRequest();
	void ReplyStockDataReq(StockDataReq *pReq, const QuoteAckDataBody &data);
	void ReplyDataReqError(StockDataReq *pReq, int nErrCode, LPCWSTR pErrDesc);

	void ReleaseAllReqData();	
	
protected:
	CPluginQuoteServer* m_pQuoteServer;
	IFTQuoteData*		m_pQuoteData;
	CMsgHandler			m_MsgHandler;

	VT_STOCK_DATA_REQ	m_vtReqData;
};