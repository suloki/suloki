#ifndef _MYHANDLER_H_
#define _MYHANDLER_H_

#include "suloki.h"

//handler
class MyDispatcher : public Suloki::Dispatcher<Suloki::Message, Suloki::Context>//, Suloki::NoLockPolicy>
{
public:
	MyDispatcher();
	virtual ~MyDispatcher();
	//
	void SetServiceInterface(Suloki::Service* pServiceInterface);
protected:
	virtual std::string CalKey(Suloki::SmartPtr<Suloki::Message> msgSmart);
	virtual Suloki::Ret HandleUnmatched(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart);
protected:
	Suloki::Ret Handler_System_Test(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	Suloki::Ret Handler_System_RegService(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	Suloki::Ret Handler_System_Wsconnected(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	Suloki::Ret Handler_Urc_Add(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	Suloki::Ret Handler_Urc_Del(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	Suloki::Ret Handler_Urc_Update(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	Suloki::Ret Handler_Urc_Get(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	Suloki::Ret Handler_Auth_Login(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	Suloki::Ret Handler_Ai_Work(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	Suloki::Ret Handler_Ai_Wenzizuobiao(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
protected:
	Suloki::Service* m_pServiceInterface;
};
class MainService : public Suloki::Service
{
public:
	MainService();
	virtual ~MainService();
	//Service
	virtual Suloki::Ret Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName);
	virtual Suloki::Ret Start(void);
	virtual Suloki::Ret Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart);
	virtual Suloki::Ret Stop(void);
	virtual Suloki::Ret Clear(void);
protected:
	MyDispatcher m_handler;
};
typedef Suloki::Singleton<MainService> MyHandlerMachineSingleton;

#endif