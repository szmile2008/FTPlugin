#pragma once
#include "Include/Define_Struct_Plugin.h"
#include "Include/IQuote.h"
#include "Include/ITrade.h"
#include "Protocol/ProtoDataStruct_Trade.h"
#include "TimerWnd.h"
#include "MsgHandler.h"
#include "JsonCpp/json.h"

class CPluginHKTradeServer;

class CPluginQueryHKAccInfo : public CTimerWndInterface, public CMsgHandlerEventInterface
{
public:
	CPluginQueryHKAccInfo();
	virtual ~CPluginQueryHKAccInfo();
	
	void Init(CPluginHKTradeServer* pTradeServer, ITrade_HK*  pTradeOp);
	void Uninit();	
	void SetTradeReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock);
	void NotifyOnQueryHKAccInfo(Trade_Env enEnv, UINT32 nCookie, const Trade_AccInfo& accInfo);

protected:
	//CTimerWndInterface 
	virtual void OnTimeEvent(UINT nEventID);

	//CMsgHandlerEventInterface
	virtual void OnMsgEvent(int nEvent,WPARAM wParam,LPARAM lParam);

protected:
	//tomodify 1
	typedef QueryHKAccInfo_Req	TradeReqType;
	typedef QueryHKAccInfo_Ack	TradeAckType;

	struct	StockDataReq
	{
		SOCKET	sock;
		DWORD	dwReqTick;
		DWORD	dwLocalCookie;
		TradeReqType req;
	};
	
	typedef std::vector<StockDataReq*>		VT_REQ_TRADE_DATA;	
	

protected:	
	void HandleTimeoutReq();
	void HandleTradeAck(TradeAckType *pAck, SOCKET	sock);
	void SetTimerHandleTimeout(bool bStartOrStop);
	void ClearAllReqAckData();
	
private: 
	bool	DoDeleteReqData(StockDataReq* pReq); 

protected:
	CPluginHKTradeServer	*m_pTradeServer;
	ITrade_HK				*m_pTradeOp;	
	BOOL					m_bStartTimerHandleTimeout;
	
	CTimerMsgWndEx		m_TimerWnd;
	CMsgHandler			m_MsgHandler;

	VT_REQ_TRADE_DATA	m_vtReqData;
};