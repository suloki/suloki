#include "jshandler.h"

static duk_ret_t native_print(duk_context *ctx) {
	duk_push_string(ctx, " ");
	duk_insert(ctx, 0);
	duk_join(ctx, duk_get_top(ctx) - 1);
	printf("%s\n", duk_safe_to_string(ctx, -1));
	return 0;
}

static duk_ret_t native_adder(duk_context *ctx) {
	int i;
	int n = duk_get_top(ctx);  /* #args */
	double res = 0.0;

	for (i = 0; i < n; i++) {
		res += duk_to_number(ctx, i);
	}

	duk_push_number(ctx, res);
	return 1;  /* one return value */
}
static duk_ret_t native_OperateCache(duk_context *ctx)
{
	Suloki::Ret ret = Suloki::SUCCESS;
	int n = duk_get_top(ctx);
	if (n != 1 || !duk_is_string(ctx, 0))
	{
		//SULOKI_WRITE_LOG("para num error", Suloki::ERROR_LEVEL_LOG, Suloki::SULOKI_LOG_ATTRIB_LOG);
		//duk_push_int(ctx, Suloki::INVALIDPARA_ERRCODE);
		boost::property_tree::ptree jsonObj;
		Suloki::Message::MakeJsonMsg(jsonObj);
		std::string strRes;
		try {
			jsonObj.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(Suloki::INVALIDPARA_ERRCODE));
			jsonObj.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(Suloki::INVALIDPARA_ERRCODE));
			std::stringstream strStream;
			boost::property_tree::write_json(strStream, jsonObj);
			strRes = strStream.str();
		}
		catch (boost::property_tree::ptree_error pt) {
			duk_push_string(ctx, "make json exception");
			return 1;
		}
		catch (...) {
			duk_push_string(ctx, "make json exception1");
			return 1;
		}
		duk_push_string(ctx, strRes.c_str());
		return 1;
	}
	//
	//if (!duk_is_buffer_data(ctx, 0))
	//{
	//	//SULOKI_WRITE_LOG("para 0 is not buffer type error", Suloki::ERROR_LEVEL_LOG, Suloki::SULOKI_LOG_ATTRIB_LOG);
	//	duk_push_int(ctx, Suloki::INVALIDPARA_ERRCODE);
	//	return 1;
	//}
	/*duk_size_t sz = 0;
	void* pTr = duk_get_buffer_data(ctx, 0, &sz);
	Suloki::SmartPtr<Suloki::Message> msgSmart(new Suloki::Message(std::string((char*)pTr, sz)));
	if (msgSmart.get() == NULL)
	{
		duk_push_int(ctx, Suloki::NOMEMORY_ERRCODE);
		return 1;
	}*/
	//if (!duk_is_string(ctx, 0))
	//{
	//	//SULOKI_WRITE_LOG("para 1 is not string type error", Suloki::ERROR_LEVEL_LOG, Suloki::SULOKI_LOG_ATTRIB_LOG);
	//	duk_push_int(ctx, Suloki::INVALIDPARA_ERRCODE);
	//	return 1;
	//}
	std::string strJsonInput = duk_to_string(ctx, 0);
	std::cout << "native_OperateCache:" << strJsonInput << std::endl;
	//if (!duk_is_string(ctx, 1))
	//{
	//	//SULOKI_WRITE_LOG("para 1 is not string type error", Suloki::ERROR_LEVEL_LOG, Suloki::SULOKI_LOG_ATTRIB_LOG);
	//	duk_push_int(ctx, Suloki::INVALIDPARA_ERRCODE);
	//	return 1;
	//}
	//std::string moduleNameFrom = duk_to_string(ctx, 1);
	//msgSmart->add_routers(moduleNameFrom);
	//
	//std::auto_ptr<Suloki::SulokiMessage> resSmart;
	ret = Suloki::SUCCESS;// Suloki_Sfw::MyMainModuleSingleton::Instance().OperateCache(msgSmart, resSmart);
	//if (ret != Suloki::SUCCESS)
	//{
	//	//SULOKI_WRITE_LOG("OperateCache error", Suloki::ERROR_LEVEL_LOG, Suloki::SULOKI_LOG_ATTRIB_LOG);
	//	//duk_push_int(ctx, ret);
	//	boost::property_tree::ptree jsonMsg;
	//	Suloki::Message::MakeJsonMsg(jsonMsg);
	//	std::string strRes;
	//	try {
	//		jsonMsg.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
	//		jsonMsg.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
	//		std::stringstream strStream;
	//		boost::property_tree::write_json(strStream, jsonMsg);
	//		strRes = strStream.str();
	//	}
	//	catch (boost::property_tree::ptree_error pt) {
	//		duk_push_string(ctx, "make json exception");
	//		return 1;
	//	}
	//	catch (...) {
	//		duk_push_string(ctx, "make json exception1");
	//		return 1;
	//	}
	//	duk_push_string(ctx, strRes.c_str());
	//	return 1;
	//}
	//if (ret == Suloki::SUCCESS && resSmart.get() == NULL)
	//{
	//	duk_push_int(ctx, ret);
	//	return 1;
	//}
	//resSmart->set_errorcode(ret);
	//std::string strRes;
	//ret = SulokiProtoSwrap::EncodeProtocol(*resSmart, strRes);
	//if (ret != Suloki::SUCCESS)
	//{
	//	//SULOKI_WRITE_LOG("response EncodeProtocol error", Suloki::ERROR_LEVEL_LOG, Suloki::SULOKI_LOG_ATTRIB_LOG);
	//	duk_push_int(ctx, ret);
	//	return 1;
	//}
	//void* pBuf = duk_push_buffer(ctx, strRes.length(), 0);
	//memcpy(pBuf, strRes.c_str(), strRes.length());
	//duk_push_int(ctx, ret);
	{
		boost::property_tree::ptree jsonObj;
		//Suloki::Message::MakeJsonMsg(jsonObj);
		std::string strRes;
		try {
			std::stringstream strStream;
			strStream << strJsonInput;
			boost::property_tree::read_json<boost::property_tree::ptree>(strStream, jsonObj);
			jsonObj.put<std::string>(SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF, Suloki::GetCodeErr(ret));
			jsonObj.put<std::string>(SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF, Suloki::GetErrStr(ret));
			{
				std::stringstream strStream;
				boost::property_tree::write_json(strStream, jsonObj);
				strRes = strStream.str();
			}
		}
		catch (boost::property_tree::ptree_error pt) {
			duk_push_string(ctx, "make json exception");
			return 1;
		}
		catch (...) {
			duk_push_string(ctx, "make json exception1");
			return 1;
		}
		duk_push_string(ctx, strRes.c_str());
	}
	return 1;
}

