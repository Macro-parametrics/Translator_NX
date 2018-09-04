#include "stdafx.h"
#include "FeatureSOLIDCreateCutExtrude.h"

/** UG Post's header files **/
#include "Part.h"
#include "FSketch.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>
#include <NXOpen\BodyCollection.hxx>

#include <NXOpen\Features_ExtrudeBuilder.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\Section.hxx>
#include <NXOpen\SectionCollection.hxx>
#include <NXOpen\GeometricUtilities_BooleanOperation.hxx>	// for EXTRUDE boolean operation
#include <NXOpen\Features_SketchFeature.hxx>
#include <NXOpen\CurveFeatureRule.hxx>
#include <NXOpen\ScRuleFactory.hxx>
#include <NXOpen\SketchCollection.hxx>				// for EXTRUDE profile sketch
#include <NXOpen\DirectionCollection.hxx>			// for EXTRUDE direction
#include <NXOpen\GeometricUtilities_Limits.hxx>		// for EXTRUDE depth
#include <NXOpen\GeometricUtilities_Extend.hxx>		// for EXTRUDE depth

/** For intersection test **/
#include <NXOpen\Features_BooleanBuilder.hxx>
#include <NXOpen\SelectBodyList.hxx>
#include <NXOpen\ScCollectorCollection.hxx>
#include <NXOpen\ScRuleFactory.hxx>


using namespace std;

FeatureSOLIDCreateCutExtrude::FeatureSOLIDCreateCutExtrude(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart, spFeature)
{

}

FeatureSOLIDCreateCutExtrude::~FeatureSOLIDCreateCutExtrude(void) {}

void FeatureSOLIDCreateCutExtrude::GetInfo()
{
	TransCAD::IStdSolidCutExtrudeFeaturePtr spFeature = GetTransCADFeature();
	
	/** Extrude의 깊이, 설정, 방향을 TransCAD에서 맴버 변수로 저장 **/
	_startDepth = (-1) * (spFeature->EndDepth);
	_endDepth = spFeature->StartDepth;

	_startCondition = spFeature->EndCondition;
	_endCondition = spFeature->StartCondition;

	_flip = spFeature->IsFlip ? true : false;

	cout << "\n  CutExtrude startDepth = " << _startDepth << endl;
	cout << "  CutExtrude endDepth   = " << _endDepth << endl;

	/** cut extrude의 profile sketch를 저장 **/
	TransCAD::IReferencePtr spReference = spFeature->ProfileSketch;
	string profileSketch = spReference->ReferenceeName;

	cout << "  CutExtrude Profile sketch = " << profileSketch << endl;

	_featureProfileSketch = (FSketch *)(GetPart()->GetFeatureByName(profileSketch));
}

void FeatureSOLIDCreateCutExtrude::ToUG()
{
	using namespace NXOpen;

	Features::Feature * cutExtrudeFeat;

	Features::ExtrudeBuilder * builder;
	builder = this->cutExtrudeBuilderSet(_featureProfileSketch);
	

	/** Subtract을 수행할 target body 선정 **/
	vector<Body *> targetBodies;
	

	if ( _featureProfileSketch->_refType == 1 )		// Datum plane
	{
		Session::UndoMarkId cutExtCreateUndo;
		cutExtCreateUndo = _Part->_nxSession->SetUndoMark(Session::MarkVisibilityVisible, "cutExtCreateUndo");

		builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeCreate);
		
		// Cut extrude feature 생성(Body 단위)
		cutExtrudeFeat = builder->CommitFeature();

		/**
		 * Target body를 탐색
		 * target body가 존재하는 경우 -> tool body와 구분하고 Unite 작업을 수행
		 * target body가 없는 경우	  -> Create 작업으로 Cutextrude feature 생성
		**/

		vector<Body *> bodyInPart = _Part->GetNXBodyList();	
		
		Body * toolBody;
		vector<Body *> tempTargetBody;

		/** tool body와 target body 선정 **/
		for ( int i = 0; i < bodyInPart.size(); ++i )
		{
			string testBodyJID = bodyInPart[i]->JournalIdentifier().GetUTF8Text();
			string toolBodyJID = cutExtrudeFeat->JournalIdentifier().GetUTF8Text();

			if ( testBodyJID.compare(toolBodyJID) == 0 )
				toolBody = bodyInPart[i];
			else
				tempTargetBody.push_back(bodyInPart[i]);
		}
	
		for ( int i = 0; i < tempTargetBody.size(); ++i )
		{		

			if ( this->isIntersect(toolBody, tempTargetBody[i]) )
			{
				_Part->_nxSession->UndoToMark(cutExtCreateUndo, "cutExtCreateUndo");

				targetBodies.push_back(tempTargetBody[i]);
				builder->BooleanOperation()->SetTargetBodies(targetBodies);

				builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeSubtract);
		
				// Cut extrude feature 생성(Body 단위)
				cutExtrudeFeat = builder->CommitFeature();
				
				/** Extrude feature JID 저장 **/
				g_pRefManager->AddNewJID(_spFeature, cutExtrudeFeat->JournalIdentifier().GetUTF8Text(), targetBodies[0]->JournalIdentifier().GetUTF8Text());
				cout << "CutExtrude Feature JID: " << cutExtrudeFeat->JournalIdentifier().GetUTF8Text() << endl;
			}
		}

		if ( targetBodies.empty() )
		{
			/** Extrude feature JID 저장 **/
			g_pRefManager->AddNewJID(_spFeature, cutExtrudeFeat->JournalIdentifier().GetUTF8Text(), "");
			cout << "CutExtrude Feature JID: " << cutExtrudeFeat->JournalIdentifier().GetUTF8Text() << endl;
		}
	}

	else if ( _featureProfileSketch->_refType == 2 )	// Brep
	{
		string refName = _featureProfileSketch->GetRefName();
		string featNameByBRep = g_pRefManager->ConvertToBRepFeature(refName);
		string strBF = g_pRefManager->GetJIDByTCName(featNameByBRep);
		Body * targetBody = _Part->_nxPart->Bodies()->FindObject(strBF);
		
		targetBodies.push_back(targetBody);

		builder->BooleanOperation()->SetTargetBodies(targetBodies);
		
		builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeSubtract);

		cutExtrudeFeat = builder->CommitFeature();


		/** Extrude feature JID 저장 **/
		g_pRefManager->AddNewJID(_spFeature, cutExtrudeFeat->JournalIdentifier().GetUTF8Text(), targetBodies[0]->JournalIdentifier().GetUTF8Text());
		cout << "CutExtrude Feature JID: " << cutExtrudeFeat->JournalIdentifier().GetUTF8Text() << endl;
	}


	builder->Destroy();
}

