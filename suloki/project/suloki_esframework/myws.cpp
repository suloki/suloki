#include "myws.h"
//#include "myhandler.h"
#include "myservicemanager.h"

/*
MyWsService::MyWsService()
{
	Suloki::Service::Init(&Suloki::UrcSingleton::Instance(), Suloki::URC_MAINGROUP_SERVICE, Suloki::URC_MAINSERVICE_SERVICE);
}
MyWsService::~MyWsService()
{}
Suloki::Ret MyWsService::Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName)
{
	Suloki::Ret ret = Suloki::Service::Init(pUrcInterface, groupName, serviceName);
	if (ret != Suloki::SUCCESS)
		return ret;
	//
	return Suloki::SUCCESS;
}
Suloki::Ret MyWsService::Start(void)
{
	return Suloki::SUCCESS;
}
Suloki::Ret MyWsService::Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart)
{
	return Suloki::SUCCESS;
}
Suloki::Ret MyWsService::Stop(void)
{
	return Suloki::SUCCESS;
}
Suloki::Ret MyWsService::Clear(void)
{
	return Suloki::SUCCESS;
}
*/
/*
MyWsDispatcher::MyWsDispatcher()// :m_pServiceInterface(NULL)
{
	{
		std::stringstream sStream;
		sStream << SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_SYSTEM_TEST_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyWsDispatcher::Handler_System_Test);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_URC_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_URC_UPDATE_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyWsDispatcher::Handler_Urc_Update);
	}
}
MyWsDispatcher::~MyWsDispatcher()
{}
//void MyWsDispatcher::SetServiceInterface(Suloki::Service* pServiceInterface)
//{
//	m_pServiceInterface = pServiceInterface;
//}
std::string MyWsDispatcher::CalKey(Suloki::SmartPtr<Suloki::Message> msgSmart)
{
	std::stringstream sStream;
	sStream << msgSmart->GetField(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF) << "_" << msgSmart->GetField(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF);
	return sStream.str();// protocolReq.m_msgKey;
}
Suloki::Ret MyWsDispatcher::HandleUnmatched(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart)
{
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "have msg HandleUnmatched error", Suloki::LOG_WARN_LEVEL);
	return Suloki::FAIL;
}
Suloki::Ret MyWsDispatcher::Handler_System_Test(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart)
{
	//Suloki::UrcInterface* pUrcInterface;
	//m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	//
	std::string testContext = msgSmart->GetField("testcontext");
	//
	std::stringstream strStream;
	strStream << "MainService's MyWsDispatcher recv msg.msg constent:" << testContext;
	SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_DEBUG_LEVEL);
	return Suloki::SUCCESS;
}
Suloki::Ret MyWsDispatcher::Handler_Urc_Update(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart)
{
	//Suloki::UrcInterface* pUrcInterface;
	//m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	//
	std::string strUrName = msgSmart->GetField(SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF);
	//
	std::stringstream strStream;
	strStream << "MainService's MyWsDispatcher recv update msg.urname:" << strUrName;
	SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_DEBUG_LEVEL);
	return Suloki::SUCCESS;
}
*/
MyWsSession::MyWsSession(tcp::socket&& socket)
	: session(std::move(socket))
{
}
MyWsSession::~MyWsSession()
{
}
Suloki::Ret MyWsSession::Handle(std::string strReq, std::string& strRes)
{
	//Suloki::Ret ret = Suloki::SUCCESS;
	std::stringstream strStream;
	strStream << strReq;
	boost::property_tree::ptree root;
	try {
		boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(strStream, root);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << Suloki::GetErrStr(Suloki::PARSEJSON_ERRCODE) << strReq;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << Suloki::GetErrStr(Suloki::PARSEJSON_ERRCODE) << strReq;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	if (Suloki::Message::CheckJsonMsg(root) != Suloki::SUCCESS)
	{
		std::stringstream strStream;
		strStream << Suloki::GetErrStr(Suloki::INVALIDMSG_ERRCODE) << strReq;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		root.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		root.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(Suloki::INVALIDMSG_ERRCODE));
		root.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(Suloki::INVALIDMSG_ERRCODE));
		MakeJsonstr(root, strRes);
		return Suloki::SUCCESS;
	}
	/*
	try {
		std::string strBusinessid = root.get<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF);
		std::string strMessageid = root.get<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF);
		boost::property_tree::ptree body = root.get_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF);
		std::string strGroup = body.get<std::string>(SULOKI_GROUP_MSGFIELD_KEY_SULOKIDEF);
		std::string strService = body.get<std::string>(SULOKI_SERVICE_MSGFIELD_KEY_SULOKIDEF);
		if (strBusinessid.compare(SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF) == 0 && strMessageid.compare(SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF) == 0)
		if(strGroup.length() > 0 && strGroup.compare(Suloki::URC_MAINGROUP_SERVICE) != 0 && strService.length() > 0 && strService.compare(Suloki::URC_MAINSERVICE_SERVICE) != 0)
		{
			Suloki::SmartPtr< Suloki::Service > serviceSmart;
			Suloki::SmartPtr< MyWsService > mainserviceSmart(new MyWsService());
			if (mainserviceSmart.IsNULL())
			{
				return Suloki::NOMEMORY_ERRCODE;
			}
			mainserviceSmart->Init(&(Suloki::UrcSingleton::Instance()), strGroup, strService);
			serviceSmart = mainserviceSmart.Downcast<Suloki::Service>();
			if (MyServiceManagerSingleton::Instance().AddServide(strGroup, strService, serviceSmart) != Suloki::SUCCESS)
			{
				std::stringstream strStream;
				strStream << "add ws service error.strGroup:" << strGroup << ";strService:" << strService;
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
			}
			else
			{//????? not thread safe
				SetGroupService(strGroup, strService);
			}
			return Suloki::FAIL;
		}
	}
	catch (boost::property_tree::ptree_error pt) {
		return Suloki::FAIL;
	}
	catch (...) {
		return Suloki::FAIL;
	}
	*/
	Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(root));
	if(msgSmart.IsNULL())
		return Suloki::FAIL;
	//
	std::string group = msgSmart->GetField(SULOKI_GROUPTO_MSGFIELD_KEY_SULOKIDEF);
	std::string service = msgSmart->GetField(SULOKI_SERVICETO_MSGFIELD_KEY_SULOKIDEF);
	std::string strMyGroup;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_GROUPNAME_KEY, strMyGroup);
	if (strMyGroup.compare(Suloki::URC_URCGROUP_SERVICE) == 0 && group.length() > 0 && strMyGroup.compare(group) != 0 && service.length() > 0)
	{
		std::vector<std::string> context;
		context.push_back(GetUrname());
		context.push_back(GetRemoteIp());
		context.push_back(GetRemotePort());
		context.push_back(SULOKI_TRUE_VAL_SULOKIDEF);
		Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
		if (contextSmart.IsNULL())
			return Suloki::FAIL;
		std::stringstream strStreamQueueKey;
		//strStreamQueueKey << Suloki::URC_URC_MQUEUE_NAMEPREFIX << group << "_" << service;
		strStreamQueueKey << Suloki::URC_SYSTEM_NAMEPREFIX << group << "_" << service << Suloki::URC_PARAQUEUE_NAMEKEY;
		Suloki::UrcSingleton::Instance().PostMsgSubUr(strStreamQueueKey.str(), msgSmart, contextSmart);
		return Suloki::FAIL;
	}
	//
	std::vector<std::string> context;
	context.push_back(GetUrname());
	context.push_back(GetRemoteIp());
	context.push_back(GetRemotePort());
	context.push_back(SULOKI_TRUE_VAL_SULOKIDEF);
	Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
	if (msgSmart.IsNULL() || contextSmart.IsNULL())
	{
		std::stringstream strStream;
		strStream << Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE) << strReq;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		root.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		root.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(Suloki::NOMEMORY_ERRCODE));
		root.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE));
		MakeJsonstr(root, strRes);
		return Suloki::SUCCESS;
	}
	//MyHandlerMachineSingleton::Instance().Handle(msgSmart, contextSmart);
	Suloki::UrcSingleton::Instance().PostMsgToMainService(msgSmart, contextSmart);
	return Suloki::FAIL;
}
Suloki::Ret MyWsSession::MakeJsonstr(const boost::property_tree::ptree& jsonObj, std::string& strRes)
{
	try {
		std::stringstream strStream;
		boost::property_tree::write_json(strStream, jsonObj);
		strRes = strStream.str();
		//?????
		while (true)
		{
			int pos = strRes.find("\\/");
			if (pos == std::string::npos)
				break;
			strRes.erase(pos, 1);
		}
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "handle json excepion", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "handle json excepion1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	return Suloki::SUCCESS;
}
void MyWsSession::HandleException(void)
{
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (Suloki::UrcSingleton::Instance().GetUr(m_urName, baseSmart) == Suloki::SUCCESS)
	{
		Suloki::SmartPtr< Suloki::Service > wsService = baseSmart.Upcast<Suloki::Service>();
		std::string group, service;
		wsService->GetGroupService(group, service);
		Suloki::SmartPtr< Suloki::Service > serviceSmart;
		if(group.length() > 0 && service.length() > 0)
			MyServiceManagerSingleton::Instance().RemoveServide(group, service, serviceSmart);
	}
	//
	session::HandleException();
	/*m_bReady = false;
	Suloki::SmartPtr< Suloki::Base > baseptrSmart;
	Suloki::UrcSingleton::Instance().DelUr(m_urName, baseptrSmart);*/
	/*
	std::string strGroup;
	std::string strService;
	GetGroupService(strGroup, strService);
	if (strGroup.length() > 0 && strService.length() > 0)
	{
		Suloki::SmartPtr< Suloki::Service > serviceSmart;
		MyServiceManagerSingleton::Instance().RemoveServide(strGroup, strService, serviceSmart);
	}
	*/
}
void MyWsSession::HandleConnectted(void)
{
	//Suloki::SmartPtr< Suloki::Base > baseptrSmart;
	//if (Suloki::UrcSingleton::Instance().GetUr(m_urName, baseptrSmart) != Suloki::SUCCESS)
	//	return;
	//baseptrSmart->SetStateDeled(true);
#ifdef SULOKI_URC_SERVICE_SULOKIDEF
#else
	Suloki::EventManagerSingleton::Instance().SetTimerDelObjUr(m_urName, 1000);
#endif

	boost::property_tree::ptree jsonMsg;
	Suloki::Message::MakeJsonMsg(jsonMsg);
	try {
		jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
		jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_WSCONNECTED_MESSAGEID_VAL_SULOKIDEF);
		std::string serverIpRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERIPREMOTED_KEY, serverIpRemoted);
		std::string serverPortRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERPORTREMOTED_KEY, serverPortRemoted);
		Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(jsonMsg));
		std::vector<std::string> context;
		context.push_back(GetUrname());
		context.push_back(GetRemoteIp());
		context.push_back(GetRemotePort());
		context.push_back(SULOKI_TRUE_VAL_SULOKIDEF);
		Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
		if (!msgSmart.IsNULL() && !contextSmart.IsNULL())
		{
			//Suloki::SmartPtr<Suloki::Message> resmsgSmart;
			//Suloki::SmartPtr<Suloki::Context> rescontextSmart;
			//if (Suloki::UrcSingleton::Instance().SendMsgToMainService(msgSmart, contextSmart, resmsgSmart, rescontextSmart) != Suloki::SUCCESS)
			//{
			//	//????? modify to async function asyncclose
			//	//Close();
			//	std::shared_ptr<session> wsconnSmartPtr = shared_from_this();
			//	s_threadPoolClosed.Post(boost::bind(&session::CloseWs, wsconnSmartPtr));
			//}
			Suloki::UrcSingleton::Instance().PostMsgToMainService(msgSmart, contextSmart);
		}
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
	}
}
//
MyWsClientSession::MyWsClientSession(net::io_context& ioc)
	: clientsession(ioc)
{
}
MyWsClientSession::~MyWsClientSession()
{
}
void MyWsClientSession::HandleRead(std::string& strMsg)
{
	std::stringstream strStream;
	strStream << strMsg;
	boost::property_tree::ptree root;
	try {
		boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(strStream, root);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << Suloki::GetErrStr(Suloki::PARSEJSON_ERRCODE) << strMsg;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << Suloki::GetErrStr(Suloki::PARSEJSON_ERRCODE) << strMsg;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return;
	}
	if (Suloki::Message::CheckJsonMsg(root) != Suloki::SUCCESS)
	{
		std::stringstream strStream;
		strStream << Suloki::GetErrStr(Suloki::INVALIDMSG_ERRCODE) << strMsg;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return;
	}
	Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(root));
	std::vector<std::string> context;
	context.push_back(GetUrname());
	std::string serverIpRemoted;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERIPREMOTED_KEY, serverIpRemoted);
	context.push_back(serverIpRemoted);
	std::string serverPortRemoted;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERPORTREMOTED_KEY, serverPortRemoted);
	context.push_back(serverPortRemoted);
	context.push_back(SULOKI_FALSE_VAL_SULOKIDEF);
	Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
	if (msgSmart.IsNULL() || contextSmart.IsNULL())
	{
		std::stringstream strStream;
		strStream << Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE) << strMsg;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return;
	}
	//
	if (Suloki::EventManagerSingleton::Instance().Notify(msgSmart) == Suloki::SUCCESS)
		return;
	//
	//MyHandlerMachineSingleton::Instance().Handle(msgSmart, contextSmart);
	Suloki::UrcSingleton::Instance().PostMsgToMainService(msgSmart, contextSmart);
	return;
}
void MyWsClientSession::HandleConnectted(void)
{
	boost::property_tree::ptree jsonMsg;
	Suloki::Message::MakeJsonMsg(jsonMsg);
	try {
		jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
		jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_WSCONNECTED_MESSAGEID_VAL_SULOKIDEF);
		std::string serverIpRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERIPREMOTED_KEY, serverIpRemoted);
		std::string serverPortRemoted;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_WSSERVERPORTREMOTED_KEY, serverPortRemoted);
		Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(jsonMsg));
		std::vector<std::string> context;
		context.push_back(GetUrname());
		context.push_back(serverIpRemoted);
		context.push_back(serverPortRemoted);
		context.push_back(SULOKI_FALSE_VAL_SULOKIDEF);
		Suloki::SmartPtr<Suloki::Context> contextSmart(new Suloki::Context(context));
		if (!msgSmart.IsNULL() && !contextSmart.IsNULL())
		{
			//Suloki::SmartPtr<Suloki::Message> resmsgSmart;
			//Suloki::SmartPtr<Suloki::Context> rescontextSmart;
			//Suloki::UrcSingleton::Instance().SendMsgToMainService(msgSmart, contextSmart, resmsgSmart, rescontextSmart);
			Suloki::UrcSingleton::Instance().PostMsgToMainService(msgSmart, contextSmart);
		}
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
	}
	/*
	std::string groupname;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_GROUPNAME_KEY, groupname);
	std::string servicename;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_SERVICENAME_KEY, servicename);
	if (groupname.length() > 0 && servicename.length() > 0)
	{
		boost::property_tree::ptree jsonMsg;
		Suloki::Message::MakeJsonMsg(jsonMsg);
		try {
			//jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
			//jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF);
			//boost::property_tree::ptree jsonBody;
			//jsonBody.put<std::string>(SULOKI_GROUP_MSGFIELD_KEY_SULOKIDEF, groupname);
			//jsonBody.put<std::string>(SULOKI_SERVICE_MSGFIELD_KEY_SULOKIDEF, servicename);
			//jsonMsg.put_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF, jsonBody);
			jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
			jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF);
			jsonMsg.put<std::string>(SULOKI_GROUPFROM_MSGFIELD_KEY_SULOKIDEF, groupname);
			jsonMsg.put<std::string>(SULOKI_SERVICEFROM_MSGFIELD_KEY_SULOKIDEF, servicename);
		}
		catch (boost::property_tree::ptree_error pt) {
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		}
		catch (...) {
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		}
		Suloki::Message msg(jsonMsg);
		std::string strMsg = msg.Get();
		if (strMsg.length() > 0)
			WriteAsync(strMsg.c_str(), strMsg.length());
	}
	*/
}
MyWsClient::MyWsClient() :WsClient<MyWsClientSession>()
{}
MyWsClient::~MyWsClient()
{}
Suloki::Ret MyWsClient::Init(void)
{
	if (WsClient<MyWsClientSession>::Init() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "WsClient<MyWsClientSession>::Init() error", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	return Suloki::SUCCESS;
}
Suloki::Ret MyWsClient::Start(void)
{
	if (WsClient<MyWsClientSession>::Start() != Suloki::SUCCESS)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "WsClient<MyWsClientSession>::Start() error", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	m_threadConnectedOtherUrcSmart = std::auto_ptr<boost::thread>(new boost::thread(boost::bind(&MyWsClient::RunConnectedOtherUrc, this)));
	if (m_threadConnectedOtherUrcSmart.get() == NULL)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "new boost::thread error", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	return Suloki::SUCCESS;
}
Suloki::Ret MyWsClient::Run(void)
{
	return Suloki::SUCCESS;
}
Suloki::Ret MyWsClient::Stop(void)
{
	if (m_threadConnectedOtherUrcSmart.get() != NULL)
		m_threadConnectedOtherUrcSmart->join();
	//
	WsClient<MyWsClientSession>::Stop();
	return Suloki::SUCCESS;
}
Suloki::Ret MyWsClient::Clear(void)
{
	m_threadConnectedOtherUrcSmart.reset();
	//
	WsClient<MyWsClientSession>::Clear();
	return Suloki::SUCCESS;
}
void MyWsClient::RunConnectedOtherUrc(void)
{
	
}