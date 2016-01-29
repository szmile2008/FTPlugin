#pragma once
#include "MsgHandler.h"
#include "include/ITrade.h"
#include "TimerWnd.h"

interface IOrderIDCvtNotify_US
{
	virtual void OnCvtOrderID_Local2Svr(int nResult, Trade_Env eEnv, INT64 nLocalID, INT64 nServerID) = 0;
};
class CDelayOrderIDCvt_US:public CTimerWndInterface 
{
public:
	CDelayOrderIDCvt_US(void);
	~CDelayOrderIDCvt_US(void);

	void Init(ITrade_US* pTradeObj, IOrderIDCvtNotify_US* pNotify); 
	void Uninit();  

	INT64 FindSvrOrderID(Trade_Env eEnv, INT64 nLocalID, bool bDelayFindAndNotify); 

protected:
	virtual void OnTimeEvent(UINT nEventID);

private:
	typedef struct tagDelayCvtInfo
	{ 
		tagDelayCvtInfo()
		{
			eEnv  = Trade_Env_Real;
			nLocalID = 0;
			nDelayCount = 0;
		}
		tagDelayCvtInfo(Trade_Env eEnvParam, INT64 nLocalIDParam)
		{
			eEnv = eEnvParam;
			nLocalID = nLocalIDParam;
			nDelayCount = 0;
		}
		Trade_Env eEnv; 
		INT64	nLocalID;
		int		nDelayCount;
	}DelayCvtInfo, *LP_DelayCvtInfo; 
	typedef std::vector<LP_DelayCvtInfo> VT_DelayCvtInfo; 

private:
	INT64 DoFindSvrOrderID(Trade_Env eEnv, INT64 nLocalID); 
	LP_DelayCvtInfo DoFindReqObj(Trade_Env eEnv, INT64 nLocalID,VT_DelayCvtInfo::iterator* pIter = NULL); 
	void	DoAddNewDelayReq(Trade_Env eEnv, INT64 nLocalID); 
	void	DoCheckDelayReq(); 

	void	StartTime_LoopFind();
	void	KillTime_LoopFind();

private:
	VT_DelayCvtInfo			m_vtReq; 
	CTimerMsgWndEx			m_TimerWnd;
	ITrade_US*				m_pTradeObj; 
	IOrderIDCvtNotify_US*	m_pNotify; 
	UINT m_nTimerID_LoopFind;
};
