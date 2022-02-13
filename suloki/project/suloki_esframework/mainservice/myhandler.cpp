#include "myhandler.h"
#include "ws.h"
#include "suloki_esframeworkdef.h"
#ifdef SULOKI_AI_WORK_SULOKIDEF
#include "suloki_ai.h"
#include "suloki_ai.cpp"
#endif

MyDispatcher::MyDispatcher() :m_pServiceInterface(NULL)
{
	{
		std::stringstream sStream;
		sStream << SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_SYSTEM_TEST_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_System_Test);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_System_RegService);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_SYSTEM_WSCONNECTED_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_System_Wsconnected);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_URC_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_URC_ADD_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_Urc_Add);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_URC_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_URC_DEL_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_Urc_Del);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_URC_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_URC_UPDATE_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_Urc_Update);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_URC_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_URC_GET_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_Urc_Get);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_AUTH_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_AUTH_LOGIN_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_Auth_Login);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_AI_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_AI_WORK_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_Ai_Work);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_AI_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_AI_WENZIZUOBIAO_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMap[sStream.str()] = HandlerFunctor(this, &MyDispatcher::Handler_Ai_Wenzizuobiao);
	}
}
MyDispatcher::~MyDispatcher()
{}
void MyDispatcher::SetServiceInterface(Suloki::Service* pServiceInterface)
{
	m_pServiceInterface = pServiceInterface;
}
std::string MyDispatcher::CalKey(Suloki::SmartPtr<Suloki::Message> msgSmart)
{
	std::stringstream sStream;
	sStream << msgSmart->GetField(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF) << "_" << msgSmart->GetField(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF);
	return sStream.str();// protocolReq.m_msgKey;
}
Suloki::Ret MyDispatcher::HandleUnmatched(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart)
{
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "have msg HandleUnmatched error", Suloki::LOG_WARN_LEVEL);
	return Suloki::FAIL;
}
Suloki::Ret MyDispatcher::Handler_System_Test(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
		return Suloki::FAIL;
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	try {
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(Suloki::SUCCESS));
		reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(Suloki::SUCCESS));
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<Suloki::Message> resSmart(new Suloki::Message(reqRoot));
	if(resSmart.IsNULL())
		return Suloki::FAIL;
	std::string strRes = resSmart->Get();
	//
	std::stringstream strStream;
	strStream << contextVector[0];
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
		return Suloki::FAIL;
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmart.Upcast<SessionSwrap>();
	wsconnSmartPtr->GetSession()->WriteAsync(strRes.c_str(), strRes.length());
	//
	//std::string testContext = msgSmart->GetField("testcontext");
	//
	//std::stringstream strStream;
	//strStream << "MainService's MyDispatcher recv msg.msg constent:" << testContext;
	//SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_DEBUG_LEVEL);
	return Suloki::SUCCESS;
}
Suloki::Ret MyDispatcher::Handler_System_RegService(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
		return Suloki::FAIL;
	if (contextVector[SULOKI_ISSERVER_POS_CONTEXT_ESFRAMEWORK].compare(SULOKI_TRUE_VAL_SULOKIDEF) != 0)
		return Suloki::FAIL;
	std::string strMyGroup;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_GROUPNAME_KEY, strMyGroup);
	if(strMyGroup.compare(Suloki::URC_URCGROUP_SERVICE) != 0)
		return Suloki::FAIL;
	//
	std::stringstream strStream;
	strStream << contextVector[0];
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
		return Suloki::FAIL;
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmart.Upcast<SessionSwrap>();
	//
	std::string strGroup = msgSmart->GetField(SULOKI_GROUPFROM_MSGFIELD_KEY_SULOKIDEF);
	std::string strService = msgSmart->GetField(SULOKI_SERVICEFROM_MSGFIELD_KEY_SULOKIDEF);
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	try {
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(Suloki::SUCCESS));
		reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(Suloki::SUCCESS));
		//
		//Suloki::Ret ret = wsconnSmartPtr->GetSession()->SetupGroupService(strGroup, strService);
		//if(ret != Suloki::SUCCESS)
		//{
		//	reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
		//	reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
		//}
		//
		Suloki::SmartPtr< Suloki::Service > wsService = wsconnSmartPtr.Upcast<Suloki::Service>();
		wsService->Init(&(Suloki::UrcSingleton::Instance()), strGroup, strService);
		Suloki::Ret ret = MyServiceManagerSingleton::Instance().AddServide(strGroup, strService, wsService);
		if (ret != Suloki::SUCCESS)
		{
			reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
			reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
			//
			std::stringstream strStream;
			strStream << "add ws service error.strGroup:" << strGroup << ";strService:" << strService;
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		}
		else
			wsconnSmartPtr->GetSession()->SetLogined(true);
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<Suloki::Message> resSmart(new Suloki::Message(reqRoot));
	if (resSmart.IsNULL())
		return Suloki::FAIL;
	std::string strRes = resSmart->Get();
	wsconnSmartPtr->GetSession()->WriteAsync(strRes.c_str(), strRes.length());
	//
	//std::string testContext = msgSmart->GetField("testcontext");
	//
	//std::stringstream strStream;
	//strStream << "MainService's MyDispatcher recv msg.msg constent:" << testContext;
	//SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_DEBUG_LEVEL);
	return Suloki::SUCCESS;
}
Suloki::Ret MyDispatcher::Handler_System_Wsconnected(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	//std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "context size != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	//if (contextVector[SULOKI_ISSERVER_POS_CONTEXT_ESFRAMEWORK].compare(SULOKI_TRUE_VAL_SULOKIDEF) == 0 
	//	&&  contextVector[SULOKI_WSREMOTEIP_POS_CONTEXT_ESFRAMEWORK].compare("127.0.0.1") != 0)
	//	return Suloki::FAIL;
	if (contextVector[SULOKI_ISSERVER_POS_CONTEXT_ESFRAMEWORK].compare(SULOKI_TRUE_VAL_SULOKIDEF) != 0)
	{
		std::stringstream strStream;
		strStream << contextVector[SULOKI_WSURNAME_POS_CONTEXT_ESFRAMEWORK];
		Suloki::SmartPtr< Suloki::Base > baseSmart;
		if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
		{
			std::stringstream strStream;
			strStream << "GetUr error.urname:" << strStream.str();
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
			return Suloki::FAIL;
		}
		Suloki::SmartPtr<ClientsessionSwrap> wsconnSmartPtr = baseSmart.Upcast<ClientsessionSwrap>();
		std::string groupname;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_GROUPNAME_KEY, groupname);
		std::string servicename;
		Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_SERVICENAME_KEY, servicename);
		if (groupname.length() > 0 && servicename.length() > 0)
		{
			boost::property_tree::ptree jsonMsg;
			Suloki::Message::MakeJsonMsg(jsonMsg);
			try {
				jsonMsg.put<std::string>(SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF);
				jsonMsg.put<std::string>(SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF, SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF);
				jsonMsg.put<std::string>(SULOKI_GROUPFROM_MSGFIELD_KEY_SULOKIDEF, groupname);
				jsonMsg.put<std::string>(SULOKI_SERVICEFROM_MSGFIELD_KEY_SULOKIDEF, servicename);
				jsonMsg.put<std::string>(SULOKI_GROUPTO_MSGFIELD_KEY_SULOKIDEF, Suloki::URC_URCGROUP_SERVICE);
				jsonMsg.put<std::string>(SULOKI_SERVICETO_MSGFIELD_KEY_SULOKIDEF, Suloki::URC_URCSERVICE_SERVICE);
			}
			catch (boost::property_tree::ptree_error pt) {
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
				return Suloki::FAIL;
			}
			catch (...) {
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
				return Suloki::FAIL;
			}
			Suloki::SmartPtr<Suloki::Message> reqSmart(new Suloki::Message(jsonMsg));
			Suloki::SmartPtr<Suloki::Message> resSmart;
			if (!reqSmart.IsNULL() && wsconnSmartPtr->GetSession()->ReqRes(reqSmart, resSmart, 1000) != Suloki::SUCCESS)
			{
				std::stringstream strStream;
				strStream << "reqres SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF msg error.groupname:" << groupname << ";servicename:" << servicename;
				SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
				wsconnSmartPtr->GetSession()->Close();
			}
		}
	}
	//
	return Suloki::SUCCESS;
}
Suloki::Ret MyDispatcher::Handler_Urc_Add(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	//std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "context size != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	//
	std::string strKey;
	std::string strType;
	std::string strVal;
	try {
		strKey = reqRoot.get<std::string>(SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF);
		strType = reqRoot.get<std::string>(SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF);
		strVal = reqRoot.get<std::string>(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << "get json field exception.blank field:" << SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF << "or" << SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF << " or " << SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << "get json field exception1.blank field:" << SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF << "or" << SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF << " or " << SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	Suloki::Ret ret = pUrc->AddData(strKey, strVal);
	//
	try {
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
		reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<Suloki::Message> resSmart(new Suloki::Message(reqRoot));
	if (resSmart.IsNULL())
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	std::string strRes = resSmart->Get();
	//
	std::stringstream strStream;
	strStream << contextVector[0];
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
	{
		std::stringstream strStream;
		strStream << "GetUr error.urname:" << strStream.str();
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmart.Upcast<SessionSwrap>();
	wsconnSmartPtr->GetSession()->WriteAsync(strRes.c_str(), strRes.length());
	return Suloki::SUCCESS;
}
Suloki::Ret MyDispatcher::Handler_Urc_Del(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	//std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "context size != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	//
	std::string strKey;
	std::string strType;
	try {
		strKey = reqRoot.get<std::string>(SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF);
		strType = reqRoot.get<std::string>(SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << "get json field exception.blank field:" << SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF << "or" << SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF << " or " << SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << "get json field exception1.blank field:" << SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF << "or" << SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF << " or " << SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	std::string strVal;
	Suloki::Ret ret = pUrc->DelData(strKey, strVal);
	//
	try {
		if (ret == Suloki::SUCCESS)
			reqRoot.put<std::string>(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF, strVal);
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
		reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<Suloki::Message> resSmart(new Suloki::Message(reqRoot));
	if (resSmart.IsNULL())
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	std::string strRes = resSmart->Get();
	//
	std::stringstream strStream;
	strStream << contextVector[0];
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
	{
		std::stringstream strStream;
		strStream << "GetUr error.urname:" << strStream.str();
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmart.Upcast<SessionSwrap>();
	wsconnSmartPtr->GetSession()->WriteAsync(strRes.c_str(), strRes.length());
	return Suloki::SUCCESS;
}
Suloki::Ret MyDispatcher::Handler_Urc_Update(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	//Suloki::UrcInterface* pUrcInterface;
	//m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	////
	//std::string strUrName = msgSmart->GetField(SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF);
	////
	//std::stringstream strStream;
	//strStream << "MainService's MyDispatcher recv update msg.urname:" << strUrName;
	//SULOKI_WRITELOG_SULOKI(Suloki::Log::STDCOUTTYPE, strStream.str(), Suloki::LOG_DEBUG_LEVEL);
	//return Suloki::SUCCESS;
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	//std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "context size != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	//
	std::string strKey;
	std::string strType;
	std::string strVal;
	try {
		strKey = reqRoot.get<std::string>(SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF);
		strType = reqRoot.get<std::string>(SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF);
		strVal = reqRoot.get<std::string>(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << "get json field exception.blank field:" << SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF << "or" << SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF << " or " << SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << "get json field exception1.blank field:" << SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF << "or" << SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF << " or " << SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	std::string strValOri;
	Suloki::Ret ret = pUrc->UpdateData(strKey, strVal, strValOri);
	//
	try {
		if (ret == Suloki::SUCCESS)
			reqRoot.put<std::string>(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF, strValOri);
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
		reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<Suloki::Message> resSmart(new Suloki::Message(reqRoot));
	if (resSmart.IsNULL())
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	std::string strRes = resSmart->Get();
	//
	std::stringstream strStream;
	strStream << contextVector[0];
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
	{
		std::stringstream strStream;
		strStream << "GetUr error.urname:" << strStream.str();
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmart.Upcast<SessionSwrap>();
	wsconnSmartPtr->GetSession()->WriteAsync(strRes.c_str(), strRes.length());
	return Suloki::SUCCESS;
}
Suloki::Ret MyDispatcher::Handler_Urc_Get(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	//std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "context size != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	//
	std::string strKey;
	std::string strType;
	try {
		strKey = reqRoot.get<std::string>(SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF);
		strType = reqRoot.get<std::string>(SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << "get json field exception.blank field:" << SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF << "or" << SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF << " or " << SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << "get json field exception1.blank field:" << SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF << "or" << SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF << " or " << SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	std::string strVal;
	Suloki::Ret ret = pUrc->GetData(strKey, strVal);
	//
	try {
		if(ret == Suloki::SUCCESS)
			reqRoot.put<std::string>(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF, strVal);
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
		reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<Suloki::Message> resSmart(new Suloki::Message(reqRoot));
	if (resSmart.IsNULL())
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	std::string strRes = resSmart->Get();
	//
	std::stringstream strStream;
	strStream << contextVector[0];
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
	{
		std::stringstream strStream;
		strStream << "GetUr error.urname:" << strStream.str();
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmart.Upcast<SessionSwrap>();
	wsconnSmartPtr->GetSession()->WriteAsync(strRes.c_str(), strRes.length());
	return Suloki::SUCCESS;
}
Suloki::Ret MyDispatcher::Handler_Auth_Login(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	//std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "context size != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	//
	boost::property_tree::ptree body;
	std::string strUser;
	std::string strPass;
	std::string strPassMd5;
	try {
		body = reqRoot.get_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF);
		strUser = body.get<std::string>(SULOKI_USER_KEY_SULOKIDEF);
		strPass = body.get<std::string>(SULOKI_PASS_KEY_SULOKIDEF);
		Suloki::Tool::GetMd5(strPass.c_str(), strPass.length(), strPassMd5);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << "get json field exception";
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << "get json field exception";
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	std::string strVal;
#ifdef SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF
	Suloki::Ret ret = pUrc->GetData(Suloki::URC_USERS_PSTORAGE_NAMEPREFIX + strUser, strVal);
	if (ret != Suloki::SUCCESS)
	{
		std::stringstream stringStream;
		stringStream << "GetData error,urc key:" << Suloki::URC_USERS_PSTORAGE_NAMEPREFIX + strUser;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, stringStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
#else
	std::stringstream stringStream;
	stringStream << "GetData error,urc key:" << Suloki::URC_USERS_PSTORAGE_NAMEPREFIX + strUser;
	SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, stringStream.str(), Suloki::LOG_ERROR_LEVEL);
	return Suloki::FAIL;
#endif
	//
	std::string strPassMd5InDb;
	try {
		std::stringstream strStream;
		strStream << strVal;
		boost::property_tree::ptree root;
		boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(strStream, root);
		//boost::property_tree::ptree body;
		//body = root.get_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF);
		strPassMd5InDb = root.get<std::string>(SULOKI_PASS_KEY_SULOKIDEF);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << "parse db json or get json field exception";
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << "parse db json or get json field exception";
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	if (strPassMd5.compare(strPassMd5InDb) != 0)
	{
		std::stringstream strStream;
		strStream << "user or pass is invalid.user:" << strUser;
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	std::stringstream strStream;
	strStream << contextVector[0];
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
	{
		std::stringstream strStream;
		strStream << "GetUr error.urname:" << strStream.str();
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//baseSmart->SetStateDeled(false);
	Suloki::SmartPtr< SessionSwrap > wssessionSwrap = baseSmart.Upcast<SessionSwrap>();
	wssessionSwrap->GetSession()->SetLogined(true);
	//
	ret = Suloki::SUCCESS;
	//
	try {
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
		reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<Suloki::Message> resSmart(new Suloki::Message(reqRoot));
	if (resSmart.IsNULL())
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	std::string strRes = resSmart->Get();
	//
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmart.Upcast<SessionSwrap>();
	wsconnSmartPtr->GetSession()->WriteAsync(strRes.c_str(), strRes.length());
	return Suloki::SUCCESS;
}
Suloki::Ret MyDispatcher::Handler_Ai_Work(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	//std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "context size != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	//
	boost::property_tree::ptree body;
	std::string strModelId;
	std::string strInput;
	try {
		body = reqRoot.get_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF);
		strModelId = body.get<std::string>(SULOKI_MODELID_KEY_SULOKIDEF);
		strInput = body.get<std::string>(SULOKI_INPUT_KEY_SULOKIDEF);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << "get json field exception";
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << "get json field exception";
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	Suloki::Ret ret = Suloki::SUCCESS;
#ifdef SULOKI_AI_WORK_SULOKIDEF
	static NumSnn s_numNn;
	static bool s_bFirst = true;
	if (s_bFirst)
	{
		s_bFirst = false;
		s_numNn.Init("model");
	}
	int patternInx;
	double value;
	//int cycle;
	std::vector<double> valueVector;
	ret = s_numNn.Work("", patternInx, value, valueVector, strInput);
#endif
	//
	std::stringstream strStream;
	strStream << contextVector[0];
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
	{
		std::stringstream strStream;
		strStream << "GetUr error.urname:" << strStream.str();
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	try {
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
		reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
		std::stringstream strStream;
#ifdef SULOKI_AI_WORK_SULOKIDEF
		strStream << patternInx;
#else
#endif
		boost::property_tree::ptree body;
		body.put<std::string>("result number", strStream.str());
		reqRoot.add_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF, body);
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<Suloki::Message> resSmart(new Suloki::Message(reqRoot));
	if (resSmart.IsNULL())
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	std::string strRes = resSmart->Get();
	//
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmart.Upcast<SessionSwrap>();
	if(wsconnSmartPtr->GetSession()->GetLogined())
		wsconnSmartPtr->GetSession()->WriteAsync(strRes.c_str(), strRes.length());
	return Suloki::SUCCESS;
}
Suloki::Ret MyDispatcher::Handler_Ai_Wenzizuobiao(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	//std::string strReq = msgSmart->Get();
	//std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "context size != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	//
	boost::property_tree::ptree body;
	std::string strModelId;
	std::string strInput;
	try {
		body = reqRoot.get_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF);
		strModelId = body.get<std::string>(SULOKI_MODELID_KEY_SULOKIDEF);
		strInput = body.get<std::string>(SULOKI_INPUT_KEY_SULOKIDEF);
	}
	catch (boost::property_tree::ptree_error pt) {
		std::stringstream strStream;
		strStream << "get json field exception";
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		std::stringstream strStream;
		strStream << "get json field exception";
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	Suloki::Ret ret = Suloki::SUCCESS;
	//
	{
		
	}
	//
	std::stringstream strStream;
	strStream << contextVector[0];
	Suloki::SmartPtr< Suloki::Base > baseSmart;
	if (pUrc->GetUr(strStream.str(), baseSmart) != Suloki::SUCCESS)
	{
		std::stringstream strStream;
		strStream << "GetUr error.urname:" << strStream.str();
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, strStream.str(), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	//
	try {
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
		reqRoot.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
		reqRoot.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
		std::stringstream strStream;
//#ifdef SULOKI_AI_WORK_SULOKIDEF
//		strStream << patternInx;
//#else
//#endif
		boost::property_tree::ptree body;
		body.put<std::string>("result number", strStream.str());
		reqRoot.add_child(SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF, body);
	}
	catch (boost::property_tree::ptree_error pt) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	catch (...) {
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "put json field exception1", Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	Suloki::SmartPtr<Suloki::Message> resSmart(new Suloki::Message(reqRoot));
	if (resSmart.IsNULL())
	{
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, Suloki::GetErrStr(Suloki::NOMEMORY_ERRCODE), Suloki::LOG_ERROR_LEVEL);
		return Suloki::FAIL;
	}
	std::string strRes = resSmart->Get();
	//
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseSmart.Upcast<SessionSwrap>();
	if (wsconnSmartPtr->GetSession()->GetLogined())
		wsconnSmartPtr->GetSession()->WriteAsync(strRes.c_str(), strRes.length());
	return Suloki::SUCCESS;
}
//
MainService::MainService()
{
#ifdef SULOKI_MODULEDLL_SULOKIDEF
#else
	std::string groupname;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_GROUPNAME_KEY, groupname);
	std::string servicename;
	Suloki::ConfigSingleton::Instance().GetConfig(Suloki::CONFIG_SERVICENAME_KEY, servicename);
	//Suloki::Service::Init(&Suloki::UrcSingleton::Instance(), Suloki::URC_MAINGROUP_SERVICE, Suloki::URC_MAINSERVICE_SERVICE);
	Suloki::Service::Init(&Suloki::UrcSingleton::Instance(), groupname, servicename);
#endif
	m_handler.SetServiceInterface(this);
}
MainService::~MainService() 
{}
Suloki::Ret MainService::Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName)
{
	Suloki::ConfigSingleton::Instance().SetConfig(Suloki::CONFIG_GROUPNAME_KEY, groupName);
	std::string servicename;
	Suloki::ConfigSingleton::Instance().SetConfig(Suloki::CONFIG_SERVICENAME_KEY, serviceName);
	/*
	{//add urc services
		Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
		const char* pUrcServices = "{\"group\":\"urcgroup\",\"service\":\"urcservice0\",\"pass\":\"urcservice0\",\"ip\":\"127.0.0.1\",\"port\":\"8080\",\"urnamefrom\":\"\0\",\"urnameto\":\"@@@@@@@@\"}";
		std::string strMd5;
		Suloki::Tool::GetMd5("urcservice0", strlen("urcservice0"), strMd5);
		std::string strKey = "urcservice0";
		std::string strVal;
		try {
			boost::property_tree::ptree root;
			std::stringstream strStream;
			strStream << pUrcServices;
			boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(strStream, root);
			root.put<std::string>(SULOKI_PASS_KEY_SULOKIDEF, strMd5);
			{
				std::stringstream strStream;
				boost::property_tree::write_json(strStream, root);
				strVal = strStream.str();
			}
		}
		catch (boost::property_tree::ptree_error pt) {
		}
		catch (...) {
		}
		Suloki::Ret ret = pUrc->AddData(Suloki::URC_URCSERVICE_PSTORAGE_NAMEPREFIX + strKey, strVal, 0, true);
		if (ret != Suloki::SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "add user test error", Suloki::LOG_ERROR_LEVEL);
		}
	}
	{//add urc services
		Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
		const char* pUrcServices = "{\"group\":\"urcgroup\",\"service\":\"urcservice1\",\"pass\":\"urcservice1\",\"ip\":\"127.0.0.1\",\"port\":\"8081\",\"urnamefrom\":\"@@@@@@@@\",\"urnameto\":\"~~~~~~~~\"}";
		std::string strMd5;
		Suloki::Tool::GetMd5("urcservice1", strlen("urcservice1"), strMd5);
		std::string strKey = "urcservice1";
		std::string strVal;
		try {
			boost::property_tree::ptree root;
			std::stringstream strStream;
			strStream << pUrcServices;
			boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(strStream, root);
			root.put<std::string>(SULOKI_PASS_KEY_SULOKIDEF, strMd5);
			{
				std::stringstream strStream;
				boost::property_tree::write_json(strStream, root);
				strVal = strStream.str();
			}
		}
		catch (boost::property_tree::ptree_error pt) {
		}
		catch (...) {
		}
		Suloki::Ret ret = pUrc->AddData(Suloki::URC_URCSERVICE_PSTORAGE_NAMEPREFIX + strKey, strVal, 0, true);
		if (ret != Suloki::SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "add user test error", Suloki::LOG_ERROR_LEVEL);
		}
	}
	{//add bisiness test testservice0 services
		Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
		const char* pUrcServices = "{\"group\":\"test\",\"service\":\"testservice0\",\"pass\":\"testservice0\",\"ip\":\"127.0.0.1\",\"port\":\"\"}";
		std::string strMd5;
		Suloki::Tool::GetMd5("testservice0", strlen("testservice0"), strMd5);
		std::string strKey = "testservice0";
		std::string strVal;
		try {
			boost::property_tree::ptree root;
			std::stringstream strStream;
			strStream << pUrcServices;
			boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(strStream, root);
			root.put<std::string>(SULOKI_PASS_KEY_SULOKIDEF, strMd5);
			{
				std::stringstream strStream;
				boost::property_tree::write_json(strStream, root);
				strVal = strStream.str();
			}
		}
		catch (boost::property_tree::ptree_error pt) {
		}
		catch (...) {
		}
		Suloki::Ret ret = pUrc->AddData(Suloki::URC_URCSERVICE_PSTORAGE_NAMEPREFIX + strKey, strVal, 0, true);
		if (ret != Suloki::SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "add user test error", Suloki::LOG_ERROR_LEVEL);
		}
	}
	//
	//
	{//add user default
		Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
		const char* pUser = "{\"user\":\"guest\",\"pass\":\"guest\",\"phone\":\"13500002783\",\"email\":\"mcj000@aliyun.com\",\"refnumlogined\":\"0\",\"money\":\"0.0\"}";
		std::string strMd5;
		Suloki::Tool::GetMd5("guest", strlen("guest"), strMd5);
		std::string strKey = "guest";
		std::string strVal;
		try {
			boost::property_tree::ptree root;
			std::stringstream strStream;
			strStream << pUser;
			boost::property_tree::json_parser::read_json<boost::property_tree::ptree>(strStream, root);
			root.put<std::string>(SULOKI_PASS_KEY_SULOKIDEF, strMd5);
			{
				std::stringstream strStream;
				boost::property_tree::write_json(strStream, root);
				strVal = strStream.str();
			}
		}
		catch (boost::property_tree::ptree_error pt) {
		}
		catch (...) {
		}
		Suloki::Ret ret = pUrc->AddData(Suloki::URC_USERS_PSTORAGE_NAMEPREFIX + strKey, strVal, 0, true);
		if (ret != Suloki::SUCCESS)
		{
			SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, "add user test error", Suloki::LOG_ERROR_LEVEL);
		}
	}
	*/
	Suloki::Ret ret = Suloki::Service::Init(pUrcInterface, groupName, serviceName);
	return ret;
}
Suloki::Ret MainService::Start(void)
{
	return Suloki::SUCCESS;
}
Suloki::Ret MainService::Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resmsgSmart, Suloki::SmartPtr<Suloki::Context>& resparaSmart)
{
	return m_handler.Handle(msgSmart, paraSmart, resmsgSmart, resparaSmart);
	//return Suloki::SUCCESS;
}
Suloki::Ret MainService::Stop(void)
{
	return Suloki::SUCCESS;
}
Suloki::Ret MainService::Clear(void)
{
	return Suloki::SUCCESS;
}
