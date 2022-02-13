// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 MAINSERVICE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// MAINSERVICE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef MAINSERVICE_EXPORTS
#define MAINSERVICE_API __declspec(dllexport)
#else
#define MAINSERVICE_API __declspec(dllimport)
#endif

#include <memory>
#include <iostream>
#include <string>

#include "suloki.h"

#ifdef __cplusplus
extern "C"
{
#endif

MAINSERVICE_API Suloki::Ret ServiceInit(SULOKI_IN_SULOKIDEF Suloki::UrcInterface* pUrcInterface, SULOKI_IN_SULOKIDEF std::string groupName, SULOKI_IN_SULOKIDEF std::string  serviceName);
MAINSERVICE_API Suloki::Ret ServiceStart(void);
MAINSERVICE_API Suloki::Ret ServiceHandle(SULOKI_IN_SULOKIDEF Suloki::SmartPtr<Suloki::Message> msgSmart, SULOKI_IN_SULOKIDEF Suloki::SmartPtr<Suloki::Context> paraSmart, SULOKI_IN_SULOKIDEF Suloki::SmartPtr<Suloki::Message>& resSmart, SULOKI_IN_SULOKIDEF Suloki::SmartPtr<Suloki::Context>& paraOutSmart);
MAINSERVICE_API Suloki::Ret ServiceStop(void);
MAINSERVICE_API Suloki::Ret ServiceClear(void);

#ifdef __cplusplus
}
#endif

/*
// 此类是从 mainservice.dll 导出的
class MAINSERVICE_API Cmainservice {
public:
	Cmainservice(void);
	// TODO:  在此添加您的方法。
};

extern MAINSERVICE_API int nmainservice;

MAINSERVICE_API int fnmainservice(void);
*/