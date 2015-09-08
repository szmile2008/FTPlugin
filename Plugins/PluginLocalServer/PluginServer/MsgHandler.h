#pragma once
#include <afxmt.h>
#include <map>

// Release下最坏情况就是在窗口销毁后会少释放最后一个msg内存
#ifdef _DEBUG 
	#define  MsgHandle_Track_Memory     
#endif  

// CMsgHandler
class CMsgHandlerEventInterface
{	
public:
	virtual void OnMsgEvent(int nEvent,WPARAM wParam,LPARAM lParam) = 0;
};


class CMsgHandler : public CWnd
{
	DECLARE_DYNAMIC(CMsgHandler)
public:
	CMsgHandler();
	virtual ~CMsgHandler();
	BOOL	IsCreated();
	BOOL	Create();
	void	Close();
	void	SetEventInterface(CMsgHandlerEventInterface* pInterface);
	void	RaiseEvent(int nEvent,WPARAM wParam,LPARAM lParam);

protected:
	afx_msg LRESULT OnEventMessage(WPARAM wParam,LPARAM lParam);

private:
	CMsgHandlerEventInterface* m_pEventInterface;

	struct tagEventMsgData
	{
		int nEvent;
		WPARAM wParam;
		LPARAM lParam;
	};

#ifdef MsgHandle_Track_Memory
	std::map<tagEventMsgData* , BOOL> m_mapEventData; 
	CCriticalSection m_cs; 
#endif 

	DECLARE_MESSAGE_MAP()
};


