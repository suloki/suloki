#include "myservicemanager.h"

#ifdef SULOKI_MODULEDLL_SULOKIDEF
#else
#include "myhandler.cpp"
#endif

#ifdef SULOKI_MODULEDLL_SULOKIDEF
MainService::MainService()
{
	std::string groupname;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_GROUPNAME_KEY, groupname);
	std::string servicename;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_SERVICENAME_KEY, servicename);
	//Suloki::Service::Init(&Suloki::UrcSingleton::Instance(), Suloki::URC_MAINGROUP_SERVICE, Suloki::URC_MAINSERVICE_SERVICE);
	Suloki::Service::Init(&Suloki::UrcSingleton::Instance(), groupname, servicename);
}
MainService::~MainService()
{}
Suloki::Ret MainService::Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName)
{
	//Suloki::Ret ret = Suloki::Service::Init(pUrcInterface, groupName, serviceName);
	//if (ret != Suloki::SUCCESS)
	//	return ret;
	//
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
	std::string strPath = Suloki::GlobalStateMachineSingleton::Instance().GetModulePath();
	strPath += "dll/mainservice.dll";
	m_hinstLib = LoadLibrary(strPath.c_str());
#else
	std::string strPath = Suloki::GlobalStateMachineSingleton::Instance().GetModulePath();
	strPath += "dll/libmainservice.so";
	m_hinstLib = dlopen(strPath.c_str(), RTLD_LAZY);
#endif
	if (m_hinstLib != NULL)
	{
		try {
			SULOKI_GETPROCADDR_SULOKIDEF(m_MyInit, Suloki::ServiceInit, "ServiceInit", true);
			SULOKI_GETPROCADDR_SULOKIDEF(m_MyStart, Suloki::ServiceStart, "ServiceStart", true);
			SULOKI_GETPROCADDR_SULOKIDEF(m_MyHandle, Suloki::ServiceHandle, "ServiceHandle", true);
			SULOKI_GETPROCADDR_SULOKIDEF(m_MyStop, Suloki::ServiceStop, "ServiceStop", true);
			SULOKI_GETPROCADDR_SULOKIDEF(m_MyClear, Suloki::ServiceClear, "ServiceClear", true);
		}catch(...){
			return Suloki::FAIL;
		}
	}
	Suloki::Ret ret = (m_MyInit)(pUrcInterface, groupName, serviceName);
return ret;
}
Suloki::Ret MainService::Start(void)
{
	return (m_MyStart)();
}
Suloki::Ret MainService::Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::SmartPtr<Suloki::Context>& paraOutSmart)
{
	return (m_MyHandle)(msgSmart, paraSmart, resSmart, paraOutSmart);
}
Suloki::Ret MainService::Stop(void)
{
	return (m_MyStop)();
}
Suloki::Ret MainService::Clear(void)
{
	(m_MyClear)();
	//
	if (m_hinstLib != NULL)
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
		FreeLibrary(m_hinstLib);
#else
		dlclose(m_hinstLib);
#endif
	return Suloki::SUCCESS;
}
#endif

