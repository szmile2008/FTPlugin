#pragma once
#include <map> 

#pragma  warning (disable:4251) 

class  CTimerWndInterface
{	
public:
	virtual void OnTimeEvent(UINT nEventID) = 0;
};


//支持多EventID的管理 
class  CTimerMsgWndEx
{
public:
	CTimerMsgWndEx();
	virtual ~CTimerMsgWndEx();
	///////////////////////
	BOOL	Create();
	void	Destroy();
	BOOL	StartTimer(UINT nSeconds,UINT nEventID);
	BOOL    StartMillionTimer(UINT nMillionSec, UINT nEventID); 
	void	StopTimer(UINT nEventID);
	void	SetEventInterface(CTimerWndInterface* pInterface);
	HWND    GetSafeHWnd() { return m_hWnd;}

private:
	//wndTimerID -> tagTimerInfo配对关系  
	std::map<int, LPVOID> m_mapTimerInfo;  
	CTimerWndInterface*	m_pInterface;
	HWND    m_hWnd; 

protected:
	LPVOID FindFromEventID(UINT nEventID); 
	afx_msg void OnTimer(UINT nIDEvent);
	static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent,DWORD dwTime); 	
};