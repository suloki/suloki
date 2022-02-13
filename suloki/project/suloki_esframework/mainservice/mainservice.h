// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� MAINSERVICE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// MAINSERVICE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
// �����Ǵ� mainservice.dll ������
class MAINSERVICE_API Cmainservice {
public:
	Cmainservice(void);
	// TODO:  �ڴ�������ķ�����
};

extern MAINSERVICE_API int nmainservice;

MAINSERVICE_API int fnmainservice(void);
*/