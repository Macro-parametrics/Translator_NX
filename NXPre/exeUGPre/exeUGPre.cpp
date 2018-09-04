//NX10_NXOpenCPP_Wizard1

#include "Part.h"

// Mandatory UF Includes
#include <uf.h>
#include <uf_object_types.h>

// Internal+External Includes
#include <NXOpen/Session.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/NXException.hxx>

// Std C++ Includes
#include <iostream>
#include <sstream>

using std::string;
using std::exception;
using std::stringstream;
using std::endl;
using std::cout;
using std::cerr;

TransCAD::IApplicationPtr g_spApplication;
//------------------------------------------------------------------------------
// Open C error handling
//------------------------------------------------------------------------------
#define UF_CALL(X) (report_error( __FILE__, __LINE__, #X, (X)))
int report_error( char *file, int line, char *call, int code)
{
    if (code) 
	{

		stringstream errmsg;
		errmsg << "Error " << code << " in " << file << " at line " << line << endl;
		errmsg << call << endl;
		cerr << errmsg.str();
		throw NXOpen::NXException::Create(code);
	}
    return(code);
}

//------------------------------------------------------------------------------
// Entry point for unmanaged external NXOpen C/C++ programs
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	int status = false;
	HRESULT hr = CoInitialize(NULL);		// COM Automation API initialization
	if(FAILED(hr)) {
		cout << "[ Failed to initialize COM library ]" << endl;
		return status;
	}
	cout << "TransCAD Automation API initialized" << endl;

	/* ---- NXOpen initialize ---- */
	NXOpen::Session *NXSession = NXOpen::Session::GetSession();			// UG API Initialization
	if (!NXSession) {
		cout << "[ Failed to initialize NXOpen API ]" << endl;
		return 0;
	}
	cout << "NXOpen API initialized" << endl;

	/** ---- TransCAD instance »ý¼º ---- **/
	hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
	if (FAILED(hr)) {
		cout << "[ Failed to Create TransCAD Application ]" << endl;
		return 0;
	}
	cout << "TransCAD Application Created" << endl;

	/** Visualize TransCAD **/
	if ( !g_spApplication->Visible ){
		g_spApplication->Visible = VARIANT_TRUE;
	}
	cout << "Visualized TransCAD!" << endl << endl << endl;
	
	Part * pPart = new Part ( g_spApplication, argv[1], NXSession);


	pPart->GetUGInfo();	
	pPart->ToTransCAD();

	delete pPart;

	g_spApplication.Release();
	cout << endl << "//////////////Translation Finished!//////////////"<<endl;
	::CoUninitialize();
	return 0;

}



