#include "stdafx.h"
#include "FSOLIDCreateProtrusionSweep.h"

#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_SweptBuilder.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/GeometricUtilities_Limits.hxx>
#include <NXOpen/GeometricUtilities_Extend.hxx>
#include <NXOpen/Direction.hxx>

#include "Part.h"
#include "FSketch.h"

#include <iostream>
using namespace std;

int FSOLIDCreateProtrusionSweep::_proSweepCount = 1;

FSOLIDCreateProtrusionSweep::FSOLIDCreateProtrusionSweep(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
	: Feature(part,ugfeature, featureList)
{
	_fType = SOLID_Create_Protrusion_Sweep;
}

FSOLIDCreateProtrusionSweep::~FSOLIDCreateProtrusionSweep(void)
{
}

void FSOLIDCreateProtrusionSweep::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSOLIDCreateProtrusionSweep Info" << endl;
		
	int pro = 0;
	NXOpen::NXString proSketJID;
	NXOpen::NXString guiSketJID;
	vector<NXOpen::Features::Feature* > parentFeat = _ugFeature->GetParents();
	
	cout << "Parent Features: " << endl;

	for(int i = 0; i<parentFeat.size(); i++){
		cout << "	" << parentFeat.at(i)->JournalIdentifier().GetUTF8Text();
		if(!strcmp(parentFeat.at(i)->FeatureType().GetUTF8Text(),"SKETCH")){
			if(pro == 0){
				proSketJID = parentFeat.at(i)->JournalIdentifier();
				pro++;
				cout << " <- Sweep Profile Sketch";
			}else if(pro == 1){
				guiSketJID = parentFeat.at(i)->JournalIdentifier();
				cout << " <- Sweep Guideline Sketch";
			}else{
				cout << endl << "More than two Parent Sketches in this Sweep Feature -> Cannot Translate!" << endl << endl;
				return;
			}
			cout << endl;
		}
	}
	
//	NXOpen::Features::SweptBuilder* sweptBuilder = _featureList->CreateSweptBuilder(_ugFeature);
	
	_pProFSket = (FSketch *)(GetPart()->Get_pFeatureByJID(proSketJID) );
	_pGuiFSket = (FSketch *)(GetPart()->Get_pFeatureByJID(guiSketJID) );

	//----------  Set Result_Object_Name  ----------//
	char buffer[20];
	_itoa( _proSweepCount++, buffer, 10 );
	SetName("ProSweep" + (string)buffer);

	cout << endl;
}


void FSOLIDCreateProtrusionSweep::ToTransCAD()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FSOLIDCreateProtrusionSweep to TransCAD" << endl;
	
	bstr_t profileName( GetProSket()->GetName().c_str());
	bstr_t guideName( GetGuideSketch()->GetName().c_str());

	TransCAD::IReferencePtr spProfile = GetPart()->_spPart->SelectObjectByName(profileName);
	TransCAD::IReferencePtr spGuide = GetPart()->_spPart->SelectObjectByName(guideName);
	cout << "Selected Profile Sketch in TransCAD: " << spProfile->GetReferenceeName() << endl;
	cout << "Selected Guideline Sketch in TransCAD: " << spGuide->GetReferenceeName() << endl;

	// Create a Protrusion Sweep
	GetPart()->_spFeatures->AddNewSolidProtrusionSweepFeature(GetName().c_str(), spProfile, spGuide);
	//GetPart()->_spFeatures->AddNewSolidProtrusionSweepFeature((_bstr_t)"Sweep1", spProfile, spGuide);
	cout << "FSOLIDCreateProtrusionSweep: " << GetName().c_str() << " Translated" << endl << endl;

}