#ifndef _MYWS_H_
#define _MYWS_H_

#include "suloki.h"
#include "ws.h"

/*
class MyWsService : public Suloki::Service
{
public:
	MyWsService();
	virtual ~MyWsService();
	//Service
	virtual Suloki::Ret Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName);
	virtual Suloki::Ret Start(void);
	virtual Suloki::Ret Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart);
	virtual Suloki::Ret Stop(void);
	virtual Suloki::Ret Clear(void);
};
*/
//handler
/*
class MyWsDispatcher : public Suloki::Dispatcher<Suloki::Message, Suloki::Context, Suloki::NoLockPolicy>
{
public:
	MyWsDispatcher();
	virtual ~MyWsDispatcher();
	//
	//void SetServiceInterface(Suloki::Service* pServiceInterface);
protected:
	virtual std::string CalKey(Suloki::SmartPtr<Suloki::Message> msgSmart);
	virtual Suloki::Ret HandleUnmatched(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart);
protected:
	Suloki::Ret Handler_System_Test(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart);
	Suloki::Ret Handler_Urc_Update(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart);
protected:
	//Suloki::Service* m_pServiceInterface;
};
*/
class MyWsSession : public session
{
	//typedef boost::shared_mutex RwLockPolicy;
	//typedef boost::unique_lock<RwLockPolicy>   WriteLock;
	//typedef boost::shared_lock<RwLockPolicy>   ReadLock;
public:
	explicit MyWsSession(tcp::socket&& socket);
	virtual ~MyWsSession();
	virtual Suloki::Ret Handle(std::string strReq, std::string& strRes);
	static Suloki::Ret MakeJsonstr(const boost::property_tree::ptree& jsonObj, std::string& strRes);
protected:
	virtual void HandleException(void);
	virtual void HandleConnectted(void);
protected:
	void SetGroupService(std::string strGroup, std::string strService)
	{
		//WriteLock lock(m_rwLock);
		m_strGroup = strGroup;
		m_strService = strService;
	}
	void GetGroupService(std::string& strGroup, std::string& strService)
	{
		//ReadLock lock(m_rwLock);
		strGroup = m_strGroup;
		strService = m_strService;
	}
	//????? not thread safe
	//RwLockPolicy m_rwLock;
	std::string m_strGroup;
	std::string m_strService;
};
typedef Suloki::Singleton< WsServer<MyWsSession> > MyWsServerSingleton;
class MyWsClientSession : public clientsession
{
public:
	explicit MyWsClientSession(net::io_context& ioc);
	virtual ~MyWsClientSession();
	virtual void HandleRead(std::string& strMsg);
protected:
	virtual void HandleConnectted(void);
};
#ifndef SULOKI_URC_SERVICE_SULOKIDEF
typedef Suloki::Singleton< WsClient<MyWsClientSession> > MyWsClientSingleton;
#else
class MyWsClient : public WsClient<MyWsClientSession>
{
public:
	MyWsClient();
	virtual ~MyWsClient();
	//StateMachine interface
	virtual Suloki::Ret Init(void);
	virtual Suloki::Ret Start(void);
	virtual Suloki::Ret Run(void);
	virtual Suloki::Ret Stop(void);
	virtual Suloki::Ret Clear(void);
private:
	void RunConnectedOtherUrc(void);
private:
	std::auto_ptr<boost::thread> m_threadConnectedOtherUrcSmart;
};
typedef Suloki::Singleton< MyWsClient > MyWsClientSingleton;
#endif

#endif