JsMainService::JsMainService()
{
	Suloki::Service::Init(&Suloki::UrcSingleton::Instance(), Suloki::URC_MAINGROUP_SERVICE, Suloki::URC_MAINSERVICE_SERVICE);
}
JsMainService::~JsMainService()
{}
Suloki::Ret JsMainService::Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName)
{
	//Suloki::Ret ret = Suloki::Service::Init(pUrcInterface, groupName, serviceName);
	//if (ret != Suloki::SUCCESS)
	//	return ret;
	std::string strThreadsVal;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_SERVICETHREADS_KEY, strThreadsVal);
	//if (m_jsHandler.Init(pUrcInterface, groupName, serviceName) != Suloki::SUCCESS)
	//	return Suloki::FAIL;
	for (Suloki::Int i = 0; i < atoi(strThreadsVal.c_str()); i++)
	{
		Suloki::SmartPtr<JsHandler> jsHandlerSmart(new JsHandler());
		if (jsHandlerSmart.IsNULL())
			return Suloki::FAIL;
		m_jsHandlerQueue.Push(jsHandlerSmart);
	}
	//
	Suloki::SmartPtr<JsHandler> jsHandlerSmart = m_jsHandlerQueue.Pop();
	/*
	JsHandler* pJsHandler = jsHandlerSmart.get();
	while (true)
	{
		if (jsHandlerSmart->Init(pUrcInterface, groupName, serviceName) != Suloki::SUCCESS)
			return Suloki::FAIL;
		m_jsHandlerQueue.Push(jsHandlerSmart);
		jsHandlerSmart = m_jsHandlerQueue.Pop();
		if (pJsHandler == jsHandlerSmart.get())
		{
			m_jsHandlerQueue.Push(jsHandlerSmart);
			break;
		}
	}
	*/
	Suloki::SmartPtr<JsHandler> firstSmart = jsHandlerSmart;
	while (true)
	{
		if (jsHandlerSmart->Init(pUrcInterface, groupName, serviceName) != Suloki::SUCCESS)
			return Suloki::FAIL;
		m_jsHandlerQueue.Push(jsHandlerSmart);
		jsHandlerSmart = m_jsHandlerQueue.Pop();
		if (firstSmart.IsOne(jsHandlerSmart))
		{
			m_jsHandlerQueue.Push(jsHandlerSmart);
			break;
		}
	}
	//
	return Suloki::SUCCESS;
}
Suloki::Ret JsMainService::Start(void)
{
	Suloki::SmartPtr<JsHandler> jsHandlerSmart = m_jsHandlerQueue.Pop();
	/*
	JsHandler* pJsHandler = jsHandlerSmart.get();
	while (true)
	{
		if (jsHandlerSmart->Start() != Suloki::SUCCESS)
			return Suloki::FAIL;
		m_jsHandlerQueue.Push(jsHandlerSmart);
		jsHandlerSmart = m_jsHandlerQueue.Pop();
		if (pJsHandler == jsHandlerSmart.get())
		{
			m_jsHandlerQueue.Push(jsHandlerSmart);
			break;
		}
	}
	*/
	Suloki::SmartPtr<JsHandler> firstSmart = jsHandlerSmart;
	while (true)
	{
		if (jsHandlerSmart->Start() != Suloki::SUCCESS)
			return Suloki::FAIL;
		m_jsHandlerQueue.Push(jsHandlerSmart);
		jsHandlerSmart = m_jsHandlerQueue.Pop();
		if (firstSmart.IsOne(jsHandlerSmart))
		{
			m_jsHandlerQueue.Push(jsHandlerSmart);
			break;
		}
	}
#ifdef SULOKI_DEBUG_CODE_SULOKIDEF
	/*
	for (int i = 0; i < 1; i++)
	{
		boost::property_tree::ptree jsonMsg;
		Suloki::Message::MakeJsonMsg(jsonMsg);
		try {
			jsonMsg.put<std::string>(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF, "test post message to main service async");
		}
		catch (boost::property_tree::ptree_error pt) {
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "handle json exception", Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		catch (...) {
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "handle json exception1", Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(jsonMsg));
		std::vector<std::string> context;
		Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
		Suloki::UrcSingleton::Instance().PostMsgToMainService(msgSmart, contextSmart);
	}
	*/
#endif
	return Suloki::SUCCESS;
}
Suloki::Ret JsMainService::Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::SmartPtr<Suloki::Context>& paraOutSmart)
{
	Suloki::SmartPtr<JsHandler> jsHandlerSmart = m_jsHandlerQueue.Pop();
	//if(jsHandlerSmart.IsNULL())
	//	return Suloki::SUCCESS;
	jsHandlerSmart->Handle(msgSmart, paraSmart, resSmart, paraOutSmart);
	m_jsHandlerQueue.Push(jsHandlerSmart);
	return Suloki::SUCCESS;
}
Suloki::Ret JsMainService::Stop(void)
{
	Suloki::SmartPtr<JsHandler> jsHandlerSmart = m_jsHandlerQueue.Pop();
	//JsHandler* pJsHandler = jsHandlerSmart.get();
	Suloki::SmartPtr<JsHandler> firstSmart = jsHandlerSmart;
	while (true)
	{
		if (jsHandlerSmart->Stop() != Suloki::SUCCESS)
			return Suloki::FAIL;
		m_jsHandlerQueue.Push(jsHandlerSmart);
		jsHandlerSmart = m_jsHandlerQueue.Pop();
		//if (pJsHandler == jsHandlerSmart.get())
		if (firstSmart.IsOne(jsHandlerSmart))
		{
			m_jsHandlerQueue.Push(jsHandlerSmart);
			break;
		}
	}
	//
	return Suloki::SUCCESS;
}
Suloki::Ret JsMainService::Clear(void)
{
	Suloki::SmartPtr<JsHandler> jsHandlerSmart = m_jsHandlerQueue.Pop();
	//JsHandler* pJsHandler = jsHandlerSmart.get();
	Suloki::SmartPtr<JsHandler> firstSmart = jsHandlerSmart;
	while (true)
	{
		if (jsHandlerSmart->Clear() != Suloki::SUCCESS)
			return Suloki::FAIL;
		m_jsHandlerQueue.Push(jsHandlerSmart);
		jsHandlerSmart = m_jsHandlerQueue.Pop();
		//if (pJsHandler == jsHandlerSmart.get())
		if (firstSmart.IsOne(jsHandlerSmart))
		{
			m_jsHandlerQueue.Push(jsHandlerSmart);
			break;
		}
	}
	//
	return Suloki::SUCCESS;
}

