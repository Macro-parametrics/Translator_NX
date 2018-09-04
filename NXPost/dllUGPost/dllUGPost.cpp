//dllUGPost
#include "stdafx.h"
#include "dllUGPost.h"
#include "Definition.h"
#include "UGReferenceManager.h"

// Mandatory UF Includes
#include <uf.h>
#include <uf_object_types.h>

// Internal+External Includes
#include <NXOpen/Session.hxx>

// Std C++ Includes
#include <iostream>

// UGPost class Includes
#include "Part.h"


using namespace std;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

TransCAD::IApplicationPtr g_spApplication;
NXOpen::Session * nxSession = NXOpen::Session::GetSession();
UGReferenceManager * g_pRefManager = NULL;

dllUGPost_API int UGPostStartService(void)
{
	g_pRefManager = new UGReferenceManager();

	int status = false;
	HRESULT hr = CoInitialize(NULL);		// COM Automation API initialization
	UF_initialize();

	if ( FAILED(hr) || !nxSession )
	{
		MessageBoxA(NULL, "Failed to initialize UG NXOpen API", "Warning message", MB_OK);
		return status;
	}

	/** ---- TransCAD instance »ý¼º ---- **/
	hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));

	/** TransCAD initialize error check **/
	if ( FAILED(hr) )	status = 0;

	/** Visualize TransCAD **/
	if ( !g_spApplication->Visible )
	{
		g_spApplication->Visible = VARIANT_TRUE;
		MessageBoxA(NULL, "Is there a model in TransCAD ?", "Warning message", MB_OK);
	}
	

	return status;
}

dllUGPost_API int UGPostTranslate(char * nxFileName)
{
	int status = true;
	
	Part * pPart;

	pPart = new Part(g_spApplication, nxFileName, nxSession);

	pPart->GetInfo();
	pPart->ToUG();

	delete pPart;	// delete Part class

	g_spApplication.Release();	// Clear TransCAD

	CoUninitialize();	// COM Automation API Uninitialization
	UF_terminate();

	if (g_pRefManager)
		delete g_pRefManager;

	return status;
}