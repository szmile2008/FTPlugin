#include "stdafx.h"
#include "TimerWnd.h" 
#include <map> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CTimerWndEx  
typedef struct tagWndTimerInfo
{
	int  nMillSec; 
	int  nWndTimerID;
	UINT nEventID; 
	tagWndTimerInfo()
	{
		nMillSec = 0; 
		nWndTimerID = 0; 
		nEventID; 
	}
}WND_TIMER_INFO,*LP_WND_TIMER_INFO;  

std::map<HWND, CTimerMsgWndEx*>  g_mapTimerWndEx; 


CTimerMsgWndEx::CTimerMsgWndEx()
{
	m_pInterface = NULL;
	m_hWnd = NULL; 
}

CTimerMsgWndEx::~CTimerMsgWndEx()
{
	Destroy();
}

BOOL CTimerMsgWndEx::Create()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_hWnd) 
		return true; 

	static CString s_strWndClassEx; 
	if (s_strWndClassEx.IsEmpty()) 
		s_strWndClassEx = AfxRegisterWndClass(0); 
	
	HINSTANCE hInstance= AfxGetApp()? AfxGetApp()->m_hInstance: NULL; 
	m_hWnd = CreateWindow((LPCTSTR)s_strWndClassEx, _T(""), WS_POPUP, 0,0,1,1, ::GetDesktopWindow(),
		0,  hInstance, NULL); 

	if(m_hWnd) 
		g_mapTimerWndEx[m_hWnd] = this; 

	ASSERT(m_hWnd); 
	return m_hWnd != NULL; 
}

void CTimerMsgWndEx::Destroy()
{
	if (!m_hWnd)
	 return;  

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	std::map<int, LPVOID>::iterator it = m_mapTimerInfo.begin(); 
	while (it != m_mapTimerInfo.end())
	{
		LP_WND_TIMER_INFO pInfo = (LP_WND_TIMER_INFO)it->second; 
		if (pInfo) 
		{
			::KillTimer(m_hWnd, pInfo->nWndTimerID); 
			delete pInfo; 
		} 
		++it; 
	}
	m_mapTimerInfo.clear(); 

	DestroyWindow(m_hWnd); 
	m_hWnd = NULL; 
}


BOOL  CTimerMsgWndEx::StartMillionTimer(UINT nMillSec, UINT nEventID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_hWnd)
	{
		ASSERT(false); 
		return false; 
	}

	std::map<int, LPVOID>::iterator it = m_mapTimerInfo.find(nEventID);  
	LP_WND_TIMER_INFO pInfo = (LP_WND_TIMER_INFO)FindFromEventID(nEventID); 
	if (pInfo)  
	{ 
		::KillTimer(m_hWnd, pInfo->nWndTimerID);  
		m_mapTimerInfo.erase(pInfo->nWndTimerID); 
		delete pInfo; 
	}
	
	static int s_nID = 1;  
	int nWndTimerID = (int)::SetTimer(m_hWnd, s_nID++, nMillSec, &CTimerMsgWndEx::TimerProc); 

	if (nWndTimerID >0) 
	{  
		LP_WND_TIMER_INFO pInfo = new tagWndTimerInfo; 
		pInfo->nEventID = nEventID; 
		pInfo->nWndTimerID = nWndTimerID; 
		pInfo->nMillSec = nMillSec;  
		m_mapTimerInfo[nWndTimerID] = pInfo; 
		return true; 
	}

	ASSERT(false); 
	return false; 
}

void	CTimerMsgWndEx::StopTimer(UINT nEventID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	LP_WND_TIMER_INFO pInfo = (LP_WND_TIMER_INFO)FindFromEventID(nEventID); 
	if (pInfo) 
	{ 
		::KillTimer(m_hWnd, pInfo->nWndTimerID);  
		m_mapTimerInfo.erase(pInfo->nWndTimerID); 
	    delete pInfo; 
	}
} 

BOOL	CTimerMsgWndEx::StartTimer(UINT nSeconds,UINT nEventID)
{
	 return StartMillionTimer(nSeconds*1000, nEventID); 
}

void	CTimerMsgWndEx::SetEventInterface(CTimerWndInterface* pInterface)
{
	m_pInterface = pInterface; 
} 

void CALLBACK CTimerMsgWndEx::TimerProc(HWND hwnd, UINT uMsg, UINT idEvent,DWORD dwTime) 
{ 
	CTimerMsgWndEx* pTimerWnd = g_mapTimerWndEx[hwnd]; 

	if (pTimerWnd && pTimerWnd->GetSafeHWnd() == hwnd) 
	{ 
		pTimerWnd->OnTimer(idEvent);
	}
	else 
	{
		ASSERT(FALSE);
	} 
}

void CTimerMsgWndEx::OnTimer(UINT nIDTimer) 
{
	std::map<int, LPVOID>::iterator it= m_mapTimerInfo.find(nIDTimer); 
	if ( it != m_mapTimerInfo.end() && m_pInterface) 
	{ 
		LP_WND_TIMER_INFO pInfo = (LP_WND_TIMER_INFO)(it->second); 
		if (pInfo) 
		{ 
			m_pInterface->OnTimeEvent(pInfo->nEventID); 
		}
		
	}
}

LPVOID CTimerMsgWndEx::FindFromEventID(UINT nEventID)
{
	std::map<int, LPVOID>::iterator it = m_mapTimerInfo.begin(); 
	while (it != m_mapTimerInfo.end()) 
	{
		LP_WND_TIMER_INFO pInfo = (LP_WND_TIMER_INFO)it->second; 
		if (pInfo && pInfo->nEventID == nEventID)
		{ 
			return it->second; 
		}
		++it; 
	}
	return NULL; 
}
