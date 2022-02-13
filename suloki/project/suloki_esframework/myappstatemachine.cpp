#include "myappstatemachine.h"
#include "myws.h"

MyAppStateMachine::MyAppStateMachine() :Suloki::AppStateMachine()
{}
MyAppStateMachine::~MyAppStateMachine()
{}
Suloki::Ret MyAppStateMachine::Init(void)
{
	if (Suloki::AppStateMachine::Init() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "Suloki::AppStateMachine::Init() error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		return Suloki::FAIL;
	}
	//
	if (MyServiceManagerSingleton::Instance().Init() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyServiceManagerSingleton::Instance().Init() error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		return Suloki::FAIL;
	}
	if (MyWsServerSingleton::Instance().Init() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyWsServerSingleton::Init() error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		return Suloki::FAIL;
	}
	if (MyWsClientSingleton::Instance().Init() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyWsClientSingleton::Init() error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		return Suloki::FAIL;
	}
	;
	//
	SetState(Suloki::StateMachine::INITED);
	return Suloki::SUCCESS;
}
Suloki::Ret MyAppStateMachine::Start(void)
{
	if (Suloki::AppStateMachine::Start() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "Suloki::AppStateMachine::Start() error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		return Suloki::FAIL;
	}
	//
	if (MyServiceManagerSingleton::Instance().Start() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyServiceManagerSingleton::Instance().Start() error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		return Suloki::FAIL;
	}
	if (MyWsServerSingleton::Instance().Start() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyWsServerSingleton::Instance().Start error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		return Suloki::FAIL;
	}
	if (MyWsClientSingleton::Instance().Start() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyWsClientSingleton::Instance().Start error", Suloki::LOG_ERROR_LEVEL);
		Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
		return Suloki::FAIL;
	}
	;
	//
	SetState(Suloki::StateMachine::STARTED);
	return Suloki::SUCCESS;
}
Suloki::Ret MyAppStateMachine::Run(void)
{
	return Suloki::AppStateMachine::Run();
}
Suloki::Ret MyAppStateMachine::Stop(void)
{
	SetState(Suloki::StateMachine::STOPPED);
	//
	;
	MyWsClientSingleton::Instance().Stop();
	MyWsServerSingleton::Instance().Stop();
	MyServiceManagerSingleton::Instance().Stop();
	//
	Suloki::AppStateMachine::Stop();
	return Suloki::SUCCESS;
}
Suloki::Ret MyAppStateMachine::Clear(void)
{
	SetState(Suloki::StateMachine::CLEARED);
	//
	;
	MyWsClientSingleton::Instance().Clear();
	MyWsClientSingleton::Deinstance();
	Suloki::Time::Sleep(Suloki::Time::TIMEOUT);
	MyWsServerSingleton::Instance().Clear();
	MyWsServerSingleton::Deinstance();
	MyServiceManagerSingleton::Instance().Clear();
	MyServiceManagerSingleton::Deinstance();
	//
	Suloki::AppStateMachine::Clear();
	return Suloki::SUCCESS;
}
