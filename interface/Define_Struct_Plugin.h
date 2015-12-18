#pragma once

#include "IQuote.h"
#include "ITrade.h" 

static const GUID IID_IFTQuoteData = 
{ 0xb75073e3, 0xaa3a, 0x4717, { 0xac, 0xa2, 0x11, 0x94, 0xa1, 0x3, 0x78, 0xc7 } };

static const GUID IID_IFTQuoteOperation = 
{ 0x9c65990c, 0x903, 0x4185, { 0x97, 0x12, 0x3e, 0xa7, 0xab, 0x34, 0xd, 0xc5 } };

static const GUID IID_IFTTrade = 
{ 0x69a88049, 0x252e, 0x4a12, { 0x83, 0x41, 0xdd, 0x4c, 0x6e, 0x84, 0x8b, 0x27 } };


/**
* 该插件接口要求的FTCore匹配版本号
*/
#define  FTCore_Support_Ver  101

interface IFTPluginCore 
{ 
	/**
	* Core提供的相关接口支持

	* @param uid  IID_IFTQuoteData 等
	*		

	* @return  插件的模块对象
	*/ 
	virtual void QueryFTInterface(REFIID uid, void** ppInterface) = 0; 
}; 


/**
* Dll 插件需要实现的接口对象 
*/ 
interface IFTPluginMoudle
{ 
	/**
	* 初始化及反初始化
	*/ 
	virtual void Init(IFTPluginCore* pPluginCore) = 0;  
	virtual void Uninit() = 0; 

	/**
	* 插件名称guid  
	*/ 
	virtual LPCWSTR	GetName() = 0;
	virtual GUID    GetGuid() = 0; 

	/**
	* 应用层控制是否显示插件窗口（如果插件有窗口) 
	*/
	virtual void 	ShowPlugin(bool bShow) = 0; 

	/*
	* 主程序事件通知时， 从Plugin得到回调接口 
	*/
	virtual void  GetPluginCallback_Quote(IQuoteInfoCallback** pCallback) = 0; 
	virtual void  GetPluginCallback_TradeHK(ITradeCallBack_HK** pCallback) = 0; 
}; 


/**
* 插件dll 导出接口"GetFTPluginMoudle"， 以便ftnn主程序能够加载该模块 

* @param nVerSupport该插件需要FTCore最低版本号,一般传回定义FTCore_Support_MinVer,
   如果过低,将不会加载该插件

* @return  插件的模块对象
*/
typedef IFTPluginMoudle*  (__stdcall* LPGetFTPluginMoudle)(int& nVerSupport); 


/************************************************************************/
/* 后记思考                                                                 */
/************************************************************************/

//目的及现状:
//1. 收集行情交易接口的真实需求, 以此作为彼此沟通的起点和开端 
//2. 如上的接口目前尚处于初级定义阶段，尚未进入开发阶段，欢迎多多提出意见 


//安全性：
//
//1. 插件身份验证
//2. 股票定阅的个数需要限制
//3. 交易接口的频率限制
//4. 插件加载个数限制 

//易用性
//
//1. 实现这套plugin 需要很熟悉vc ，对于python等其它语言的可能支持方案: 
//   实现一个plugin, 在内部开启一个简单的socket 服务, 转发相关数据或接口（必要可由futu来实现) 
//
//2. 直接以web的形式开放接口(时间上可能会更久一些)
// 









