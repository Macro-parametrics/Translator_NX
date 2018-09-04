#include "stdafx.h"
#include "FSOLIDOperatePatternRectangular.h"

#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/Features_SelectFeatureList.hxx>
#include <NXOpen/Features_PatternFeatureBuilder.hxx>
#include <NXOpen/Direction.hxx>

#include "Part.h"

#include <iostream>
using namespace std;

int FSOLIDOperatePatternRectangular::_patRecCnt = 1;

FSOLIDOperatePatternRectangular::FSOLIDOperatePatternRectangular(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
	: Feature(part,ugfeature, featureList)
{
	_fType = SOLID_Operate_Pattern_Rectangular;
}

FSOLIDOperatePatternRectangular::~FSOLIDOperatePatternRectangular(void)
{
}


void FSOLIDOperatePatternRectangular::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSOLIDOperatePatternRectangular Info" << endl;
	
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

	NXOpen::Direction* xdir = patternBuilder->PatternService()->RectangularDefinition()->XDirection();
	NXOpen::Direction* ydir = patternBuilder->PatternService()->RectangularDefinition()->YDirection();

	RowDirX = ValRnd(xdir->Vector().X);
	RowDirY = ValRnd(xdir->Vector().Y);
	RowDirZ = ValRnd(xdir->Vector().Z);

	cout <<	"Pattern Row Direction: (" << RowDirX << " , " << RowDirY << " , " << RowDirZ << ")" << endl; 		
	
	ColDirX = ValRnd(ydir->Vector().X);
	ColDirY = ValRnd(ydir->Vector().Y);
	ColDirZ = ValRnd(ydir->Vector().Z);

	cout <<	"Pattern Column Direction: (" << ColDirX << " , " << ColDirY << " , " << ColDirZ << ")" << endl; 		

	RowSpacing = ValRnd(patternBuilder->PatternService()->RectangularDefinition()
		->XSpacing()->PitchDistance()->Value());
	ColumnSpacing = ValRnd(patternBuilder->PatternService()->RectangularDefinition()
		->YSpacing()->PitchDistance()->Value());
	cout << "(Row Spacing, Col Spacing) = (" << RowSpacing << ", " << ColumnSpacing << ")"<< endl;

	RowNumber = patternBuilder->PatternService()->RectangularDefinition()->XSpacing()->NCopies()->Value();
	ColumnNumber = patternBuilder->PatternService()->RectangularDefinition()->YSpacing()->NCopies()->Value();

	cout << "(Row Copies, Col Copies) = (" << RowNumber << ", " << ColumnNumber << ")"<< endl;

	//-------------------  Set Result_Object_Name  -------------------//
	char buffer[20];
	_itoa( _patRecCnt++, buffer, 10 );
	SetName("PatternRec" + (string)buffer);

	cout << endl;
}
	

void FSOLIDOperatePatternRectangular::ToTransCAD()
{
	//TransCAD::IReferencesPtr spReferences = GetPart()->_spPart->CreateReferences();

	//vector<ReferenceEntity *>::iterator iter = _referenceEntityList.begin();

	//while(iter != _referenceEntityList.end())
	//{
	//	(*iter)->ToTransCAD();
	//	if((*iter)->GetReferencePtr() != NULL)
	//		spReferences->Add((*iter)->GetReferencePtr());
	//	iter ++;
	//}

	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FSOLIDOperatePatternRectangular to TransCAD" << endl;
	
	TransCAD::IReferencesPtr spReferences = GetPart()->_spPart->CreateReferences();

	// 패턴 방향 설정
	for(int i=0; i<_spTargetFeatures.size();i++){
		TransCAD::IReferencePtr spReference = GetPart()->_spPart->SelectFeatureByName(
			_spTargetFeatures[i]->GetName().c_str());
		if (spReference) {
			spReferences->Add(spReference);
			cout << "	Selected Reference Feature in TransCAD: " << spReference->GetReferenceeName() << endl;
		}
	}

	//-------------------  Create Feature  -------------------//
	GetPart()->_spFeatures->AddNewSolidOperatePatternRectangularFeature(GetName().c_str(),
		spReferences,ColumnSpacing, ColDirX, ColDirY, ColDirZ, ColumnNumber,
		RowSpacing, RowDirX, RowDirY, RowDirZ, RowNumber);
	cout << "FSOLIDOperatePatternRectangular: " << GetName().c_str() << " Translated" << endl << endl;

}