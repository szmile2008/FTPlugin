#include "stdafx.h"
#include "MsgHandler.h"

// CMsgHandler

IMPLEMENT_DYNAMIC(CMsgHandler, CWnd)

#define WM_EVENT_MESSAGE			(WM_USER + 546)

CMsgHandler::CMsgHandler()
{
	m_pEventInterface = NULL;
}

CMsgHandler::~CMsgHandler()
{
	Close();

#ifdef MsgHandle_Track_Memory
	CSingleLock lock(&m_cs, TRUE); 
	//窗口销毁了，没有收到Post消息, 清除相应的EventData对象 
	std::map<tagEventMsgData* , BOOL>::iterator  itMap = m_mapEventData.begin(); 
	while (itMap != m_mapEventData.end())
	{
		if (itMap->second)
			delete itMap->first; 
		++itMap; 
	}
	m_mapEventData.clear(); 
#endif
}

BEGIN_MESSAGE_MAP(CMsgHandler, CWnd)
	ON_MESSAGE(WM_EVENT_MESSAGE,OnEventMessage)
END_MESSAGE_MAP()

// CMsgHandler 消息处理程序
BOOL CMsgHandler::Create()
{
	Close();
	return CWnd::CreateEx(0,AfxRegisterWndClass(0),_T(""),WS_POPUP,CRect(0,0,1,1),NULL,0);
}

BOOL CMsgHandler::IsCreated()
{
	if(::IsWindow(m_hWnd))
		return TRUE;
	else
		return FALSE;
}

void CMsgHandler::Close()
{
	if(::IsWindow(m_hWnd))
		DestroyWindow();
}

void CMsgHandler::SetEventInterface(CMsgHandlerEventInterface* pInterface)
{
	m_pEventInterface = pInterface;
}

void CMsgHandler::RaiseEvent(int nEvent,WPARAM wParam,LPARAM lParam)
{
	if (!GetSafeHwnd())
		return;

	if(m_pEventInterface)
	{
		tagEventMsgData* pEvent = new tagEventMsgData;
		pEvent->nEvent = nEvent;
		pEvent->wParam = wParam;
		pEvent->lParam = lParam; 

#ifdef MsgHandle_Track_Memory
		if (1)
		{
			CSingleLock lock(&m_cs, TRUE);
			m_mapEventData[pEvent] = TRUE;
		}
#endif 
		
		PostMessage(WM_EVENT_MESSAGE,0,(LPARAM)pEvent);
	}
}

LRESULT CMsgHandler::OnEventMessage(WPARAM wParam,LPARAM lParam)
{
	if(lParam != 0)
	{
		tagEventMsgData* pEvent = (tagEventMsgData*)lParam;
		int nEvent = pEvent->nEvent;
		LPARAM lParam = pEvent->lParam;
		WPARAM wParam = pEvent->wParam;
		
#ifdef MsgHandle_Track_Memory
		if (1)
		{
			CSingleLock  lock(&m_cs, TRUE);
			m_mapEventData.erase(pEvent);
		}
#endif
		
		SAFE_DELETE(pEvent);

		if(m_pEventInterface)
			m_pEventInterface->OnMsgEvent(nEvent,wParam,lParam);
	}
	return 0;
}
