#pragma once
#include "Include/Define_Struct_Plugin.h"
#include "Include/IQuote.h"
#include "Include/ITrade.h"
#include "Protocol/ProtoDataStruct.h"
#include "JsonCpp/json.h"
#include "PluginPlaceOrder_US.h"
#include "PluginChangeOrder_US.h"
#include "PluginSetOrderStatus_US.h"

class CPluginNetwork;

class CPluginUSTradeServer: 	
	public ITradeCallBack_US
{
public:
	CPluginUSTradeServer();
	virtual ~CPluginUSTradeServer();
	
	void InitTradeSvr(IFTPluginCore* pPluginCore, CPluginNetwork *pNetwork);
	void UninitTradeSvr();	
	void SetTradeReqData(int nCmdID, const Json::Value &jsnVal, SOCKET sock);
	void ReplyTradeReq(int nCmdID, const char *pBuf, int nLen, SOCKET sock);	

protected:
	//ITradeCallBack_US
	virtual void OnPlaceOrder(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nLocalID, INT64 nErrHash);
	virtual void OnOrderUpdate(const Trade_OrderItem& orderItem);

	virtual void OnCancelOrder(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, INT64 nErrHash);
	virtual void OnChangeOrder(UINT32 nCookie, Trade_SvrResult enSvrRet, UINT64 nOrderID, INT64 nErrHash);

	//virtual void OnOrderErrNotify(Trade_Env enEnv, UINT64 nOrderID, Trade_OrderErrNotify_HK enErrNotify, UINT16 nErrCode);
	virtual void OnQueryOrderList(UINT32 nCookie, INT32 nCount, const Trade_OrderItem* pArrOrder) {}
	virtual void OnQueryAccInfo(UINT32 nCookie, const Trade_AccInfo& accInfo){}
	virtual void OnQueryPositionList(UINT32 nCookie, INT32 nCount, const Trade_PositionItem* pArrPosition){}

protected:
	IFTPluginCore		*m_pPluginCore;
	ITrade_US			*m_pTradeOp;	
	CPluginNetwork		*m_pNetwork;
	CPluginPlaceOrder_US	m_PlaceOrder;
	CPluginChangeOrder_US  m_ChangeOrder;
	CPluginSetOrderStatus_US m_SetOrderStatus;
};