JsHandler::JsHandler()
{
	Suloki::Service::Init(&Suloki::UrcSingleton::Instance(), Suloki::URC_MAINGROUP_SERVICE, Suloki::URC_MAINSERVICE_SERVICE);
}
JsHandler::~JsHandler()
{}
Suloki::Ret JsHandler::Init(Suloki::UrcInterface* pUrcInterface, std::string groupName, std::string  serviceName)
{
	//Suloki::Ret ret = Suloki::Service::Init(pUrcInterface, groupName, serviceName);
	//if (ret != Suloki::SUCCESS)
	//	return ret;
	m_ctx = duk_create_heap_default();
	if (m_ctx == NULL)
		return Suloki::FAIL;
	//
	duk_push_c_function(m_ctx, native_print, DUK_VARARGS);
	duk_put_global_string(m_ctx, "print");
	//duk_push_c_function(m_ctx, native_adder, DUK_VARARGS);
	//duk_put_global_string(m_ctx, "adder");
	duk_push_c_function(m_ctx, native_OperateCache, 1);
	duk_put_global_string(m_ctx, "Suloki_OperateCache");
	//
	//duk_eval_string(m_ctx, "print('Hello world!');");
	//
	duk_push_global_object(m_ctx);
	//const char* pString = "function handle(message, context){print('Hello world!!'+message+context);Suloki_OperateCache('{\"add\":\"data\"}');}";
	//duk_size_t len = strlen(pString);
	std::string strFromFile;
	{
		char* pData = NULL;
		duk_size_t dataLen = 0;
		FILE* fp = fopen("./js/mainservice.js", "rb");
		if (fp == NULL)
			return Suloki::FAIL;
		fseek(fp, 0, SEEK_END);
		dataLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		pData = new char[dataLen + 2];
		if (pData == NULL)
		{
			fclose(fp);
			return Suloki::FAIL;
		}
		fread(pData, dataLen, 1, fp);
		pData[dataLen] = '\0';
		fclose(fp);
		strFromFile.append(pData);
		delete[] pData;
	}
	duk_push_lstring(m_ctx, strFromFile.c_str(), strFromFile.length());
	duk_peval(m_ctx);
	duk_pop(m_ctx);  /* ignore result */
	//
	//duk_bool_t bRet = duk_get_prop_string(m_ctx, -1, "handle");
	//duk_push_string(m_ctx, "message");
	//duk_push_string(m_ctx, "context");
	//duk_int_t iRet = duk_pcall(m_ctx, 2);
	//std::cout << duk_safe_to_string(m_ctx, -1) << std::endl;
	//duk_pop(m_ctx);  /* ignore result */
	//
	return Suloki::SUCCESS;
}
Suloki::Ret JsHandler::Start(void)
{
	return Suloki::SUCCESS;
}
Suloki::Ret JsHandler::Handle(Suloki::SmartPtr<Suloki::Message> msgSmart, Suloki::SmartPtr<Suloki::Context> paraSmart, Suloki::SmartPtr<Suloki::Message>& resSmart, Suloki::SmartPtr<Suloki::Context>& paraOutSmart)
{
	std::string strMessage;
	strMessage = msgSmart->Get();
	std::string strContext;
	duk_bool_t bRet = duk_get_prop_string(m_ctx, -1, "handle");
	duk_push_string(m_ctx, strMessage.c_str());
	duk_push_string(m_ctx, strContext.c_str());
	duk_int_t iRet = duk_pcall(m_ctx, 2);
	//std::cout << duk_safe_to_string(m_ctx, -1) << std::endl;
	duk_pop(m_ctx);  /* ignore result */
	return Suloki::SUCCESS;
}
Suloki::Ret JsHandler::Stop(void)
{
	return Suloki::SUCCESS;
}
Suloki::Ret JsHandler::Clear(void)
{
	if (m_ctx != NULL)
	{
		duk_destroy_heap(m_ctx);
		m_ctx = NULL;
	}
	return Suloki::SUCCESS;
}