#include "stdafx.h"
#include "FSOLIDCreateProtrusionExtrude.h"

#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_ExtrudeBuilder.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/GeometricUtilities_Limits.hxx>
#include <NXOpen/GeometricUtilities_Extend.hxx>
#include <NXOpen/Direction.hxx>

#include "Part.h"
#include "FSketch.h"

#include <iostream>
using namespace std;

int FSOLIDCreateProtrusionExtrude::_proExtCnt = 1;

FSOLIDCreateProtrusionExtrude::FSOLIDCreateProtrusionExtrude(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
	: Feature(part,ugfeature, featureList)
{
	_fType = SOLID_Create_Protrusion_Extrude;
}

FSOLIDCreateProtrusionExtrude::~FSOLIDCreateProtrusionExtrude(void)
{
}

void FSOLIDCreateProtrusionExtrude::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSOLIDCreateProtrusionExtrude Info" << endl;
	
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
	
	NXOpen::Features::ExtrudeBuilder* extrudeBuilder = _featureList->CreateExtrudeBuilder(_ugFeature);
	
	_pProFSket = (FSketch *)(GetPart()->Get_pFeatureByJID(refSketJID) );
	
	_dStart = extrudeBuilder->Limits()->EndExtend()->Value()->Value();	// set _dStart
	_dEnd = -(extrudeBuilder->Limits()->StartExtend()->Value()->Value());	// set _dEnd

	cout << "Extrude Start Value: " << _dStart << endl;
	cout << "Extrude End Value: " << _dEnd << endl;

	cout << "Flip Status: ";
	if(extrudeBuilder->Direction()->Sense() == NXOpen::Sense::SenseReverse){
		_bFlip = VARIANT_TRUE;
		cout << "True" << endl;
	}else{
		_bFlip = VARIANT_FALSE;
		cout << "False" << endl;
	}// set _ flipms

	//-------------------  Set Result_Object_Name  -------------------//
	char buffer[20];
	_itoa( _proExtCnt++, buffer, 10 );
	SetName("ProExtrude" + (string)buffer);

	cout << endl;
}


void FSOLIDCreateProtrusionExtrude::ToTransCAD()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FSOLIDCreateProtrusionExtrude to TransCAD" << endl;
	
	bstr_t baseSketchName( GetProSket()->GetName().c_str() );

	TransCAD::IReferencePtr spBaseSketch = GetPart()->_spPart->SelectObjectByName(baseSketchName);
    cout << "Selected Reference Sketch in TransCAD: " << spBaseSketch->GetReferenceeName() << endl;
	
	// Create a protrusion extrude feature with the sketch
	GetPart()->_spFeatures->AddNewSolidProtrusionExtrudeFeature(GetName().c_str(), spBaseSketch, _dStart, TransCAD::Blind, _dEnd, TransCAD::Blind, _bFlip);

	cout << "FSOLIDCreateProtrusionExtrude: " << GetName().c_str() << " Translated" << endl << endl;

}