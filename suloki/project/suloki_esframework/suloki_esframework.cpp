// suloki_esframework.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "suloki.h"
#include "myappstatemachine.h"

int main()
{
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
	char cName[256 + 2];
	::GetModuleFileName(NULL, cName, 256);
	std::string strName(cName);
	size_t pos = strName.rfind("\\");
	strName = strName.substr(0, pos + 1);
	//Suloki::GlobalStateMachineSingleton::Instance().SetModulePath(strName);
#else
	char cName[256 + 2];
	const char* pSysFile = "/proc/self/exe";
	memset(cName, 0, 256 + 2);
	readlink(pSysFile, cName, 256);
	std::string strName(cName);
	size_t pos = strName.rfind("/");
	strName = strName.substr(0, pos + 1);
	//Suloki::GlobalStateMachineSingleton::Instance().SetModulePath(strName);
#endif
	if (MyAppStateMachineSingleton::Instance().Init() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyAppStateMachineSingleton::Instance().Init() error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT + Suloki::Time::TIMEOUT);
		return -1;
	}
	if (MyAppStateMachineSingleton::Instance().Start() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyAppStateMachineSingleton::Instance().Start() error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT + Suloki::Time::TIMEOUT);
		return -1;
	}
	//
#ifdef SULOKI_DEBUG_CODE_SULOKIDEF
#ifdef SULOKI_DUMP_SULOKIDEF
	//Suloki::Dump::TestDump();
#endif
#endif
	//
	MyAppStateMachineSingleton::Instance().Run();
	//
	MyAppStateMachineSingleton::Instance().Stop();
	MyAppStateMachineSingleton::Instance().Clear();
	//
	MyAppStateMachineSingleton::Deinstance();
	//
	return 0;
}
