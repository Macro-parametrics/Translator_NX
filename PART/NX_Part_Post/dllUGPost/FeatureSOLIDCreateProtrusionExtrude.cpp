#include "stdafx.h"
#include "FeatureSOLIDCreateProtrusionExtrude.h"

/** UG Post's header files **/
#include "Part.h"
#include "FSketch.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Body.hxx>
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

#include <NXOpen\Features_BooleanBuilder.hxx>
#include <NXOpen\SelectBodyList.hxx>
#include <NXOpen\ScCollectorCollection.hxx>
#include <NXOpen\ScRuleFactory.hxx>

using namespace std;

FeatureSOLIDCreateProtrusionExtrude::FeatureSOLIDCreateProtrusionExtrude(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart, spFeature)
{

}

FeatureSOLIDCreateProtrusionExtrude::~FeatureSOLIDCreateProtrusionExtrude(void) {}

void FeatureSOLIDCreateProtrusionExtrude::GetInfo()
{
	TransCAD::IStdSolidProtrusionExtrudeFeaturePtr spFeature = GetTransCADFeature();
	
	/** Extrude의 깊이, 설정, 방향을 TransCAD에서 맴버 변수로 저장 **/
	_startDepth = (-1) * (spFeature->EndDepth);
	_endDepth = spFeature->StartDepth;

	_startCondition = spFeature->EndCondition;
	_endCondition = spFeature->StartCondition; 

	_flip = spFeature->IsFlip ? true : false;

	cout << "\n  Extrude startDepth = " << _startDepth << endl;
	cout << "  Extrude endDepth   = " << _endDepth << endl;

	/** Extrude의 profile sketch를 저장 **/
	TransCAD::IReferencePtr spReference = spFeature->ProfileSketch;
	string profileSketch = spReference->ReferenceeName;

	cout << "  Extrude Profile sketch = " << profileSketch << endl;

	_featureProfileSketch = (FSketch *)(GetPart()->GetFeatureByName(profileSketch));

}

void FeatureSOLIDCreateProtrusionExtrude::ToUG()
{
	try
	{
		using namespace NXOpen;

		Features::Feature * extrudeFeat;

		Features::ExtrudeBuilder * builder;
		builder = this->extrudeBuilderSet(_featureProfileSketch);


		/** Unite를 수행할 target body 선정 **/
		vector<Body *> targetBodies;
	
		if ( _featureProfileSketch->_refType == 1 )
		{
			Session::UndoMarkId extrudeCreateUndo;
			extrudeCreateUndo = _Part->_nxSession->SetUndoMark(Session::MarkVisibilityVisible, "extrudeCreateUndo");

			builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeCreate);

			// Extrude feature 생성
			extrudeFeat = builder->CommitFeature();

			/**
			 * Target body를 탐색
			 * target body가 존재하는 경우 -> tool body와 구분하고 Unite 작업을 수행
			 * target body가 없는 경우	  -> Create 작업으로 revolve feature 생성
			**/

			vector<Body *> bodyInPart = _Part->GetNXBodyList();
		
			Body * toolBody;
			vector<Body *> tempTargetBody;

			/** tool body와 target body 선정 **/
			for ( int i = 0; i < bodyInPart.size(); ++i )
			{
				string testBodyJID = bodyInPart[i]->JournalIdentifier().GetUTF8Text();
				string toolBodyJID = extrudeFeat->JournalIdentifier().GetUTF8Text();

				if ( testBodyJID.compare(toolBodyJID) == 0 )
					toolBody = bodyInPart[i];
				else
					tempTargetBody.push_back(bodyInPart[i]);
			}
	
			for ( int i = 0; i < tempTargetBody.size(); ++i )
			{		

				if ( this->isIntersect(toolBody, tempTargetBody[i]) )
				{
					_Part->_nxSession->UndoToMark(extrudeCreateUndo, "extrudeCreateUndo");

					targetBodies.push_back(tempTargetBody[i]);
					builder->BooleanOperation()->SetTargetBodies(targetBodies);

					builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeUnite);
		
					// Extrude feature 생성(Body 단위)
					extrudeFeat = builder->CommitFeature();
				
					/** Extrude feature JID 저장 **/
					g_pRefManager->AddNewJID(_spFeature, extrudeFeat->JournalIdentifier().GetUTF8Text(), targetBodies[0]->JournalIdentifier().GetUTF8Text());
					cout << "Extrude Feature JID: " << extrudeFeat->JournalIdentifier().GetUTF8Text() << endl;
				}
			}

			if ( targetBodies.empty() )
			{
				/** Extrude feature JID 저장 **/
				g_pRefManager->AddNewJID(_spFeature, extrudeFeat->JournalIdentifier().GetUTF8Text(), "");
				cout << "Extrude Feature JID: " << extrudeFeat->JournalIdentifier().GetUTF8Text() << endl;
			}
		}

		else if ( _featureProfileSketch->_refType == 2 )	// Brep
		{
			string refName = _featureProfileSketch->GetRefName();
			string featNameByBRep = g_pRefManager->ConvertToBRepFeature(refName);
			
			Feature * tempFeat = _Part->GetFeatureByName(featNameByBRep);
			
			if ( tempFeat->GetTransCADFeature()->Type == 3 )
			{				
				string strBF = g_pRefManager->GetJIDByTCName2(featNameByBRep);
				Body * targetBody = _Part->_nxPart->Bodies()->FindObject(strBF);
				targetBodies.push_back(targetBody);
			}
			else
			{				
				string strBF = g_pRefManager->GetJIDByTCName(featNameByBRep);
				Body * targetBody = _Part->_nxPart->Bodies()->FindObject(strBF);
				targetBodies.push_back(targetBody);
			}
			
			builder->BooleanOperation()->SetTargetBodies(targetBodies);

			builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeCreate);


			extrudeFeat = builder->CommitFeature();

			/** Revolve feature JID 저장 **/
			g_pRefManager->AddNewJID(_spFeature, extrudeFeat->JournalIdentifier().GetUTF8Text(), targetBodies[0]->JournalIdentifier().GetUTF8Text());
			cout << "Extrude Feature JID: " << extrudeFeat->JournalIdentifier().GetUTF8Text() << endl;
		}
	

		builder->Destroy();
	}
	
	catch (const NXOpen::NXException &ex)
	{
		cout << "   Error location [ Extrude feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}

NXOpen::Features::ExtrudeBuilder * FeatureSOLIDCreateProtrusionExtrude::extrudeBuilderSet(FSketch * profileSketch)
{
	try
	{
		using namespace NXOpen;

		Features::ExtrudeBuilder * builder;
		builder = _Part->_nxPart->Features()->CreateExtrudeBuilder(NULL);

		Section * pSectinon;
		pSectinon = _Part->_nxPart->Sections()->CreateSection(0.02413, 0.0254, 0.5);

		builder->SetSection(pSectinon);
		builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeCreate);

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
		Direction * extrudeDir;
		extrudeDir = _Part->_nxPart->Directions()->CreateDirection(usedSket, SenseForward, SmartObject::UpdateOptionWithinModeling);
	
		if (!_flip)
		{			
			builder->SetDirection(extrudeDir);
		}
		else
		{
			extrudeDir->ReverseDirection();
			builder->SetDirection(extrudeDir);
		}
	

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

	catch (const NXOpen::NXException &ex)
	{
		cout << "   Error location [ Extrude feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}

BOOL FeatureSOLIDCreateProtrusionExtrude::isIntersect(NXOpen::Body * toolBody, NXOpen::Body * targetBody)
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
		cout << "   Error location [ Revolve feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}