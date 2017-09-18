#include "stdafx.h"
#include "FSOLIDCreateProtrusionRevolve.h"

#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_RevolveBuilder.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/GeometricUtilities_Limits.hxx>
#include <NXOpen/GeometricUtilities_Extend.hxx>
#include <NXOpen/Direction.hxx>
#include <NXOpen/Axis.hxx>

#include "Part.h"
#include "FSketch.h"
#include "FSKETCHCreate2DCenterline.h"

#include <iostream>
using namespace std;

int FSOLIDCreateProtrusionRevolve::_nProRevCnt = 1;

FSOLIDCreateProtrusionRevolve::FSOLIDCreateProtrusionRevolve(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
	: Feature(part,ugfeature, featureList)
{
	_fType = SOLID_Create_Protrusion_Revolve;
	
	_bFlip = VARIANT_FALSE;
	_StaType=End_Dimension;
	_EndType=End_Dimension;
}

FSOLIDCreateProtrusionRevolve::~FSOLIDCreateProtrusionRevolve(void)
{
}

void FSOLIDCreateProtrusionRevolve::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSOLIDCreateProtrusionRevolve Info" << endl;
		
	NXOpen::NXString refSketJID;
	vector<NXOpen::Features::Feature* > parentFeat = _ugFeature->GetParents();
	
	cout << "Parent Features: " << endl;

	for(int i = 0; i<parentFeat.size(); i++){
		cout << "	" << parentFeat.at(i)->JournalIdentifier().GetUTF8Text();
		if(!strcmp(parentFeat.at(i)->FeatureType().GetUTF8Text(),"SKETCH")){
			refSketJID = parentFeat.at(i)->JournalIdentifier();
			cout << " <- Reference Sketch";
		}
		cout << endl;
	}
	
	NXOpen::Features::RevolveBuilder* revolveBuilder = _featureList->CreateRevolveBuilder(_ugFeature);
	
	_pProFSket = (FSketch *)(GetPart()->Get_pFeatureByJID(refSketJID) );
	
	_dStaAng = revolveBuilder->Limits()->EndExtend()->Value()->Value();	// set start angle
	_dEndAng = revolveBuilder->Limits()->StartExtend()->Value()->Value();	// set end angle

	cout << "Revolve Start Angle: " << _dStaAng << endl;
	cout << "Revolve End Angle: " << _dEndAng << endl;

	//-------------------  Set Result_Object_Name  -------------------//
	char buffer[20];
	_itoa( _nProRevCnt++, buffer, 10 );
	SetName("ProRevolve" + (string)buffer);

	cout << endl;
}

void FSOLIDCreateProtrusionRevolve::ToTransCAD()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FSOLIDCreateProtrusionRevolve to TransCAD" << endl;
	
	bstr_t proSketName( GetProSket()->GetName().c_str() );
	TransCAD::IReferencePtr spProSket = GetPart()->_spPart->SelectObjectByName(proSketName);
    cout << "Selected Reference Sketch in TransCAD: " << spProSket->GetReferenceeName() << endl;

	// Create a protrusion revolve feature with the sketch
	GetPart()->_spFeatures->AddNewSolidProtrusionRevolveFeature(GetName().c_str(),spProSket,
		_dStaAng, TransCAD::StdRevolveEndType_Blind,
		_dEndAng, TransCAD::StdRevolveEndType_Blind, false);
	cout << "FSOLIDCreateProtrusionRevolve: " << GetName().c_str() << " Translated" << endl << endl;
}
