#include "myappstatemachine.h"
//#include "myws.h"

MyAppStateMachine::MyAppStateMachine() :Suloki::AppStateMachine()
{}
MyAppStateMachine::~MyAppStateMachine()
{}
Suloki::Ret MyAppStateMachine::Init(void)
{
	if (Suloki::AppStateMachine::Init() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "Suloki::AppStateMachine::Init() error", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	;
	//
	SetState(Suloki::INIT_STATEMACHINE);
	return Suloki::SUCCESS;
}
Suloki::Ret MyAppStateMachine::Start(void)
{
	if (Suloki::AppStateMachine::Start() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "Suloki::AppStateMachine::Start() error", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	if (MyHandlerMachineSingleton::Instance().Start() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "MyHandlerMachineSingleton::Instance().Start error", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	;
	//
	SetState(Suloki::START_STATEMACHINE);
	return Suloki::SUCCESS;
}
Suloki::Ret MyAppStateMachine::Run(void)
{
	return Suloki::AppStateMachine::Run();
}
Suloki::Ret MyAppStateMachine::Stop(void)
{
	SetState(Suloki::STOP_STATEMACHINE);
	//
	;
	MyHandlerMachineSingleton::Instance().Stop();
	//
	Suloki::AppStateMachine::Stop();
	return Suloki::SUCCESS;
}
Suloki::Ret MyAppStateMachine::Clear(void)
{
	SetState(Suloki::CLEAR_STATEMACHINE);
	//
	;
	MyHandlerMachineSingleton::Instance().Clear();
	MyHandlerMachineSingleton::Deinstance();
	//
	Suloki::AppStateMachine::Clear();
	return Suloki::SUCCESS;
}
