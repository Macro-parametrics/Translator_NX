#include "stdafx.h"
#include "FSOLIDOperateFilletingFilletConstant.h"
#include "Feature.h"
#include "Part.h"

#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/Features_EdgeBlendBuilder.hxx>
#include <NXOpen/ScCollector.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/EdgeMultipleSeedTangentRule.hxx>
#include <NXOpen/NXException.hxx>
#include <NXOpen/NXObjectManager.hxx>		// to handle tag btn UF func. and NXOpen func.

#include <iostream>
using namespace std;

int FSOLIDOperateFilletingFilletConstant::_fillCnt = 1;

FSOLIDOperateFilletingFilletConstant::FSOLIDOperateFilletingFilletConstant(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
	: Feature(part,ugfeature, featureList)
{
	_fType = SOLID_Operate_Filleting_Fillet_Constant;
}

FSOLIDOperateFilletingFilletConstant::~FSOLIDOperateFilletingFilletConstant(void)
{
}
void FSOLIDOperateFilletingFilletConstant::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSOLIDOperateFilletingFilletConstant Info" << endl;
	
	_PropType = Minimal;

	//NXOpen::Features::EdgeBlend * edgeBlendFeature(dynamic_cast<NXOpen::Features::EdgeBlend *>(_ugFeature));
	NXOpen::Features::EdgeBlendBuilder* edgeBlendBuilder = _featureList->CreateEdgeBlendBuilder(_ugFeature);
	
	NXOpen::ScCollector *collector;
	NXOpen::Expression * radius;
	NXOpen::Point3d *point1 = new NXOpen::Point3d;
	NXOpen::Point3d *point2 = new NXOpen::Point3d;

	std::vector<NXOpen::SelectionIntentRule*> _rules;
	std::vector<NXOpen::Edge*> _seedEdges;
	
	double* angleTolerance1 = new double();
	bool* hasSameConvexity = new bool();
	_numChainSet = edgeBlendBuilder->GetNumberOfValidChainsets();
	cout << "Fillet ChainSet Number(Edges and Curves in a ChainSet have same radius for filleting): " << _numChainSet << endl;
	for(int i=0; i<_numChainSet; i++){
		////NX7.5, vs2008에서 됬지만 이제 안되는 방식, collector->GetRules에서 internal error 발생 161101
		//edgeBlendBuilder->GetChainset(i, &collector, &radius);
		//cout << radius->Value() <<endl;
		//try
		//{
		//	collector->GetRules(_rules);
		//}
		//catch(NXOpen::NXException& e)
		//{
		//	cout << e.Message() << endl;
		//}
		//NXOpen::EdgeMultipleSeedTangentRule *edgeMultipleSeedTangentRule1 = (NXOpen::EdgeMultipleSeedTangentRule*)_rules[i];
		//edgeMultipleSeedTangentRule1->GetData(_seedEdges,angleTolerance1,hasSameConvexity);
		//_edgeNumPerChainSet.push_back(_seedEdges.size());
		//_ugFeature->Suppress();
		//for(int i=0;i<_seedEdges.size();i++){
		//	NXOpen::Edge* _edge = _seedEdges[i];
		//	_edge->GetVertices(point1, point2);
		//	Point3D sP(ValRnd(point1->X), ValRnd(point1->Y), ValRnd(point1->Z));
		//	Point3D eP(ValRnd(point2->X), ValRnd(point2->Y), ValRnd(point2->Z));
		//	Point3D pP = (sP+eP)*0.5;
		//	_sP.push_back(sP);
		//	_eP.push_back(eP);
		//	_pP.push_back(pP);
		//	_filRadius.push_back(radius->Value());
		//}
		//_ugFeature->Unsuppress();
		
		
		////결국 collector->GetRules안되서 collector->GetObjects를 이용한 tag를 이용한 edge 정보 접근방법 사용 161101
		edgeBlendBuilder->GetChainset(i, &collector, &radius);
		cout << "	///////////" << endl;
		cout << "	ChainSet" << i+1 << " with radius(value:" << radius->Value() << "):" << endl;
		std::vector<NXOpen::TaggedObject*> taggedobj = collector->GetObjects();
		_edgeNumPerChainSet.push_back(taggedobj.size());
		_ugFeature->Suppress();
		cout << "	Edge Number in this ChainSet: " << taggedobj.size() << endl;
		for(int j=0;j<taggedobj.size();j++){
			cout << "		Edge" << j+1 << ": " << endl;
			NXOpen::Edge* edgeFromTag = (NXOpen::Edge *)NXOpen::NXObjectManager::Get(taggedobj[j]->Tag());
			edgeFromTag->GetVertices(point1, point2);
			Point3D sP(ValRnd(point1->X), ValRnd(point1->Y), ValRnd(point1->Z));
			Point3D eP(ValRnd(point2->X), ValRnd(point2->Y), ValRnd(point2->Z));
			Point3D pP = (sP+eP)*0.5;
			_sP.push_back(sP);
			_eP.push_back(eP);
			_pP.push_back(pP);
			cout <<	"		Start Point: (" << sP[0] << " , " << sP[1] << " , " << sP[2] << ")" << endl; 
			cout <<	"		End Point: (" << eP[0] << " , " << eP[1] << " , " << eP[2] << ")" << endl; 
			cout <<	"		Mid Point: (" << pP[0] << " , " << pP[1] << " , " << pP[2] << ")" << endl; 
			_filRadius.push_back(radius->Value());
		}
		_ugFeature->Unsuppress();
		cout << "	ChainSet" << i+1 << " GetInfo finished" << endl;
	}
	delete angleTolerance1;
	delete hasSameConvexity;
	delete point1;
	delete point2;
	angleTolerance1 = NULL;
	hasSameConvexity = NULL;
	point1 = NULL;
	point2 = NULL;	

	cout << endl;
}

void FSOLIDOperateFilletingFilletConstant::ToTransCAD()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FSOLIDOperateFilletingFilletConstant to TransCAD" << endl;
	
	int edgeCnt = 0;
	for(int i=0; i<_numChainSet ;i++){
		cout << "///////////" << endl;
		cout << "ChainSet" << i+1 << ":" << endl;
		TransCAD::IReferencesPtr spReferences = GetPart()->_spPart->CreateReferences();
		double  rd;
		for(int j=0; j<_edgeNumPerChainSet[i]; j++){
			Point3D sP = _sP[edgeCnt];
			Point3D eP = _eP[edgeCnt];
			Point3D pP = _pP[edgeCnt];
			rd = _filRadius[edgeCnt];
			edgeCnt++;
			TransCAD::IReferencePtr spReference = GetPart()->_spPart->SelectCurveBy3Points(sP[0],sP[1],sP[2],eP[0],eP[1],eP[2],pP[0],pP[1],pP[2]);
			if (spReference) {
				spReferences->Add(spReference);
				cout << "	Edge" << j+1 << "(TransCAD): " << spReference->GetReferenceeName() << endl;
			}else {
				cout << "	Fillet Edge is not detected!"<<endl;
			}
		}
		//-------------------  Set Result_Object_Name  -------------------//
		char buffer[20];
		_itoa( _fillCnt++, buffer, 10 );
		SetName("Fillet" + (string)buffer);
		//-------------------  Creat Fillet --------------------------//
		GetPart()->_spFeatures->AddNewSolidFilletConstantFeature(GetName().c_str(), spReferences, rd, (TransCAD::PropagationType)GetPropType());
		cout << "ChainSet" << i+1 << "(FSOLIDOperateFilletingFilletConstant): " << GetName().c_str() << " Translated" << endl;
	}
}