MyServiceManager::MyServiceManager() {}
MyServiceManager::~MyServiceManager() {}
Suloki::Ret MyServiceManager::Init(void)
{
	std::string strServiceTypeVal;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_SERVICETYPE_KEY, strServiceTypeVal);
	//
	Suloki::SmartPtr< Suloki::Service > serviceSmart;
	if (strServiceTypeVal.compare(Suloki::CONFIG_CPLUSPLUS_SERVICETYPE_VAL) == 0)
	{
		Suloki::SmartPtr< MainService > mainserviceSmart(new MainService());
		if (mainserviceSmart.IsNULL())
		{
			return Suloki::NOMEMORY_ERRCODE;
		}
		serviceSmart = mainserviceSmart.Downcast<Suloki::Service>();
	}
	else	
	{
		Suloki::SmartPtr< JsMainService > mainserviceSmart(new JsMainService());
		if (mainserviceSmart.IsNULL())
		{
			return Suloki::NOMEMORY_ERRCODE;
		}
		serviceSmart = mainserviceSmart.Downcast<Suloki::Service>();
	}
	AddServide(Suloki::URC_MAINGROUP_SERVICE, Suloki::URC_MAINSERVICE_SERVICE, serviceSmart);
	//
	return Suloki::ServiceManager::Init();
}
Suloki::Ret MyServiceManager::Start(void)
{
	return Suloki::ServiceManager::Start();
}
Suloki::Ret MyServiceManager::Run(void)
{
	return Suloki::ServiceManager::Run();
}
Suloki::Ret MyServiceManager::Stop(void)
{
	return Suloki::ServiceManager::Stop();
}
Suloki::Ret MyServiceManager::Clear(void)
{
	Suloki::ServiceManager::Clear();
	//
	Suloki::SmartPtr< Suloki::Service > serviceSmart;
	RemoveServide(Suloki::URC_MAINGROUP_SERVICE, Suloki::URC_MAINSERVICE_SERVICE, serviceSmart);
	//
	return Suloki::SUCCESS;
}
