#include "stdafx.h"
#include "FSOLIDOperateFilletingChamferConstant.h"
#include "Feature.h"
#include "Part.h"

#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/Features_ChamferBuilder.hxx>
#include <NXOpen/ScCollector.hxx>
#include <NXOpen/EdgeTangentRule.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/Expression.hxx>
#include <NXOpen/NXException.hxx>
#include <NXOpen/NXObjectManager.hxx>		// to handle tag btn UF func. and NXOpen func.
#include <NXOpen/Features_BodyFeature.hxx>
#include <NXOpen/Part.hxx>

#include <NXOpen\GeometricAnalysis_GeometricProperties.hxx>
#include <NXOpen\GeometricAnalysis_AnalysisManager.hxx>


#include <NXOpen\GeometricAnalysis_GeometricProperties.hxx>
#include <NXOpen\GeometricAnalysis_AnalysisManager.hxx>

#include <iostream>
using namespace std;

int FSOLIDOperateFilletingChamferConstant::_chamfCnt = 1;

FSOLIDOperateFilletingChamferConstant::FSOLIDOperateFilletingChamferConstant(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
	: Feature(part,ugfeature, featureList)
{
	_fType = SOLID_Operate_Filleting_Chamfer_Constant;
}

FSOLIDOperateFilletingChamferConstant::~FSOLIDOperateFilletingChamferConstant(void)
{
}

void FSOLIDOperateFilletingChamferConstant::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSOLIDOperateFilletingChamferConstant Info" << endl;
	
	NXOpen::Point3d *point1 = new NXOpen::Point3d;
	NXOpen::Point3d *point2 = new NXOpen::Point3d;

	NXOpen::Features::ChamferBuilder *chamferBuilder = _featureList->CreateChamferBuilder(_ugFeature);
	if(chamferBuilder->Option()!=chamferBuilder->ChamferOptionSymmetricOffsets){
		cout << "Cannot translate this option of chamfer (This is not symmetric offset chamfer)" <<endl;
	}else{
		NXOpen::ScCollector *collector = chamferBuilder->SmartCollector();
		_chamfLength = chamferBuilder->FirstOffsetExp()->Value();
		std::vector<NXOpen::TaggedObject*> taggedobj = collector->GetObjects();
		_edgeNum = taggedobj.size();
		cout << "Chamefer Edge Number: " << _edgeNum << endl;
		_ugFeature->Suppress();
		for(int i=0;i<taggedobj.size();i++){
			cout << "	///////////" << endl;
			cout << "	Edge" << i+1 << ": " << endl;
			NXOpen::Edge* edgeFromTag = (NXOpen::Edge *)NXOpen::NXObjectManager::Get(taggedobj[i]->Tag());
			//string temp = edgeFromTag->JournalIdentifier().GetUTF8Text();
			edgeFromTag->GetVertices(point1, point2);
			Point3D sP(ValRnd(point1->X), ValRnd(point1->Y), ValRnd(point1->Z));
			Point3D eP(ValRnd(point2->X), ValRnd(point2->Y), ValRnd(point2->Z));
			Point3D pP = (sP+eP)*0.5;
			_sP.push_back(sP);
			_eP.push_back(eP);
			_pP.push_back(pP);
			cout <<	"	Start Point: (" << sP[0] << " , " << sP[1] << " , " << sP[2] << ")" << endl; 
			cout <<	"	End Point: (" << eP[0] << " , " << eP[1] << " , " << eP[2] << ")" << endl; 
			cout <<	"	Mid Point: (" << pP[0] << " , " << pP[1] << " , " << pP[2] << ")" << endl; 
		}
		_ugFeature->Unsuppress();
	}

	cout << endl;
}

