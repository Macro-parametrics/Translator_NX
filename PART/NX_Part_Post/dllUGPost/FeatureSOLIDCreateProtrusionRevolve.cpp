#include "stdafx.h"
#include "FeatureSOLIDCreateProtrusionRevolve.h"

#include <iostream>

/** UG Post's header files **/
#include "Part.h"
#include "FSketch.h"
#include "SKETCHCreate2DCenterline.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Features_RevolveBuilder.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\GeometricUtilities_BooleanOperation.hxx>
#include <NXOpen\Features_SketchFeature.hxx>
#include <NXOpen\CurveFeatureRule.hxx>
#include <NXOpen\ScRuleFactory.hxx>
#include <NXOpen\SectionCollection.hxx>
#include <NXOpen\BodyCollection.hxx>
#include <NXOpen\GeometricUtilities_Limits.hxx>
#include <NXOpen\GeometricUtilities_Extend.hxx>
#include <NXOpen\SketchCollection.hxx>
#include <NXOpen\Line.hxx>
#include <NXOpen\DirectionCollection.hxx>
#include <NXOpen\AxisCollection.hxx>

#include <NXOpen\Body.hxx>
#include <NXOpen\Features_BooleanBuilder.hxx>
#include <NXOpen\SelectBodyList.hxx>
#include <NXOpen\ScCollectorCollection.hxx>
#include <NXOpen\ScRuleFactory.hxx>



using namespace std;

FeatureSOLIDCreateProtrusionRevolve::FeatureSOLIDCreateProtrusionRevolve(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart,spFeature)
{
}

FeatureSOLIDCreateProtrusionRevolve::~FeatureSOLIDCreateProtrusionRevolve(void)
{
}

void FeatureSOLIDCreateProtrusionRevolve::GetInfo()
{
	TransCAD::IStdSolidProtrusionRevolveFeaturePtr spFeature = GetTransCADFeature();
	double _angle;
	
	_startAngle = spFeature->EndAngle;
	_endAngle = spFeature->StartAngle;
	
	_angle = _startAngle-_endAngle;

	if (abs(_angle) > 360.0)
		_angle = 360.0;

	_flip = spFeature->IsFlip?true:false;	// VARIANT_BOOL (for TransCAd) -> bool Cast
	
	cout << " Revolve startAngle-> " << _startAngle << endl; //80
	cout << " Revolve endAngle-> " << _endAngle << endl;

	TransCAD::IReferencePtr spReference = spFeature->ProfileSketch;
	string sketchName = spReference->ReferenceeName;
	cout << " Revolve Profile sketch-> " << sketchName.c_str() <<endl;

	cout << " RevolveAngle : " << (abs(_angle)/180*PI) << endl;
	
	
	_featureProfileSketch = (FSketch*)(GetPart()->GetFeatureByName(sketchName));

}