NXOpen::Features::ExtrudeBuilder * FeatureSOLIDCreateCutExtrude::cutExtrudeBuilderSet(FSketch * profileSketch)
{
	using namespace NXOpen;

	try
	{
		Features::ExtrudeBuilder * builder;
		builder = _Part->_nxPart->Features()->CreateExtrudeBuilder(NULL);

		Section * pSectinon;
		pSectinon = _Part->_nxPart->Sections()->CreateSection(0.02413, 0.0254, 0.5);

		builder->SetSection(pSectinon);
	

		string profileSketJID;
		profileSketJID = g_pRefManager->GetJIDByTCName(profileSketch->GetFeatureName());

		vector<Features::Feature *> profileSket;
		Features::SketchFeature * profileSketFeat = (Features::SketchFeature *)(_Part->_nxPart->Features()->FindObject(profileSketJID));
		profileSket.push_back(profileSketFeat);

		CurveFeatureRule * CFR;
		CFR = _Part->_nxPart->ScRuleFactory()->CreateRuleCurveFeature(profileSket);

		vector<SelectionIntentRule *> rule;
		rule.push_back(CFR);

		Point3d hp(0, 0, 0);
		pSectinon->AddToSection(rule, (NXObject*)NULL, (NXObject*)NULL, (NXObject*)NULL, hp, (Section::Mode)0, false);


		/** Extrude direction 정의 **/
		Sketch * usedSket = (Sketch *)profileSketFeat;

		Direction * cutExtrudeDir;
		cutExtrudeDir = _Part->_nxPart->Directions()->CreateDirection(usedSket, SenseReverse, SmartObject::UpdateOptionWithinModeling);
	
		if (!_flip)
		{
			builder->SetDirection(cutExtrudeDir);
		}
		else
		{
			cutExtrudeDir->ReverseDirection();
		}

		builder->SetDirection(cutExtrudeDir);

		/** Extrude depth 정의 **/
		// Start depth
		if (_startCondition == TransCAD::ThroughAll)
			builder->Limits()->StartExtend()->SetTrimType(GeometricUtilities::Extend::ExtendTypeThroughAll);
		else
			builder->Limits()->StartExtend()->Value()->SetRightHandSide(to_string(_startDepth));

		// End depth
		if (_endCondition == TransCAD::ThroughAll)
			builder->Limits()->EndExtend()->SetTrimType(GeometricUtilities::Extend::ExtendTypeThroughAll);
		else
			builder->Limits()->EndExtend()->Value()->SetRightHandSide(to_string(_endDepth));

		return builder;
	}

	catch (const NXException &ex)
	{
		cout << "   Error location [ CutExtrude feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}

BOOL FeatureSOLIDCreateCutExtrude::isIntersect(NXOpen::Body * toolBody, NXOpen::Body * targetBody)
{
	using namespace NXOpen;

	try
	{
		Features::BooleanBuilder * booleanBuilder;
		booleanBuilder = _Part->_nxPart->Features()->CreateBooleanBuilderUsingCollector(NULL);

		booleanBuilder->SetTolerance(0.00001);
		booleanBuilder->SetOperation(Features::Feature::BooleanTypeIntersect);

		GeometricUtilities::BooleanRegionSelect * booleanRegionSelect;
		booleanRegionSelect = booleanBuilder->BooleanRegionSelect();

		booleanBuilder->Targets()->Add(targetBody);
			
		vector<TaggedObject *> targets;
		targets.push_back(targetBody);

		booleanRegionSelect->AssignTargets(targets);

		ScCollector * scCollector;
		scCollector = _Part->_nxPart->ScCollectors()->CreateCollector();

		vector<Body *> tool;
		tool.push_back(toolBody);

		BodyDumbRule * bodyDumbRule;
		bodyDumbRule = _Part->_nxPart->ScRuleFactory()->CreateRuleBodyDumb(tool, true);

		vector<SelectionIntentRule *> rule;
		rule.push_back((SelectionIntentRule *)bodyDumbRule);
		scCollector->ReplaceRules(rule, false);

		booleanBuilder->SetToolBodyCollector(scCollector);

		NXObject * nxObject;
		nxObject = booleanBuilder->Commit();
		
		if ( nxObject )
		{
			booleanBuilder->Destroy();
			return true;
		}
			
		else
		{
			booleanBuilder->Destroy();
			return false;	
		}
		
	}

	catch (const NXException &ex)
	{
		cout << "   Error location [ CutExtrude feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}