#pragma once

#pragma warning(disable:4786)
#pragma warning(disable:4284)
#pragma warning(disable:4127)


#include <assert.h>
#include <string>

#pragma warning( disable : 4505)

#define _CMHELPER_BEGIN	namespace CMHelper {
#define _CMHELPER_END	};

_CMHELPER_BEGIN 

_CMHELPER_END 

// ºê¶¨Òå
#define NORET
#define NOOP

#if defined(_WIN32) && defined(_DEBUG)
	static bool CMAssertFailedLine(const char* szFileName, int nLine)
	{
		MSG msg;
		BOOL bQuit = ::PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
		BOOL bResult = _CrtDbgReport(_CRT_ASSERT, szFileName, nLine, NULL, NULL);
		if (bQuit)
		{
			::PostQuitMessage((int)msg.wParam);
		}

		return !!bResult;
	}
	#define CM_Assert (!CMAssertFailedLine(__FILE__, __LINE__) || (_CrtDbgBreak(), 0))
#else
	static bool CMAssertFailedLine(const char* szFileName, int nLine)
	{
		return true;
	}
	#define CM_Assert
#endif


#define CM_AssertEx(b) if (!(b)) {CM_Assert;}


#define CHECK_RET(p, r) do{ \
		if(!(p)) { \
			CM_AssertEx(false); \
			return r; \
		} \
	}while(0)

#define COMCHECK_RET(p, r) do{ \
		if(FAILED(p)) { \
			CM_AssertEx(false); \
			return r; \
		} \
	}while(0)

#define CHECK_OP(p, op) { \
		if(!(p)) { \
			CM_AssertEx(false); \
			op; \
		} \
	}

#define COMCHECK_OP(p, op) { \
		if(FAILED(p)) { \
			CM_AssertEx(false); \
			op; \
		} \
	}

#define SAFE_DELETE(p) do{ \
		if(p) { \
			delete (p); \
			(p) = NULL; \
		} \
	}while(0)

#define SAFE_DELETE_ARR(p) do{ \
		if(p) { \
			delete [] (p); \
			(p) = NULL; \
		} \
	}while(0)

#define SAFE_RELEASE(p)	do{ \
		if(p) { \
			(p)->Release(); \
			(p) = NULL; \
		} \
	}while(0)

#define SAFE_DELETEGDIOBJ(obj) do{ \
		if(obj.GetSafeHandle()) { \
			obj.DeleteObject(); \
		} \
	}while(0)

#define SAFE_DELETE_OBJECT(p) do{ \
		if(p) { \
			DeleteObject(p); \
			p = NULL; \
		} \
	}while(0)

#define SAFE_CLOSEHANDLE(h) do{ \
		if(h) { \
			CloseHandle(h); \
			h = NULL; \
		} \
	}while(0)