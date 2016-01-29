#include "stdafx.h"
#include "PluginModule.h"
#include "Protocol/ProtoParseBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EVENT_ID_ON_RECEIVE_DATA	984
#define EVENT_ID_ON_SEND_DATA		985



//////////////////////////////////////////////////////////////////////////
//todo:每个工程要重新产生一个GUID
GUID PLUGIN_GUID = 
{ 0x70386302, 0xa1f1, 0x4395, { 0x8c, 0x76, 0x4f, 0x89, 0x66, 0x2d, 0x54, 0xe } };


//////////////////////////////////////////////////////////////////////////

static CPluginModule *g_pPluginModule = NULL;

IFTPluginMoudle*  __stdcall GetFTPluginMoudle(int& nVerSupport)
{
	if ( g_pPluginModule == NULL )
	{
		g_pPluginModule = new CPluginModule;
		ASSERT(g_pPluginModule != NULL);
	}

	nVerSupport = FTCore_Support_Ver;
	return g_pPluginModule;
}

void ReleasePluginMoudle()
{
	if ( g_pPluginModule != NULL )
	{
		delete g_pPluginModule;
		g_pPluginModule = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////

CPluginModule::CPluginModule()
{
	m_pPluginCore = NULL;
}

CPluginModule::~CPluginModule()
{
	Uninit();
}

void CPluginModule::Init(IFTPluginCore* pPluginCore)
{
	if ( m_pPluginCore != NULL )
		return;

	if ( pPluginCore == NULL )
	{
		ASSERT(false);
		return;
	}

	m_pPluginCore = pPluginCore;	
	m_MsgHandler.Create();
	m_MsgHandler.SetEventInterface(this);

	m_strRecvBuf.reserve(1024*10);
	m_Network.InitNetwork(this);

	m_QuoteServer.InitQuoteSvr(pPluginCore, &m_Network);
	m_HKTradeServer.InitTradeSvr(pPluginCore, &m_Network);
	m_USTradeServer.InitTradeSvr(pPluginCore, &m_Network);
}

void CPluginModule::Uninit()
{
	if ( m_pPluginCore )
	{
		m_pPluginCore = NULL;
		m_QuoteServer.UninitQuoteSvr();
		m_HKTradeServer.UninitTradeSvr();

		m_Network.UninitNetwork();		
		m_MsgHandler.Close();
		m_MsgHandler.SetEventInterface(NULL);
	}
}

LPCWSTR	CPluginModule::GetName()
{
	return L"插件服务器";
}

GUID    CPluginModule::GetGuid()
{
	return PLUGIN_GUID;
}

void 	CPluginModule::ShowPlugin(bool bShow)
{
	AfxMessageBox(L"服务器插件对话框");
}

void  CPluginModule::GetPluginCallback_Quote(IQuoteInfoCallback** pCallback) 
{
	if ( pCallback == NULL )
	{
		ASSERT(false);
		return ;
	}

	*pCallback = &m_QuoteServer;
}

void  CPluginModule::GetPluginCallback_TradeHK(ITradeCallBack_HK** pCallback)
{
	if ( pCallback == NULL )
	{
		ASSERT(false);
		return ;
	}

	*pCallback = &m_HKTradeServer;
}

void  CPluginModule::GetPluginCallback_TradeUS(ITradeCallBack_US** pCallback)
{
	if ( pCallback == NULL )
	{
		ASSERT(false);
		return ;
	}

	*pCallback = &m_USTradeServer;
}

void CPluginModule::OnReceive(SOCKET sock)
{
	m_MsgHandler.RaiseEvent(EVENT_ID_ON_RECEIVE_DATA, sock, 0);
}

void CPluginModule::OnSend(SOCKET sock)
{
	m_MsgHandler.RaiseEvent(EVENT_ID_ON_SEND_DATA, 0, 0);
}

void CPluginModule::OnMsgEvent(int nEvent,WPARAM wParam,LPARAM lParam)
{
	if ( EVENT_ID_ON_RECEIVE_DATA == nEvent )
	{
		OnRecvNetData(wParam);
	}
}

void CPluginModule::OnRecvNetData(SOCKET sock)
{
	CHECK_RET(sock != INVALID_SOCKET, NORET);

	const char *pBuf = NULL; 
	int nBufLen = 0;
	while ( m_Network.GetRecvData(sock, pBuf, nBufLen) && pBuf && nBufLen )
	{
		int nFirstValidCh = -1;
		int nLastValidCh = -1;
		for ( int n = 0; n < nBufLen; n++ )
		{
			char ch = pBuf[n];
			if ( ch == 0xd || ch == 0xa )
			{
				if ( nFirstValidCh != -1 && nLastValidCh != -1 )
				{					
					int nPreDataSize = (int)m_strRecvBuf.size();
					int nNewDataNum = nLastValidCh - nFirstValidCh + 1;
					m_strRecvBuf.resize(nPreDataSize + nNewDataNum);
					memcpy(&m_strRecvBuf[0] + nPreDataSize, pBuf + nFirstValidCh, nNewDataNum);
					
					//{"a":"b"}
					if ( m_strRecvBuf.size() < 10 ) 
					{
						CHECK_OP(false, NOOP);
					}
					else
					{
						ParseRecvData(sock, &m_strRecvBuf[0], (int)m_strRecvBuf.size());
					}

					m_strRecvBuf.clear();
					nFirstValidCh = -1;
					nLastValidCh = -1;
				}
				continue;
			}

			if ( nFirstValidCh == -1 )
			{
				nFirstValidCh = n;
			}

			nLastValidCh = n;
		}

		if ( nFirstValidCh >= 0 && nLastValidCh >= nFirstValidCh )
		{	
			int nPreDataSize = (int)m_strRecvBuf.size();
			int nUnparseNum = nLastValidCh - nFirstValidCh + 1;			
			m_strRecvBuf.resize(nPreDataSize + nUnparseNum);
			memcpy(&m_strRecvBuf[0] + nPreDataSize, pBuf + nFirstValidCh, nUnparseNum);
		}
	}
}

void CPluginModule::ParseRecvData(SOCKET sock, char *pBuf, int nBufLen)
{
	CHECK_RET(pBuf && nBufLen > 2, NORET);

	Json::Value jsnVal;
	bool bRet = CProtoParseBase::ConvBuffer2Json(pBuf, nBufLen, jsnVal);
	CHECK_RET(bRet, NORET);

	int nCmdID = CProtoParseBase::GetProtoID(jsnVal);
	if ( nCmdID >= PROTO_ID_QUOTE_MIN && nCmdID <= PROTO_ID_QUOTE_MAX )
	{
		m_QuoteServer.SetQuoteReqData(nCmdID, jsnVal, sock);
	}
	else if ( nCmdID >= PROTO_ID_TRADE_HK_MIN && nCmdID <= PROTO_ID_TRADE_HK_MAX )
	{
		m_HKTradeServer.SetTradeReqData(nCmdID, jsnVal, sock);
	}
	else if ( nCmdID >= PROTO_ID_TRADE_US_MIN && nCmdID <= PROTO_ID_TRADE_US_MAX )
	{
		m_USTradeServer.SetTradeReqData(nCmdID, jsnVal, sock);
	}
	else
	{
		CHECK_OP(false, NOOP);
	}
}