void FeatureSOLIDCreateProtrusionRevolve::ToUG()
{
	try
	{
		using namespace NXOpen;

		Features::RevolveBuilder * builder;
		builder = this->revolveBuilderSet(_featureProfileSketch, _startAngle, _endAngle);
	

		/** Unite를 수행할 target body 선정 **/
		vector<Body *> targetBodies;
		Features::Feature * revolveFeat;
	
		if ( _featureProfileSketch->_refType == 1 )
		{
			Session::UndoMarkId revolveCreateUndo;
			revolveCreateUndo = _Part->_nxSession->SetUndoMark(Session::MarkVisibilityVisible, "revolveCreateUndo");

			builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeCreate);

			// Revolve feature 생성
			revolveFeat = builder->CommitFeature();

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
				string toolBodyJID = revolveFeat->JournalIdentifier().GetUTF8Text();

				if ( testBodyJID.compare(toolBodyJID) == 0 )
					toolBody = bodyInPart[i];
				else
					tempTargetBody.push_back(bodyInPart[i]);
			}
	
			for ( int i = 0; i < tempTargetBody.size(); ++i )
			{		

				if ( this->isIntersect(toolBody, tempTargetBody[i]) )
				{
					_Part->_nxSession->UndoToMark(revolveCreateUndo, "revolveCreateUndo");

					targetBodies.push_back(tempTargetBody[i]);
					builder->BooleanOperation()->SetTargetBodies(targetBodies);

					builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeUnite);
		
					// Revolve feature 생성(Body 단위)
					revolveFeat = builder->CommitFeature();
				
					/** Revolve feature JID 저장 **/
					g_pRefManager->AddNewJID(_spFeature, revolveFeat->JournalIdentifier().GetUTF8Text(), targetBodies[0]->JournalIdentifier().GetUTF8Text());
					cout << "Revolve Feature JID: " << revolveFeat->JournalIdentifier().GetUTF8Text() << endl;
				}
			}

			if ( targetBodies.empty() )
			{
				/** Revolve feature JID 저장 **/
				g_pRefManager->AddNewJID(_spFeature, revolveFeat->JournalIdentifier().GetUTF8Text(), "");
				cout << "Revolve Feature JID: " << revolveFeat->JournalIdentifier().GetUTF8Text() << endl;
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

			builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeUnite);


			revolveFeat = builder->CommitFeature();

			/** Revolve feature JID 저장 **/
			g_pRefManager->AddNewJID(_spFeature, revolveFeat->JournalIdentifier().GetUTF8Text(), targetBodies[0]->JournalIdentifier().GetUTF8Text());
			cout << "Revolve Feature JID: " << revolveFeat->JournalIdentifier().GetUTF8Text() << endl;
		}
	

		builder->Destroy();
	}

	catch (const NXOpen::NXException &ex)
	{
		cout << "   Error location [ Revolve feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
	
}

NXOpen::Features::RevolveBuilder * FeatureSOLIDCreateProtrusionRevolve::revolveBuilderSet(FSketch * profileSketch, double sA, double eA)
{
	try
	{
		using namespace NXOpen;

		Features::RevolveBuilder * builder;
		builder = _Part->_nxPart->Features()->CreateRevolveBuilder(NULL);
	
		builder->SetTolerance(0.01);

		Section * pSectinon;
		pSectinon = _Part->_nxPart->Sections()->CreateSection(0.02413, 0.0254, 0.5);

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

		builder->SetSection(pSectinon);

		/** Revolve 회전축 정의 **/
		string axisJID;
		axisJID = _featureProfileSketch->GetSketchName();
	
		Sketch * targetSketch;
		targetSketch = _Part->_nxPart->Sketches()->FindObject(axisJID);
	

		FSketch * pSketch = (FSketch *)_featureProfileSketch;
		SKETCHCreate2DCenterline * pCenterLine;
		for ( int i = 0; i < pSketch->GetSketchItemsSize(); ++i )
		{
			//string sketchItemName = pSketch->GetSketchItem(i)->GetSketchItemName();
			
			if ( pSketch->GetSketchItem(i)->GetSketType() == 1 )
			{
				pCenterLine = (SKETCHCreate2DCenterline *)pSketch->GetSketchItem(i);
				break;
			}
			
		}
	
		Line * centerLine(dynamic_cast<Line *>(targetSketch->FindObject(pCenterLine->GetCenterLineJID())));
		Direction * direction;
		direction = _Part->_nxPart->Directions()->CreateDirection(centerLine, SenseReverse, SmartObject::UpdateOptionWithinModeling);
    
		Axis * axis;
		axis = _Part->_nxPart->Axes()->CreateAxis(NULL, direction, SmartObject::UpdateOptionWithinModeling);
    
		builder->SetAxis(axis);

		/** Revolve angle 정의 **/
		builder->Limits()->StartExtend()->Value()->SetRightHandSide(to_string(sA));
		builder->Limits()->EndExtend()->Value()->SetRightHandSide(to_string(eA));

		return builder;
	}

	catch (const NXOpen::NXException &ex)
	{
		cout << "   Error location [ Revolve feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}

BOOL FeatureSOLIDCreateProtrusionRevolve::isIntersect(NXOpen::Body * toolBody, NXOpen::Body * targetBody)
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