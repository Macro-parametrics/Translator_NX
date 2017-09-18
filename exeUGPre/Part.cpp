#pragma once
#include "stdafx.h"

#include "Part.h"
#include "Feature.h"
#include "FDatumPlane.h"
#include "FSOLIDCreateProtrusionExtrude.h"
#include "FSOLIDCreateCutExtrude.h"
#include "FSOLIDOperateFilletingFilletConstant.h"
#include "FSOLIDOperateFilletingChamferConstant.h"
#include "FSOLIDCreateProtrusionRevolve.h"
#include "FSOLIDCreateCutRevolve.h"
#include "FSOLIDCreateProtrusionSweep.h"
#include "FSOLIDOperatePatternRectangular.h"
#include "FSOLIDOperatePatternCircular.h"
#include "FSOLIDCreateCounterbored.h"
#include "FSOLIDCreateCountersunk.h"
#include "FSketch.h"
#include "FSketchHolePoint.h"

#include <NXOpen/Features_ExtrudeBuilder.hxx>
#include <NXOpen/Features_RevolveBuilder.hxx>
#include <NXOpen/Features_EdgeBlendBuilder.hxx>
#include <NXOpen/Features_BodyFeature.hxx>
#include <NXOpen/Features_EdgeBlend.hxx>
#include <NXOpen/Features_PatternFeatureBuilder.hxx>
#include <NXOpen/GeometricUtilities_BooleanOperation.hxx>
#include <NXOpen/ScCollector.hxx>
#include <NXOpen/BodyCollection.hxx>
#include <NXOpen/Edge.hxx>
#include <NXOpen/Body.hxx>
#include <NXOpen/EdgeMultipleSeedTangentRule.hxx>
#include <NXOpen/NXObjectManager.hxx>
#include <NXOpen/CurveCollection.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/NXString.hxx>
#include <NXOpen/NXException.hxx>

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

Part::Part(TransCAD::IApplicationPtr transcad , string fileName, NXOpen::Session* NXSession)
{
	// Initiate TransCAD related members (생성된 TransCAD application instance에 파트 추가)
	TransCAD::IDocsPtr spDocuments = transcad->Documents;
	_spDocument = spDocuments->AddPartDocument();
	_spPart     = _spDocument->Part;
	_spFeatures = _spPart->Features;

	_fileName  = fileName;

	// Open UG Part File
	_ugSession = NXSession;
	_bFileOpened = false;
	PartFileOpen();
}

Part::~Part(void)
{
	Clear();
}

void Part::Clear()
{
	std::vector<Feature *>::iterator i = _featureList.begin();
	while (i != _featureList.end()) {delete *i; i++;}
	_featureList.clear();
}

//완료
void Part::PartFileOpen()
{
	// open UG file to get Part Tag int _partTag
	NXOpen::PartLoadStatus *partLoadStatus1;
	_ugPart = _ugSession->Parts()->OpenDisplay(_fileName.c_str(), &partLoadStatus1);

	int returnval = partLoadStatus1->NumberUnloadedParts();
	delete partLoadStatus1;
	
	if (returnval != 0) return;

	_bFileOpened = true;
}

Feature * Part::Get_pFeatureByJID(NXOpen::NXString jID) const
{
	string temp = jID.GetUTF8Text();
	for (unsigned int i=0; i < GetFeaturesSize() ; ++i)
		if (!strcmp(jID.GetUTF8Text(),GetFeature(i)->GetJID().GetUTF8Text()))
			return GetFeature(i);
	return 0;
}

