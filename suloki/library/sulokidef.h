#ifndef _SULOKI_DEF_H_
#define _SULOKI_DEF_H_

#define SULOKI_OPENSOURCE_VERSION_SULOKIDEF
#define SULOKI_WINDOWS_OS_SULOKIDEF
//#define SULOKI_LINUX_OS_SULOKIDEF
#define SULOKI_64_PLATFORM_SULOKIDEF	//only 64 bit valid
#define SULOKI_PERSISTENT_STORAGE_PLATFORM_SULOKIDEF //by leveldb
#define SULOKI_URC_SERVICE_SULOKIDEF //urc service system
#define SULOKI_DUMP_SULOKIDEF

//function
#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
#define SULOKI_STDCALL_SULOKIDEF __stdcall
#else
#define SULOKI_STDCALL_SULOKIDEF
#endif
#define SULOKI_IN_SULOKIDEF
#define SULOKI_OUT_SULOKIDEF
#define SULOKI_INOUT_SULOKIDEF

//field key def
////msg field key
#define SULOKI_BUSINESSID_MSGFIELD_KEY_SULOKIDEF "businessid"
#define SULOKI_MESSAGEID_MSGFIELD_KEY_SULOKIDEF "messageid"
#define SULOKI_TYPE_MSGFIELD_KEY_SULOKIDEF "type"
#define SULOKI_SEQUENCEID_MSGFIELD_KEY_SULOKIDEF "sequenceid"
#define SULOKI_BODY_MSGFIELD_KEY_SULOKIDEF "body"
#define SULOKI_ERRCODE_MSGFIELD_KEY_SULOKIDEF "errcode"
#define SULOKI_ERRINFO_MSGFIELD_KEY_SULOKIDEF "errinfo"
#define SULOKI_URNAME_MSGFIELD_KEY_SULOKIDEF "urname"
#define SULOKI_URTYPE_MSGFIELD_KEY_SULOKIDEF "urtype"
#define SULOKI_URATTRIB_MSGFIELD_KEY_SULOKIDEF "urattr"
#define SULOKI_GROUPFROM_MSGFIELD_KEY_SULOKIDEF "groupfrom"		//source group, none mean owner
#define SULOKI_SERVICEFROM_MSGFIELD_KEY_SULOKIDEF "servicefrom"	//source service,none mean any one service of service
#define SULOKI_GROUPTO_MSGFIELD_KEY_SULOKIDEF "groupto"		//dest group, none mean owner
#define SULOKI_SERVICETO_MSGFIELD_KEY_SULOKIDEF "serviceto"	//dest service,none mean any one service of service
#define SULOKI_ROUTERS_MSGFIELD_KEY_SULOKIDEF "routers"
////normal key
#define SULOKI_REMOTEIP_KEY_SULOKIDEF "ip"
#define SULOKI_REMOTEPORT_KEY_SULOKIDEF "port"
#define SULOKI_ISSERVERKEY_KEY_SULOKIDEF "isserver"
#define SULOKI_USER_KEY_SULOKIDEF "user"
#define SULOKI_PASS_KEY_SULOKIDEF "pass"
#define SULOKI_MODELID_KEY_SULOKIDEF "modelid"
#define SULOKI_INPUT_KEY_SULOKIDEF "input"
#define SULOKI_OUTPUT_KEY_SULOKIDEF "output"
//field val def
#define SULOKI_TRUE_VAL_SULOKIDEF "true"
#define SULOKI_FALSE_VAL_SULOKIDEF "false"
//business id
#define SULOKI_SYSTEM_BUSINESSID_VAL_SULOKIDEF "system"
#define SULOKI_URC_BUSINESSID_VAL_SULOKIDEF "urc"
#define SULOKI_AUTH_BUSINESSID_VAL_SULOKIDEF "auth"
#define SULOKI_AI_BUSINESSID_VAL_SULOKIDEF "ai"
#define SULOKI_FRAMEWORK_BUSINESSID_VAL_SULOKIDEF "framework"
//message id
//SULOKI_SYSTEM_BUSINESSID_VAL_DEF
#define SULOKI_SYSTEM_TEST_MESSAGEID_VAL_SULOKIDEF "test"
#define SULOKI_SYSTEM_TIMEOUT_MESSAGEID_VAL_SULOKIDEF "timeout"
//#define SULOKI_SYSTEM_PTOP_MESSAGEID_VAL_SULOKIDEF "ptop"
#define SULOKI_SYSTEM_HEARTBEAT_MESSAGEID_VAL_SULOKIDEF "heartbeat"
#define SULOKI_SYSTEM_REGSERVICE_MESSAGEID_VAL_SULOKIDEF "regservice"
#define SULOKI_SYSTEM_UNREGSERVICE_MESSAGEID_VAL_SULOKIDEF "unregservice"
#define SULOKI_SYSTEM_WSCONNECTED_MESSAGEID_VAL_SULOKIDEF "wsconnected"
#define SULOKI_SYSTEM_WSDISCONNECTED_MESSAGEID_VAL_SULOKIDEF "wsdisconnected"
#define SULOKI_SYSTEM_WSDISCONNECT_MESSAGEID_VAL_SULOKIDEF "wsdisconnect"
//SULOKI_URC_BUSINESSID_VAL_SULOKIDEF
#define SULOKI_URC_ADD_MESSAGEID_VAL_SULOKIDEF "add"
#define SULOKI_URC_DEL_MESSAGEID_VAL_SULOKIDEF "del"
#define SULOKI_URC_UPDATE_MESSAGEID_VAL_SULOKIDEF "update"
#define SULOKI_URC_GET_MESSAGEID_VAL_SULOKIDEF "get"
//SULOKI_AUTH_BUSINESSID_VAL_DEF
#define SULOKI_AUTH_LOGIN_MESSAGEID_VAL_SULOKIDEF "login"
#define SULOKI_AUTH_LOGOUT_MESSAGEID_VAL_SULOKIDEF "logout"
//SULOKI_AI_BUSINESSID_VAL_SULOKIDEF
#define SULOKI_AI_WORK_MESSAGEID_VAL_SULOKIDEF "work" //手写数字识别
#define SULOKI_AI_WENZIZUOBIAO_MESSAGEID_VAL_SULOKIDEF "wenzizuobiao" //文字坐标查找
//SULOKI_SNN_BUSINESSID_VAL_DEF
#define SULOKI_SNN_REGEDGE_MESSAGEID_VAL_SULOKIDEF "regedge"
#define SULOKI_SNN_UNREGEDGE_MESSAGEID_VAL_SULOKIDEF "unregedge"
#define SULOKI_SNN_DOWNLOAD_MESSAGEID_VAL_SULOKIDEF "download"
#define SULOKI_SNN_GETVIDEOSRC_MESSAGEID_VAL_SULOKIDEF "getvideosrc"
#define SULOKI_SNN_SETVIDEOSRC_MESSAGEID_VAL_SULOKIDEF "setvideosrc"
#define SULOKI_SNN_NOTIFYPATTERN_MESSAGEID_VAL_SULOKIDEF "notifypattern"
#define SULOKI_SNN_GETALARMPATTERN_MESSAGEID_VAL_SULOKIDEF "getpattern"
#define SULOKI_SNN_REGALARMPATTERN_MESSAGEID_VAL_SULOKIDEF "regpattern"
#define SULOKI_SNN_UNREGALARMPATTERN_MESSAGEID_VAL_SULOKIDEF "unregpattern"
#define SULOKI_SNN_OPENMEDIAFAIL_MESSAGEID_VAL_SULOKIDEF "openmediafail"
#define SULOKI_SNN_MEDIASTOP_MESSAGEID_VAL_SULOKIDEF "mediastopped"
//message type
#define SULOKI_REQUEST_TYPE_VAL_SULOKIDEF "request"
#define SULOKI_RESPONSE_TYPE_VAL_SULOKIDEF "response"
#define SULOKI_NOTICE_TYPE_VAL_SULOKIDEF "notice"

