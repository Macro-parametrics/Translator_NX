#include "stdafx.h"
#include "FSOLIDOperatePatternCircular.h"

#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/Features_SelectFeatureList.hxx>
#include <NXOpen/Features_PatternFeatureBuilder.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/Axis.hxx>
#include <NXOpen/Direction.hxx>

#include "Part.h"

#include <iostream>
using namespace std;

int FSOLIDOperatePatternCircular::_patCirCnt = 1;

FSOLIDOperatePatternCircular::FSOLIDOperatePatternCircular(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
	: Feature(part,ugfeature, featureList)
{
	_fType = SOLID_Operate_Pattern_Circular;
}

FSOLIDOperatePatternCircular::~FSOLIDOperatePatternCircular(void)
{
}


void FSOLIDOperatePatternCircular::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSOLIDOperatePatternCircular Info" << endl;
	
	NXOpen::NXString refFeatJID;
	NXOpen::Features::PatternFeatureBuilder *patternBuilder = _featureList->CreatePatternFeatureBuilder(
		_ugFeature);
	vector<NXOpen::Features::Feature *> patternRefFeatures = patternBuilder->FeatureList()->GetArray();
	
	cout << "Reference Features: " << endl;

	//Multi Feature for target body
	for(int i = 0; i<patternRefFeatures.size(); i++){
		refFeatJID = patternRefFeatures.at(i)->JournalIdentifier();
		_spTargetFeatures.push_back((Feature *)(GetPart()->Get_pFeatureByJID(refFeatJID)));
		cout << "	" << patternRefFeatures.at(i)->JournalIdentifier().GetUTF8Text() << endl;
		string temp = refFeatJID.GetUTF8Text();
	}

	OrgX = patternBuilder->PatternService()->CircularDefinition()->RotationAxis()->Origin().X;
	OrgY = patternBuilder->PatternService()->CircularDefinition()->RotationAxis()->Origin().Y;
	OrgZ = patternBuilder->PatternService()->CircularDefinition()->RotationAxis()->Origin().Z;

	cout <<	"Pattern Origin Point: (" << OrgX << " , " << OrgY << " , " << OrgZ << ")" << endl; 		

	DirX = patternBuilder->PatternService()->CircularDefinition()->RotationAxis()->DirectionVector().X;
	DirY = patternBuilder->PatternService()->CircularDefinition()->RotationAxis()->DirectionVector().Y;
	DirZ = patternBuilder->PatternService()->CircularDefinition()->RotationAxis()->DirectionVector().Z;

	cout <<	"Pattern Direction Vector: (" << DirX << " , " << DirY << " , " << DirZ << ")" << endl; 		
	
	AngularNumber = patternBuilder->PatternService()->CircularDefinition()->AngularSpacing()->NCopies()->Value();
	AngularSpacing = patternBuilder->PatternService()->CircularDefinition()->AngularSpacing()->PitchAngle()->Value();
	
	cout << "Circular Pattern Angular Copies: " << AngularNumber << endl;
	cout << "Circular Pattern Angular Spacing: " << AngularSpacing << endl;
	
	RadialNumber = patternBuilder->PatternService()->CircularDefinition()->RadialSpacing()->NCopies()->Value();
	RadialSpacing = patternBuilder->PatternService()->CircularDefinition()->RadialSpacing()->PitchDistance()->Value();
	
	cout << "Circular Pattern Radial Copies: " << RadialNumber << endl;
	cout << "Circular Pattern Radial Spacing: " << RadialSpacing << endl;

	//-------------------  Set Result_Object_Name  -------------------//
	char buffer[20];
	_itoa( _patCirCnt++, buffer, 10 );
	SetName("PatternCirc" + (string)buffer);

	cout << endl;
}
	

void FSOLIDOperatePatternCircular::ToTransCAD()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FSOLIDOperatePatternCircular to TransCAD" << endl;
	
	TransCAD::IReferencesPtr spReferences = GetPart()->_spPart->CreateReferences();

	for(int i=0; i<_spTargetFeatures.size();i++){
		TransCAD::IReferencePtr spReference = GetPart()->_spPart->SelectFeatureByName(
			_spTargetFeatures[i]->GetName().c_str());

		if (spReference) {
			spReferences->Add(spReference);
			cout << "	Selected Reference Feature in TransCAD: " << spReference->GetReferenceeName() << endl;
		}
	}

	//-------------------  Create Feature  -------------------//
	GetPart()->_spFeatures->AddNewSolidOperatePatternCircularFeature
		(GetName().c_str(), spReferences, OrgX, OrgY, OrgZ, DirX, DirY, DirZ,
		AngularNumber, AngularSpacing, RadialNumber, RadialSpacing, true);
	cout << "FSOLIDOperatePatternCircular: " << GetName().c_str() << " Translated" << endl << endl;

}