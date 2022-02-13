#include "ws.h"

void
fail(beast::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

//Suloki::ThreadPool session::s_threadPoolClosed;
Suloki::Ret SessionSwrap::Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::SmartPtr<Suloki::Context>& paraOutSmart)
{
	/*std::vector<std::string> contexts = paraSmart->Get();
	if (contexts.size() == 0)
		return Suloki::FAIL;
	Suloki::SmartPtr< Suloki::Base > baseptrSmart;
	if (Suloki::UrcSingleton::Instance().GetUr(contexts[0], baseptrSmart) != Suloki::SUCCESS)
		return Suloki::FAIL;
	Suloki::SmartPtr<SessionSwrap> wsconnSmartPtr = baseptrSmart.Upcast<SessionSwrap>();
	std::string strMsg = msgSmart->Get();
	wsconnSmartPtr->GetSession()->WriteAsync(strMsg.c_str(), strMsg.length());*/
	std::string strMsg = msgSmart->Get();
	GetSession()->WriteAsync(strMsg.c_str(), strMsg.length());
	return Suloki::SUCCESS;
}