#include "stdafx.h"
#include "Part.h"
#include "UGReferenceManager.h"
#include "dllUGPost.h"

#include <iostream>

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Part.hxx>
#include <NXOpen\PartCollection.hxx>	// For _nxPart
#include <NXOpen\PartSaveStatus.hxx>
#include <NXOpen\FileNew.hxx>
#include <NXOpen\Unit.hxx>
#include <NXOpen\Body.hxx>
#include <NXOpen\BodyCollection.hxx>

/** UG Post's header files **/
#include "Feature.h"
#include "FDatumPlane.h"
#include "FDatumPlaneOffset.h"
#include "FSketch.h"
#include "FeatureSOLIDCreateProtrusionExtrude.h"
#include "FeatureSOLIDCreateCutExtrude.h"
#include "FeatureSOLIDCreateFilletConstant.h"
#include "FeatureSOLIDCreateProtrusionSweep.h"
//#include "FeatureSOLIDCreateProtrusionSweep_swept.h"
#include "FeatureSOLIDCreateProtrusionRevolve.h"
#include "FeatureSOLIDCreateCutRevolve.h"
#include "FeatureSOLIDCreateChamfer.h"
#include "FeatureSOLIDCreatePatternRectangular.h"
#include "FeatureSOLIDCreatePatternCircular.h"
#include "FeatureSOLIDCreateCounterbored.h"
#include "FeatureSOLIDCreateCountersunk.h"
#include "FeatureSOLIDCreateLoft.h"

using namespace std;

Part::Part(TransCAD::IApplicationPtr tc, char * nxFileName, NXOpen::Session * nxSession)
{
	
	_spDocument = tc->ActiveDocument;
	
	/** Error 처리 **/
	if ( _spDocument == NULL ) return;

	/** TransCAD infomation **/
	_spPart			= _spDocument->Part;
	_spFeatures		= _spPart->Features;
	_tcPartName		= _spDocument->FullName;

	/** Create a UG part **/
	_nxSession		= nxSession;
	_nxPartName		= string(nxFileName);
	_nxPart			= CreateNXPart();
}

Part::~Part(void) {
	clear();//
}

void Part::GetInfo() /*************************** START getinfo *****************************/
{
	// DEBUG printf("<<<< Start to GetInfo process >>>>\n");

	/** TransCAD에 있는 feature의 수 만큼 반복 --> 각각 Feature로 정의 **/
	for ( int i = 1; i <= _spFeatures->Count; ++i )
	{
		/** CreateFeature를 통해서, Type에 맞는 Feature 생성 후, m_featureList에 넣어줌 **/
		Feature * pFeature = CreateFeature(_spFeatures->Item[i]); /***********************/
		
		if ( !pFeature )
			cout << "** " << _spFeatures->Item[i]->Name << " not yet supported ! **" << endl;
	}

	/** _featureList에 들어있는 feature의 수만큼 반복 & GetInfo **/
	for ( int i = 0; (unsigned)i < GetSize(); ++i )
	{
		cout << "\nIndex[" << i << "] = " << GetFeature(i)->GetFeatureName();

		/** Feature class의 virtual GetInfo함수를 통해서 feature type의 GetInfo함수로 이동 **/
		GetFeature(i)->GetInfo();
	}
}

