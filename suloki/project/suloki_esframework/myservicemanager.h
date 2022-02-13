#ifndef _MYSERVICEMANAGER_H_
#define _MYSERVICEMANAGER_H_

#include "suloki.h"
#include "jshandler.h"

#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
#else
#include <dlfcn.h>
#endif

#ifdef SULOKI_MODULEDLL_SULOKIDEF
#else
#include "myhandler.h"
#endif

#ifdef SULOKI_MODULEDLL_SULOKIDEF
class MainService : public Suloki::Service
{
public:
	MainService();
	virtual ~MainService();
	//Service
	virtual Suloki::Ret Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName);
	virtual Suloki::Ret Start(void);
	virtual Suloki::Ret Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::SmartPtr<Suloki::Context>& paraOutSmart);
	virtual Suloki::Ret Stop(void);
	virtual Suloki::Ret Clear(void);
protected:
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
	HINSTANCE m_hinstLib;
#else
	void* m_hinstLib;
#endif
	Suloki::ServiceInit m_MyInit;
	Suloki::ServiceStart m_MyStart;
	Suloki::ServiceHandle m_MyHandle;
	Suloki::ServiceStop m_MyStop;
	Suloki::ServiceClear m_MyClear;
};
#endif

class JsMainService : public Suloki::Service
{
public:
	JsMainService();
	virtual ~JsMainService();
	//Service
	virtual Suloki::Ret Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName);
	virtual Suloki::Ret Start(void);
	virtual Suloki::Ret Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::SmartPtr<Suloki::Context>& paraOutSmart);
	virtual Suloki::Ret Stop(void);
	virtual Suloki::Ret Clear(void);
protected:
	//JsHandler m_jsHandler;
	Suloki::Queue<JsHandler> m_jsHandlerQueue;
};

class MyServiceManager : public Suloki::ServiceManager
{
public:
	MyServiceManager();
	virtual ~MyServiceManager();
	//StateMachine interface
	virtual Suloki::Ret Init(void);
	virtual Suloki::Ret Start(void);
	virtual Suloki::Ret Run(void);
	virtual Suloki::Ret Stop(void);
	virtual Suloki::Ret Clear(void);
private:
	MyServiceManager(MyServiceManager& ref) {}
	MyServiceManager& operator=(MyServiceManager& ref) { return *this; }
protected:
};
typedef Suloki::Singleton< MyServiceManager > MyServiceManagerSingleton;

#endif
