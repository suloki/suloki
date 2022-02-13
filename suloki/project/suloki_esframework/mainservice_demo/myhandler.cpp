#include "myhandler.h"
#include "ws.h"
#include "suloki_esframeworkdef.h"

MyDispatcher::MyDispatcher() :m_pServiceInterface(NULL)
{
	{
		std::stringstream sStream;
		sStream << SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_SYSTEM_TEST_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_System_Test);
	}
	{
		std::stringstream sStream;
		sStream << SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF << "_" << SULOKI_SYSTEM_WSCONNECTED_MESSAGEID_VAL_SULOKIDEF;
		m_syncMsgHandlerMapSimple[sStream.str()] = HandlerFunctorSimple(this, &MyDispatcher::Handler_System_Wsconnected);
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
Suloki::Ret MyDispatcher::Handler_System_Test(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart)
{
	Suloki::UrcInterface* pUrcInterface;
	m_pServiceInterface->GetUrcInterface(&pUrcInterface);
	Suloki::Urc* pUrc = (Suloki::Urc*)pUrcInterface;
	//
	std::string strReq = msgSmart->Get();
	std::cout << strReq << std::endl;
	std::vector<std::string> contextVector = contextSmart->Get();
	if (contextVector.size() != SULOKI_TOTAL_CONTEXT_ESFRAMEWORK)
		return Suloki::FAIL;
	//
	boost::property_tree::ptree reqRoot = msgSmart->GetJsonobj();
	try {
		reqRoot.put<std::string>(SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF, SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF);
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
Suloki::Ret MyDispatcher::Handler_System_Wsconnected(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> contextSmart)
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
	if (contextVector[SULOKI_ISSERVER_POS_CONTEXT_ESFRAMEWORK].compare(SULOKI_TRUE_VAL_SULOKIDEF) == 0 
		&&  contextVector[SULOKI_WSREMOTEIP_POS_CONTEXT_ESFRAMEWORK].compare("127.0.0.1") != 0)
		return Suloki::FAIL;
	//
	return Suloki::SUCCESS;
}
//
MainService::MainService()
{
	m_handler.SetServiceInterface(this);
}
MainService::~MainService() 
{}
Suloki::Ret MainService::Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName)
{
	Suloki::Ret ret = Suloki::Service::Init(pUrcInterface, groupName, serviceName);
	return ret;
}
Suloki::Ret MainService::Start(void)
{
	return Suloki::SUCCESS;
}
Suloki::Ret MainService::Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart)
{
	return m_handler.Handle(msgSmart, paraSmart);
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