Feature * Part::CreateFeature(NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
{
	bool bNoComment = 0;
	cout << "Create Feature: " << endl;
	cout << "Feature Name = [ " << ugfeature->GetFeatureName().GetUTF8Text() << " ]" << endl;
	cout << "Feature JID = [ " << ugfeature->JournalIdentifier().GetUTF8Text() << " ]" << endl;
	cout << "Feature type = [ " << ugfeature->FeatureType().GetUTF8Text() << " ]" << endl;

	Feature* pFeature = 0;
	
	//==========  DATUM_PLANE : <SELECT_Reference_Plane>  ==========//
	if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "DATUM_PLANE"))
	{
		pFeature = new FDatumPlane(this, ugfeature, featureList);
		cout << "FDatumPlane class created" << endl;
		bNoComment = 1;
	}

	//==========  DATUM_AXIS : No feature  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "DATUM_AXIS"))
	{
		cout << "Do Not Need to Translate" << endl;
		bNoComment = 1;
	}

	//==========  "SKETCH" : SketchFeature  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "SKETCH")&&
		strstr(ugfeature->JournalIdentifier().GetUTF8Text(),"SKETCH"))
	{
		pFeature = new FSketch(this, ugfeature, featureList);
		cout << "FSketch class created" << endl;
		bNoComment = 1;
	}

	//==========  "EXTRUDE" : Extrude, ExtrudeCut  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "EXTRUDE")&&
		strstr(ugfeature->JournalIdentifier().GetUTF8Text(),"EXTRUDE"))//because of features that has JID of pattern feature but feature type of EXTRUDE
	{
		NXOpen::Features::ExtrudeBuilder* extrudeBuilder = _ugPart->Features()->CreateExtrudeBuilder(ugfeature);
		if(extrudeBuilder->BooleanOperation()->Type()==0||extrudeBuilder->BooleanOperation()->Type()==1){
			pFeature = new FSOLIDCreateProtrusionExtrude(this, ugfeature, featureList);
			cout << "FSOLIDCreateProtrusionExtrude class created" << endl;
		}else if(extrudeBuilder->BooleanOperation()->Type()==NXOpen::GeometricUtilities::BooleanOperation::BooleanType::BooleanTypeSubtract){
			pFeature = new FSOLIDCreateCutExtrude(this, ugfeature, featureList);
			cout << "FSOLIDCreateCutExtrude class created" << endl;
		}else{
			cout << "Not protruded nor substracted Extrude Type not supported yet!" << endl;
		}
		bNoComment = 1;
	}

	//==========  "BLEND" : Fillet Feature  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "BLEND"))
	{
		pFeature = new FSOLIDOperateFilletingFilletConstant(this, ugfeature, featureList);
		cout << "FSOLIDOperateFilletingFilletConstant class created" << endl;
		bNoComment = 1;
		////NXOpen::Features::EdgeBlend * edgeBlendFeature(dynamic_cast<NXOpen::Features::EdgeBlend *>(ugfeature));
		////NXOpen::Features::EdgeBlendBuilder* edgeBlendBuilder = _ugPart->Features()->CreateEdgeBlendBuilder(ugfeature);
		////NXOpen::ScCollector *collector;
		////NXOpen::Expression * radius;
		//vector<NXOpen::SelectionIntentRule*> rules1;
		//vector<NXOpen::Edge*> seedEdges1;
		//double* angleTolerance1 = new double();
		//bool* hasSameConvexity = new bool();
		//int numChainSet = edgeBlendBuilder->GetNumberOfValidChainsets();
		//edgeBlendBuilder->GetChainset(0, &collector, &radius);
		////vector<NXOpen::TaggedObject*> myObjects = collector->GetObjects();
		////cout << "size: "<< myObjects.size()<<endl;
		//NXOpen::Point3d *point1 = new NXOpen::Point3d;
		//NXOpen::Point3d *point2 = new NXOpen::Point3d;
		///*for(int i=0; i<myObjects.size();i++){
		//	cout << myObjects[i]->Tag() <<endl;
		//	NXOpen::Edge* edgeFromTag = (NXOpen::Edge*)NXOpen::NXObjectManager::Get(myObjects[i]->Tag());
		//	cout << edgeFromTag->JournalIdentifier().GetUTF8Text()<<endl;
		//	NXOpen::Edge * aa = dynamic_cast<NXOpen::Edge *>(_ugPart->Features()->FindObject("EDGE 170 * 180 {EXTRUDE(2)}"));
		//	try{double l = aa->GetLength();}
		//	catch(const NXOpen::NXException e){cout <<

		//	
		//	
		//}*/
		///*vector<NXOpen::Edge*> myEdges = edgeBlendFeature->GetEdges();
		//for(int j=0; j<myEdges.size();j++){
		//	cout << myEdges[j]->Tag()<<endl;
		//}*/
		//cout << radius->Value() <<endl;
		//collector->GetRules(rules1);
		//cout <<rules1.size()<<endl;
		////rules1[0]->get
		////cout << rules1[0]->Type() <<endl;
		//
		//NXOpen::EdgeMultipleSeedTangentRule *edgeMultipleSeedTangentRule1 = (NXOpen::EdgeMultipleSeedTangentRule*)rules1[0];

		//
		//edgeMultipleSeedTangentRule1->GetData(seedEdges1,angleTolerance1,hasSameConvexity);
		//cout << seedEdges1.size()<<endl;
		//ugfeature->Suppress();
		//for(int i=0;i<seedEdges1.size();i++){
		//	NXOpen::Edge* edge1 = seedEdges1[i];
		//	bool bb = edge1->IsOccurrence();
		//	cout << edge1->GetTag() <<endl;
		//	NXOpen::Edge* edgeFromTag = (NXOpen::Edge*)NXOpen::NXObjectManager::Get(edge1->GetTag());
		//	double l;// = edge1->GetLength();
		//	edge1->GetVertices(point1, point2);
		//	try{
		//		double l = edge1->GetLength();}
		//	catch(NXOpen::NXException e){
		//		cout << e.Message()<<endl;
		//	}

		//	/*
		//	cout <<edge1->JournalIdentifier().GetUTF8Text()<<endl;
		//	edge1->GetVertices(point1, point2);*/
		//	cout << point1->X <<","<<point1->Y<<","<<point1->Z<<endl;
		//	cout << point2->X <<","<<point2->Y<<","<<point2->Z<<endl;
		//}
		//delete angleTolerance1;
		//delete hasSameConvexity;
		//delete point1;
		//delete point2;
		
	}

		//==========  "CHAMFER" : Chamfer Feature  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "CHAMFER"))
	{
		pFeature = new FSOLIDOperateFilletingChamferConstant(this, ugfeature, featureList);
		bNoComment = 1;
		cout << "FSOLIDOperateFilletingChamferConstant class created" << endl;
	}
	//==========  Revolve  ==========//
	else if(strstr(ugfeature->JournalIdentifier().GetUTF8Text(),"REVOLVED")) 
	{
		NXOpen::Features::RevolveBuilder* revolveBuilder = _ugPart->Features()->CreateRevolveBuilder(ugfeature);
		//cout << revolveBuilder->BooleanOperation()->Type() << endl;
		if(revolveBuilder->BooleanOperation()->Type()==0||revolveBuilder->BooleanOperation()->Type()==1){
			pFeature = new FSOLIDCreateProtrusionRevolve(this, ugfeature, featureList);
			cout << "FSOLIDCreateProtrusionRevolve class created" << endl;
			bNoComment = 1;
		}else if(revolveBuilder->BooleanOperation()->Type()==NXOpen::GeometricUtilities::BooleanOperation::BooleanType::BooleanTypeSubtract){
			pFeature = new FSOLIDCreateCutRevolve(this, ugfeature, featureList);
			cout << "FSOLIDCreateCutRevolve class created" << endl;
			bNoComment = 1;
		}else{
			cout << "Not protruded nor substracted Revolve Type not supported" << endl;
		}
	}

	//==========  Swept OR SweepAlongGuide==========//
	else if(strstr(ugfeature->JournalIdentifier().GetUTF8Text(),"SWEPT")||
		strstr(ugfeature->JournalIdentifier().GetUTF8Text(),"SWEEP")) 
	{
		pFeature = new FSOLIDCreateProtrusionSweep(this, ugfeature, featureList);
		cout << "FSOLIDCreateProtrusionSweep class created" << endl;
		bNoComment = 1;
	}


	//==========  Pattern Feature  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "Pattern Feature"))
	{
		NXOpen::Features::PatternFeatureBuilder* patternBuilder = 
			_ugPart->Features()->CreatePatternFeatureBuilder(ugfeature);
		//string patternJID = ugfeature->JournalIdentifier().GetUTF8Text();
		//unsigned first = patternJID.find("(");
		//unsigned last = patternJID.find(")");
		//string featNum = patternJID.substr (first,last-first);
		
		//Rectangular
		if(patternBuilder->PatternService()->PatternType() == 
			patternBuilder->PatternService()->PatternEnumLinear){
				pFeature = new FSOLIDOperatePatternRectangular(this, ugfeature, featureList);
				cout << "FSOLIDOperatePatternRectangular class created" << endl;
				bNoComment = 1;
		}
		//Circular
		else if(patternBuilder->PatternService()->PatternType() == 
			patternBuilder->PatternService()->PatternEnumCircular){
				pFeature = new FSOLIDOperatePatternCircular(this, ugfeature, featureList);
				cout << "FSOLIDOperatePatternCircular class created" << endl;
				bNoComment = 1;
		}else{
			cout << "Only (Rectangular, Circular) pattern supported for now" << endl;
		}
	}

	//==========  SketchHolePoint  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "SKETCH")&&
		strstr(ugfeature->JournalIdentifier().GetUTF8Text(),"COUNTERBORED HOLE"))
	{
		pFeature = new FSketchHolePoint(this, ugfeature, featureList);
		cout << "FSketchHolePoint class created" << endl;
		bNoComment = 1;
	}
		
	//==========  SketchHolePoint  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "SKETCH")&&
		strstr(ugfeature->JournalIdentifier().GetUTF8Text(),"COUNTERSUNK HOLE"))
	{
		pFeature = new FSketchHolePoint(this, ugfeature, featureList);
		cout << "FSketchHolePoint class created" << endl;
		bNoComment = 1;
	}

	////==========  "Counterbored Hole"  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "HOLE PACKAGE")&&
		strstr(ugfeature->JournalIdentifier().GetUTF8Text(),"COUNTERBORED HOLE"))
	{		
		pFeature = new FSOLIDCreateCounterbored(this, ugfeature, featureList);
		cout << "FSOLIDCreateCounterbored class created" << endl;
		bNoComment = 1;
	}
	
	////==========  "Countersunk Hole"  ==========//
	else if(!strcmp(ugfeature->FeatureType().GetUTF8Text(), "HOLE PACKAGE")&&
		strstr(ugfeature->JournalIdentifier().GetUTF8Text(),"COUNTERSUNK HOLE"))
	{
		pFeature = new FSOLIDCreateCountersunk(this, ugfeature, featureList);
		cout << "FSOLIDCreateCountersunk class created" << endl;
		bNoComment = 1;
	}

	////==========  "SWP104" : Extrude, Sweep, Revolve  ==========//
	//else if(!strcmp(featureType, "SWP104"))			
	//{
	//	UF_FEATURE_SIGN sign;						// UF_NULLSIGN = 0, create new target solid
	//	UF_MODL_ask_feature_sign(fTag, &sign);		// UF_POSITIVE = 1, add to target solid
	//												// UF_NEGATIVE = 2, subtract from target solid
	//		
	//	//==========  Extrude  ==========//
	//	if(!_strnicmp(featureName, "E", 1))
	//	{
	//		if (sign < 2)	{		// Protrusion
	//			pFeature = new FSOLIDCreateProtrusionExtrude(this,fTag);
	//			//cout << "SOLIDCreateProtrusionExtrude Constructed" << endl;
	//		}
	//			
	//		else if (sign == 2)	{	// Cut
	//			pFeature = new FSOLIDCreateCutExtrude(this,fTag);
	//			//cout << "SOLIDCreateCutExtrude Constructed" << endl;
	//		}
	//	}

	//	//==========  SWEEP  ==========//
	//	else if(!_strnicmp(featureName, "S", 1))
	//	{
	//		if (sign < 2) {			// Protrusion
	//			pFeature = new FSOLIDCreateProtrusionSweep(this,fTag);
	//			//cout << "SOLIDCreateProtrusionSweep Constructed" << endl;
	//			bNoComment = 1;
	//		}
	//		else if (sign == 2)	{	// Cut
	//			//pFeature = new FSOLIDCreateCutSweep(this,fTag);
	//			//cout << "SOLIDCreateCutSweep under construction" << endl;
	//			bNoComment = 1;
	//		}
	//	}
	//		

	if (pFeature)
		_featureList.push_back(pFeature);	// put into _featureList
	else if (!bNoComment)
		cerr << "*** Not Supported Yet!!! ***" << endl;

	cout << endl;

	//
	return pFeature;
}


