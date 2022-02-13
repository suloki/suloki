#ifndef _JSHANDLER_H_
#define _JSHANDLER_H_

#include "suloki.h"

#include "duktape.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include "duk_module_duktape.h"
#ifdef __cplusplus
}
#endif

class JsHandler : public Suloki::Service
{
public:
	JsHandler();
	virtual ~JsHandler();
	//Service
	virtual Suloki::Ret Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName);
	virtual Suloki::Ret Start(void);
	virtual Suloki::Ret Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::SmartPtr<Suloki::Context>& paraOutSmart);
	virtual Suloki::Ret Stop(void);
	virtual Suloki::Ret Clear(void);
protected:
	duk_context* m_ctx;
};

#endif