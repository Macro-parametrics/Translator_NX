#include "stdafx.h"
#include "FSOLIDCreateCutExtrude.h"

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

int FSOLIDCreateCutExtrude::_cutExtrudeCount = 1;

FSOLIDCreateCutExtrude::FSOLIDCreateCutExtrude(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
	: Feature(part,ugfeature, featureList)
{
	_fType = SOLID_Create_Cut_Extrude;
}

FSOLIDCreateCutExtrude::~FSOLIDCreateCutExtrude(void)
{
}

void FSOLIDCreateCutExtrude::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSOLIDCreateCutExtrude Info" << endl;
	
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
	
	cout << "Start Condition: ";
	if(extrudeBuilder->Limits()->StartExtend()->TrimType() == NXOpen::GeometricUtilities::Extend::ExtendTypeThroughAll){
		_StaType = End_ThroughAll;
		_dStart = 0;
		cout << "Through All" << endl;
	}else{
		_StaType = End_Dimension;
		_dStart = -(extrudeBuilder->Limits()->StartExtend()->Value()->Value());	// set _dStart
		cout << "Blind" << endl;
		cout << "Start Value: " << _dStart << endl;
	} 
	
	cout << "End Condition: ";
	if(extrudeBuilder->Limits()->EndExtend()->TrimType() == NXOpen::GeometricUtilities::Extend::ExtendTypeThroughAll){
		_EndType = End_ThroughAll;
		_dEnd = 0;
		cout << "Through All" << endl;
	}else{
		_EndType = End_Dimension;
		_dEnd = extrudeBuilder->Limits()->EndExtend()->Value()->Value();	// set _dEnd
		cout << "Blind" << endl;
		cout << "End Value: " << _dEnd << endl;
	}

	cout << "Flip Status: ";
	if(extrudeBuilder->Direction()->Sense() == NXOpen::Sense::SenseReverse){
		_bFlip1 = VARIANT_TRUE;
		cout << "True" << endl;
	}else{
		_bFlip1 = VARIANT_FALSE;
		cout << "False" << endl;
	}
	//-------------------  Set Result_Object_Name  -------------------//
	char buffer[20];
	_itoa( _cutExtrudeCount++, buffer, 10 );
	SetName("CutExtrude" + (string)buffer);

	cout << endl;
}

void FSOLIDCreateCutExtrude::ToTransCAD()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FSOLIDCreateCutExtrude to TransCAD" << endl;
	
	bstr_t baseSketchName( _pProFSket->GetName().c_str() );

	TransCAD::IReferencePtr spBaseSketch = GetPart()->_spPart->SelectObjectByName(baseSketchName);
	cout << "Selected Reference Sketch in TransCAD: " << spBaseSketch->GetReferenceeName() << endl;
	
	TransCAD::StdExtrudeEndType startCond;
	TransCAD::StdExtrudeEndType endCond;
	if(_StaType == End_ThroughAll){
		startCond = TransCAD::ThroughAll;
	}else if(_StaType == End_Dimension){
		startCond = TransCAD::Blind;
	}
	if(_EndType == End_ThroughAll){
		endCond = TransCAD::ThroughAll;
	}else if(_EndType == End_Dimension){
		endCond = TransCAD::Blind;
	}
	GetPart()->_spFeatures->AddNewSolidCutExtrudeFeature(GetName().c_str(), spBaseSketch, _dStart, startCond , _dEnd, endCond , _bFlip1);
	cout << "FSOLIDCreateCutExtrude: " << GetName().c_str() << " Translated" << endl << endl;
}