Feature * Part::CreateFeature(TransCAD::IFeaturePtr spFeature)
{
	Feature * pFeature = 0;

	g_pRefManager->AddNewFeature(spFeature);
	
	
	/** TransCAD에서 제공하는 feature type을 확인하여 적절한 feature 생성 함수로 이동 **/
	switch ( spFeature->Type )
	{
	case TransCAD::StdDefaultDatumPlaneFeature :
		pFeature = new FDatumPlane(this, spFeature);
		//cout << " ->  FDatumPlane class constructed !\n" << endl;
		break;
	case TransCAD::StdDatumPlaneOffsetFeature:
		pFeature = new FDatumPlaneOffset(this, spFeature);
		//cout << " ->  FDatumPlaneOffset class constructed !\n" << endl;
		break;
	case TransCAD::StdSketchFeature:
		pFeature = new FSketch(this, spFeature);
		//cout << " ->  FSketch class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidProtrusionExtrudeFeature:
		pFeature = new FeatureSOLIDCreateProtrusionExtrude(this, spFeature);
		//cout << " ->  FeatureSOLIDCreateProtrusionExtrude class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidCutExtrudeFeature:
		pFeature = new FeatureSOLIDCreateCutExtrude(this, spFeature);
		//cout << " ->  FeatureSOLIDCreateCutExtrude class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidFilletConstantFeature:
		pFeature = new FeatureSOLIDCreateFillet(this, spFeature);
		//cout << " ->  FeatureSOLIDCreateFilletConstant class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidProtrusionRevolveFeature:
		pFeature = new FeatureSOLIDCreateProtrusionRevolve(this, spFeature);
		//cout << " ->  FeatureSOLIDCreateProtrusionRevolve class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidCutRevolveFeature:
		pFeature = new FeatureSOLIDCreateCutRevolve(this, spFeature);
		//cout << " ->  FeatureSOLIDCreateCutRevolve class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidChamferFeature:
		pFeature = new FeatureSOLIDCreateChamfer(this, spFeature);
		//cout << " ->  FeatureSOLIDCreateFilletingChamfer class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidProtrusionSweepFeature:
		pFeature = new FeatureSOLIDCreateProtrusionSweep(this, spFeature);
		//cout << " ->  FeatureSOLIDCreateProtrusionSweep class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidOperatePatternRectangularFeature:
		pFeature = new FeatureSOLIDCreatePatternRectangular(this, spFeature);
		//cout << " ->  FeatureSOLIDCreatePatternRectangular class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidOperatePatternCircularFeature:
		pFeature = new FeatureSOLIDCreatePatternCircular(this, spFeature);
		//cout << " ->  FeatureSOLIDCreatePatternCircular class constructed !\n" << endl;
		break;
	case TransCAD::StdSolidHoleCounterboredFeature:
		pFeature=new FeatureSOLIDCreateCounterbored(this,spFeature);
		break;
	case TransCAD::StdSolidHoleCountersunkFeature:
        pFeature=new FeatureSOLIDCreateCountersunk(this,spFeature);
        //cout<<"->sunk"<<endl;
        break;
	case TransCAD::StdSolidLoftSectionsFeature:
		pFeature=new FeatureSOLIDCreateLoft(this,spFeature);
		break;
	} //// ★★★★★★★★★★★★★★솔루션 다시 빌드

	if (pFeature)
		_featureList.push_back(pFeature);

	return pFeature;
}

void Part::ToUG()
{
	// DEBUG printf("\n<<<< Start to ToUG process >>>>\n");

	/** m_featureList에 있는 feature를 확인하여 UG feature로 생성 **/
	for ( int i = 0; (unsigned)i < GetSize(); ++i )
	{
		_ToUGIndex = i;
		GetFeature(i)->ToUG();
		cout << "\nIndex[" << i << "] " << GetFeature(i)->GetFeatureName() << " feature is translated." << endl;
	}

	/** Save a UG part file **/
	SaveNXPart();
}

void Part::clear()
{
	vector<Feature *>::iterator i = _featureList.begin();

	while ( i != _featureList.end() )
	{
		delete *i;
		i++;
	}

	_featureList.clear();
}

Feature * Part::GetFeatureByName(string name)
{
	for ( size_t i = 0; (unsigned)i < GetSize(); ++i )
	{
		Feature * pFeature = GetFeature((int)i);

		if ( name == pFeature->GetFeatureName() )
			return pFeature;
	}

	return NULL;
}

vector<NXOpen::Body *> Part::GetNXBodyList()
{
	try
	{
		using namespace NXOpen;

		BodyCollection * bodyInPart = _nxPart->Bodies();
		BodyCollection::iterator itr;	
	
		vector<Body *> bodyList;
		
		for ( itr = bodyInPart->begin(); itr != bodyInPart->end(); ++itr )
		{
			bodyList.push_back(* itr);
		}
		return bodyList;
	}
	
	catch ( const NXOpen::NXException &ex)
	{
		cout << "   Error location [ Part -> GetNXBodyList func. ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}


NXOpen::Part * Part::CreateNXPart()
{
	try
	{
		NXOpen::FileNew * newFile;
	
		/** UG part file setting **/
		newFile = _nxSession->Parts()->FileNew();
		newFile->SetTemplateFileName("model-plain-1-mm-template.prt");
		newFile->SetApplicationName("ModelTemplate");
		newFile->SetUnits(NXOpen::Part::UnitsMillimeters);
		newFile->SetNewFileName(_nxPartName);
		newFile->Commit();
	
		/** Create a part file **/
		NXOpen::Part * pPart = _nxSession->Parts()->Work();
	
		newFile->Destroy();

		_nxSession->ApplicationSwitchImmediate("UG_APP_MODELING");

		return pPart;
	}

	catch ( const NXOpen::NXException &ex )
	{
		MessageBoxA(NULL, "Same prt file already exist", "Warning message", MB_OK);
		cout << "   Error location [ Part -> CreateNXPart func. ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
		exit(1);
	}
}

void Part::SaveNXPart()
{
	try
	{
		using namespace NXOpen;

		PartSaveStatus * fileSave;
		fileSave = _nxPart->Save(BasePart::SaveComponentsTrue, BasePart::CloseAfterSaveFalse);

		delete fileSave;
		cout << endl << _nxPartName << " is saved.." << endl;
	}

	catch ( const NXOpen::NXException &ex )
	{
		cout << "   Error location [ Part -> SaveNXPart func. ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}