//some length
#define SULOKI_MAXLEN_URNAME_SULOKIDEF 62
#define SULOKI_MAXLEN_MSGFLAG_SULOKIDEF 6
#define SULOKI_MAXLEN_MSGVERSION_SULOKIDEF 6
#define SULOKI_MAXLEN_BYTEORDER_SULOKIDEF 6
#define SULOKI_MAXLEN_SENDED_NET_SULOKIDEF 1024 * 1024 * 32

#ifdef SULOKI_WINDOWS_OS_SULOKIDEF
#define SULOKI_GETPROCADDR_SULOKIDEF(myFunc, tFunc, nameFunc, bFlag) myFunc = (tFunc)GetProcAddress(m_hinstLib, nameFunc);\
	if (myFunc == NULL && bFlag)\
		{\
		std::stringstream sStream; sStream << "GetProcAddress " << nameFunc << " error" << std::endl; \
		SULOKI_WRITELOG_SULOKI(Suloki::Log::FILETYPE, sStream.str(), Suloki::LOG_ERROR_LEVEL);\
		throw Suloki::Exception(sStream.str());\
		}
#else
#define SULOKI_GETPROCADDR_SULOKIDEF(myFunc, tFunc, nameFunc, bFlag) myFunc = (tFunc)dlsym(m_hinstLib, nameFunc);\
	if (myFunc == NULL && bFlag)\
		{\
		std::stringstream sStream; sStream << "dlsym " << nameFunc << " error" << std::endl; throw Suloki::Exception(sStream.str());\
		}
