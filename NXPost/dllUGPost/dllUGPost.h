#pragma once

class UGReferenceManager;
extern UGReferenceManager * g_pRefManager;

#ifdef UGPOST_DLL_EXPORTS
#define dllUGPost_API extern "C" __declspec(dllexport)
#else
#define dllUGPost_API extern "C" __declspec(dllimport)
#endif


// 이 클래스는 UGpost_DLL.dll에서 내보내는 함수입니다.
dllUGPost_API int UGPostStartService(void);
dllUGPost_API int UGPostTranslate(char * nxFileName);