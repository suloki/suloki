// mainservice.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "mainservice.h"
#include "myhandler.h"
#include "myappstatemachine.h"

MAINSERVICE_API Suloki::Ret ServiceInit(SULOKI_IN_SULOKIDEF Suloki::UrcInterface* pUrcInterface, SULOKI_IN_SULOKIDEF std::string groupName, SULOKI_IN_SULOKIDEF std::string  serviceName)
{
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
	char cName[256 + 2];
	::GetModuleFileName(NULL, cName, 256);
	std::string strName(cName);
	size_t pos = strName.rfind("\\");
	strName = strName.substr(0, pos + 1);
	strName += "dll/";
	Suloki::GlobalStateMachineSingleton::Instance().SetModulePath(strName);
#else
	char cName[256 + 2];
	const char* pSysFile = "/proc/self/exe";
	memset(cName, 0, 256 + 2);
	readlink(pSysFile, cName, 256);
	std::string strName(cName);
	size_t pos = strName.rfind("/");
	strName = strName.substr(0, pos + 1);
	strName += "dll/";
	Suloki::GlobalStateMachineSingleton::Instance().SetModulePath(strName);
#endif
	if (MyAppStateMachineSingleton::Instance().Init() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyAppStateMachineSingleton::Instance().Init() error", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	if (MyHandlerMachineSingleton::Instance().Init(pUrcInterface, groupName, serviceName) != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyHandlerMachineSingleton::Instance().Init error", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "ServiceInit ok", Suloki::LOG_DEBUG_LEVEL);
	return Suloki::SUCCESS;
}
MAINSERVICE_API Suloki::Ret ServiceStart(void)
{
	if (MyAppStateMachineSingleton::Instance().Start() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyAppStateMachineSingleton::Instance().Start() error", Suloki::LOG_ERROR_LEVEL);
		return -1;
	}
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "ServiceStart ok", Suloki::LOG_DEBUG_LEVEL);
	return Suloki::SUCCESS;
}
MAINSERVICE_API Suloki::Ret ServiceHandle(SULOKI_IN_SULOKIDEF Suloki::SmartPtr<Suloki::Message> msgSmart, SULOKI_IN_SULOKIDEF Suloki::SmartPtr<Suloki::Context> paraSmart)
{
	return MyHandlerMachineSingleton::Instance().Handle(msgSmart, paraSmart);
}
MAINSERVICE_API Suloki::Ret ServiceStop(void)
{
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "ServiceStop will do", Suloki::LOG_DEBUG_LEVEL);
	//
	MyAppStateMachineSingleton::Instance().Stop();
	return Suloki::SUCCESS;
}
MAINSERVICE_API Suloki::Ret ServiceClear(void)
{
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "ServiceClear will do", Suloki::LOG_DEBUG_LEVEL);
	//
	MyAppStateMachineSingleton::Instance().Clear();
	MyAppStateMachineSingleton::Deinstance();
	return Suloki::SUCCESS;
}

/*
// 这是导出变量的一个示例
MAINSERVICE_API int nmainservice=0;

// 这是导出函数的一个示例。
MAINSERVICE_API int fnmainservice(void)
{
    return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 mainservice.h
Cmainservice::Cmainservice()
{
    return;
}
*/