/*
szError = dlerror();
if (myFunc == NULL || szError != NULL && bFlag)
{
std::stringstream sStream; sStream << "dlsym " << nameFunc << " error" << std::endl; throw Suloki::Exception(sStream.str());
}
*/
#endif

/*
#ifdef SULOKI_64_PLATFORM_DEF
#pragma  pack (push,8)
#else
#pragma  pack (push,4)
#endif
struct _SulokiMessage {
	char m_flag[SULOKI_MAXLEN_MSGFLAG_DEF+2];//suloki
	char m_version[SULOKI_MAXLEN_MSGVERSION_DEF+2];//1.0
	char m_byteorder[SULOKI_MAXLEN_BYTEORDER_DEF + 2];//little 
	SulokiInt m_totalLen;
	char m_urName[SULOKI_MAXLEN_URNAME_DEF + 2];
	SulokiInt m_urType;
	SulokiInt m_urAttrib;
	SulokiInt m_businessId;
	SulokiInt m_messageId;
	SulokiInt m_msgType;
	SulokiInt m_msgSequenceid;
	SulokiInt m_routerLen;
	char* m_pRouterBuf;
	SulokiInt m_bodyLen;
	char* m_pBodyBuf;
};
struct _SulokiContext {
	SulokiInt m_totalLen;
	char m_urName[SULOKI_MAXLEN_URNAME_DEF + 2];
};
#pragma pack(pop)
typedef struct _SulokiMessage SulokiMessage;
typedef struct _SulokiContext SulokiContext;

typedef SulokiInt(SULOKI_STDCALL *SulokiUrcAddData)(SULOKI_IN_DEF const char* strUrName, SULOKI_IN_DEF char* pDataBuf, SULOKI_IN_DEF SulokiInt dataLen, SULOKI_IN_DEF SulokiInt attrib, SULOKI_IN_DEF bool bDir);
typedef SulokiInt(SULOKI_STDCALL *SulokiUrcAddSmartPtr)(SULOKI_IN_DEF const char* strUrName, SULOKI_IN_DEF void* pSmartTr, SULOKI_IN_DEF bool bDir);
#ifdef SULOKI_64_PLATFORM_DEF
#pragma  pack (push,8)
#else
#pragma  pack (push,4)
#endif
struct _SulokiUrcInterface {
	SulokiUrcAddData m_funcAddData;
	SulokiUrcAddSmartPtr  m_funcAddSmartPtr;
};
#pragma pack(pop)
typedef struct _SulokiUrcInterface SulokiUrcInterface;

typedef SulokiInt(SULOKI_STDCALL *SulokiServiceInit)(SULOKI_IN_DEF SulokiUrcInterface* pUrcInterface, SULOKI_IN_DEF const char* pGroupName, SULOKI_IN_DEF const char* pServiceName);
typedef SulokiInt(SULOKI_STDCALL *SulokiServiceStart)(void);
typedef SulokiInt(SULOKI_STDCALL *SulokiServiceHandle)(SULOKI_IN_DEF SulokiMessage* pMessage, SULOKI_IN_DEF SulokiContext* pContext);
typedef SulokiInt(SULOKI_STDCALL *SulokiServiceStop)(void);
typedef SulokiInt(SULOKI_STDCALL *SulokiServiceClear)(void);
#ifdef SULOKI_64_PLATFORM_DEF
#pragma  pack (push,8)
#else
#pragma  pack (push,4)
#endif
struct _SulokiServiceInterface {
	SulokiServiceInit m_funcInit;
	SulokiServiceStart m_funcStart;
	SulokiServiceHandle m_funcHandle;
	SulokiServiceStop m_funcStop;
	SulokiServiceClear m_funcClear;
};
#pragma pack(pop)
typedef struct _SulokiServiceInterface SulokiServiceInterface;
*/

#endif
