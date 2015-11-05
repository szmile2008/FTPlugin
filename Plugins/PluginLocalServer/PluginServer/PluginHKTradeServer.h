#pragma once
#include "Include/Define_Struct_Plugin.h"
#include "Include/IQuote.h"
#include "Include/ITrade.h"
#include "Protocol/ProtoDataStruct.h"
#include "JsonCpp/json.h"
#include "PluginPlaceOrder.h"
#include "PluginChangeOrder.h"
#include "PluginSetOrderStatus.h"

class CPluginNetwork;

class CPluginHKTradeServer: 	
	public ITradeCallBack_HK
{
public:
	CPluginHKTradeServer();
	virtual ~CPluginHKTradeServer();
	
	void InitTradeSvr(IFTPluginCore* pPluginCore, CPluginNetwork *pNetwork);
	void UninitTradeSvr();	
	void SetTradeReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock);
	void ReplyTradeReq(int nCmdID, const char *pBuf, int nLen, SOCKET sock);	

protected:
	//ITradeCallBack_HK
	virtual void OnPlaceOrder(Trade_Env enEnv, UINT nCookie, Trade_SvrResult enSvrRet, UINT64 nLocalID, UINT16 nErrCode);	
	virtual void OnOrderUpdate(Trade_Env enEnv, const Trade_OrderItem_HK& orderItem);
	virtual void OnSetOrderStatus(Trade_Env enEnv, UINT nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, UINT16 nErrCode);
	virtual void OnChangeOrder(Trade_Env enEnv, UINT nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, UINT16 nErrCode);
	virtual void OnOrderErrNotify(Trade_Env enEnv, UINT64 nOrderID, Trade_OrderErrNotify_HK enErrNotify, UINT16 nErrCode);


protected:
	IFTPluginCore		*m_pPluginCore;
	ITrade_HK			*m_pTradeOp;	
	CPluginNetwork		*m_pNetwork;
	CPluginPlaceOrder	m_PlaceOrder;
	CPluginChangeOrder  m_ChangeOrder;
	CPluginSetOrderStatus m_SetOrderStatus;
};