void Part::GetUGInfo()
{
	Feature * pFeature;
	
	//-------  Create features list --------//
	NXOpen::Features::FeatureCollection *featureList = _ugPart->Features();
	NXOpen::Features::FeatureCollection::iterator itr;

	for (itr = featureList->begin(); itr != featureList->end(); ++itr){
		_ugFeatlist.push_back((NXOpen::Features::Feature*) *itr);
	}

	cout << "Create Feature Instances!" << endl << endl;
	for (int i=0;i<_ugFeatlist.size();i++)
	{
		if(_ugFeatlist[i]){
			pFeature = CreateFeature(_ugFeatlist[i], featureList);	// _featureList에 feature들을 모두 입력
		}
	}

	cout << endl << "///////////////////////Get NX information!///////////////////////"<<endl<<endl;
	//------- Loop to read feature information in each feature --------//
	for ( size_t i=0 ; i < GetFeaturesSize() ; ++i )
	{
		GetFeature((unsigned int)i)->GetUGInfo();
	}
	
}


void Part::ToTransCAD()
{
	cout << endl << "////////////////////////To TransCAD Start!////////////////////////"<<endl<<endl;
	//-------  Loop to write feature information into TransCAD  --------//
	for ( size_t i=0 ; i < GetFeaturesSize() ; ++i )
	{
		GetFeature((unsigned int)i)->ToTransCAD();
	}
}