//void FSOLIDOperateFilletingChamferConstant::GetUGInfo()
//{
//	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
//	cout << "Get FSOLIDOperateFilletingChamferConstant Info" << endl;
//	
//	NXOpen::Point3d *point1 = new NXOpen::Point3d;
//	NXOpen::Point3d *point2 = new NXOpen::Point3d;
//
//	NXOpen::Features::ChamferBuilder *chamferBuilder = _featureList->CreateChamferBuilder(_ugFeature);
//	if(chamferBuilder->Option()!=chamferBuilder->ChamferOptionSymmetricOffsets){
//		cout << "Cannot translate this option of chamfer (This is not symmetric offset chamfer)" <<endl;
//	}else{
//		NXOpen::ScCollector *collector = chamferBuilder->SmartCollector();
//		_chamfLength = chamferBuilder->FirstOffsetExp()->Value();
//		std::vector<NXOpen::TaggedObject*> taggedobj = collector->GetObjects();
//		_edgeNum = taggedobj.size();
//		cout << "Chamefer Edge Number: " << _edgeNum << endl;
//		_ugFeature->Suppress();
//		for(int i=0;i<taggedobj.size();i++){
//			cout << "	///////////" << endl;
//			cout << "	Edge" << i+1 << ": " << endl;
//			NXOpen::Edge* edgeFromTag = (NXOpen::Edge *)NXOpen::NXObjectManager::Get(taggedobj[i]->Tag());
//			//string temp = edgeFromTag->JournalIdentifier().GetUTF8Text();
//			edgeFromTag->GetVertices(point1, point2);
//
//			Point3D sP(ValRnd(point1->X), ValRnd(point1->Y), ValRnd(point1->Z));
//			Point3D eP(ValRnd(point2->X), ValRnd(point2->Y), ValRnd(point2->Z));
//			Point3D pP = (sP+eP)*0.5;
//
//			
//
//			
//			if(sP.operator==(eP)){
//				NXOpen::Point3d point(point1->Z, point1->Y, point1->X); 
//				vector<NXOpen::Features::Feature* > parentFeat = _ugFeature->GetParents();
//				
//				for(int i = 0; i<parentFeat.size(); i++){
//					NXOpen::NXString refFeatJID;
//					refFeatJID = parentFeat.at(i)->JournalIdentifier();
//					
//					TransCAD::IReferencePtr edgebody = GetPart()->_spPart->GetFeatures()->->SelectFeatureByName(GetPart()->Get_pFeatureByJID(refFeatJID)->GetName().c_str());
//					edgebody->
//
//					NXOpen::Features::BodyFeature* bodyFeat(dynamic_cast<NXOpen::Features::BodyFeature *>(parentFeat.at(i)));
//					vector<NXOpen::Edge* > edges = bodyFeat->GetEdges();
//				
//					for(int j = 0; j<edges.size(); j++){
//						NXOpen::NXObject * sourceEdge = (NXOpen::NXObject *)edges.at(j);
//						NXOpen::GeometricAnalysis::GeometricProperties::Edge edgeProp;
//
//						NXOpen::GeometricAnalysis::GeometricProperties * geometryObject = GetPart()->_ugPart->AnalysisManager()->CreateGeometricPropertiesObject();
//				
//						NXOpen::GeometricAnalysis::GeometricProperties::Status a = geometryObject->GetEdgeProperties(sourceEdge, point, &edgeProp);
//
//						if(a == NXOpen::GeometricAnalysis::GeometricProperties::Status::StatusSuccess){
//							cout << "Success" << endl;
//						}else if(a == NXOpen::GeometricAnalysis::GeometricProperties::Status::StatusFailed){
//							cout << "Failed" << endl;
//						}else{
//							cout << "Invalid" << endl;
//						}
//
//						NXOpen::Point3d p = edgeProp.PositionInWcs;
//						Point3D ugPoint(ValRnd(p.X), ValRnd(p.Y), ValRnd(p.Z));
//
//						_sP.push_back(ugPoint);
//						_eP.push_back(ugPoint);
//						_pP.push_back(ugPoint);
//						cout << "	Circle Edge Selection Point: (" <<  ugPoint[0] << " , " << ugPoint[1] << " , " << ugPoint[2] << ")" << endl; 
//					}
//				}
//			}else{
//				_sP.push_back(sP);
//				_eP.push_back(eP);
//				_pP.push_back(pP);
//				cout <<	"	Start Point: (" << sP[0] << " , " << sP[1] << " , " << sP[2] << ")" << endl; 
//				cout <<	"	End Point: (" << eP[0] << " , " << eP[1] << " , " << eP[2] << ")" << endl; 
//				cout <<	"	Mid Point: (" << pP[0] << " , " << pP[1] << " , " << pP[2] << ")" << endl; 
//			}
//		}
//		_ugFeature->Unsuppress();
//	}
//
//	cout << endl;
//}

void FSOLIDOperateFilletingChamferConstant::ToTransCAD()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FSOLIDOperateFilletingChamferConstant to TransCAD" << endl;
	
	TransCAD::IReferencesPtr spReferences = GetPart()->_spPart->CreateReferences();

	for(int i=0; i<_edgeNum;i++){
		cout << "	///////////" << endl;
		cout << "	Edge" << i+1 << ": " << endl;

		Point3D sP = _sP[i];
		Point3D eP = _eP[i];
		Point3D pP = _pP[i];
		TransCAD::IReferencePtr spReference = GetPart()->_spPart->SelectCurveBy3Points(sP.X(),sP.Y(),sP.Z(),eP.X(),eP.Y(),eP.Z(),pP.X(),pP.Y(),pP.Z());
		if (spReference) {
			spReferences->Add(spReference);
			cout << "	Edge" << i+1 << "(TransCAD): " << spReference->GetReferenceeName() << endl;
		}else{ //sometimes, circle edge's start and end points' coordinate obtained from NX is distorted. eg. (a, b, c) should be start point, but we get (c, b, a). So we swap x and z coordinate to pass this case... AdHoc Method
			cout << "	Selection of Edge failed with original Start/End points! Swap X and Z value of Start/End Points -> ";
			sP.SetX(_sP[i].Z());
			sP.SetZ(_sP[i].X());
			eP.SetX(_eP[i].Z());
			eP.SetZ(_eP[i].X());
			pP = (sP+eP)*0.5;
			spReference = GetPart()->_spPart->SelectCurveBy3Points(sP.X(),sP.Y(),sP.Z(),eP.X(),eP.Y(),eP.Z(),pP.X(),pP.Y(),pP.Z());
			if (spReference) {
				cout << "Edge" << i+1 << "(TransCAD): " << spReference->GetReferenceeName() << endl;
				spReferences->Add(spReference);
			}else{
				cout << "Chamfer Edge is not detected!"<<endl;
			}
		}
	}
		
	//-------------------  Set Result_Object_Name  -------------------//
	char buffer[20];
	_itoa( _chamfCnt++, buffer, 10 );
	SetName("Chamfer" + (string)buffer);
	//-------------------  Creat Chamfer --------------------------//
	GetPart()->_spFeatures->AddNewSolidChamferFeature(GetName().c_str(), spReferences, _chamfLength);

	cout << "FSOLIDOperateFilletingChamferConstant: " << GetName().c_str() << " Translated" << endl << endl;

}