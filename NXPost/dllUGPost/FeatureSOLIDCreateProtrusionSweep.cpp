#include "stdafx.h"
#include "FeatureSOLIDCreateProtrusionSweep.h"

#include <iostream>

/** UG Post's header files **/
#include "Part.h"
#include "FSketch.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\Features_SweepAlongGuideBuilder.hxx>
#include <NXOpen\Features_SketchFeature.hxx>
#include <NXOpen\CurveFeatureRule.hxx>
#include <NXOpen\ScRuleFactory.hxx>

using namespace std;

FeatureSOLIDCreateProtrusionSweep::FeatureSOLIDCreateProtrusionSweep(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart,spFeature)
{
}

FeatureSOLIDCreateProtrusionSweep::~FeatureSOLIDCreateProtrusionSweep(void)
{
}

void FeatureSOLIDCreateProtrusionSweep::GetInfo()
{
	TransCAD::IStdSolidProtrusionSweepFeaturePtr spFeature = GetTransCADFeature();
	TransCAD::IReferencePtr spProfile = spFeature->ProfileSketch;
	TransCAD::IReferencePtr spGuide = spFeature->GuideCurve;

	string profileName = spProfile->ReferenceeName;
	string guideName = spGuide->ReferenceeName;
	
	cout << " sweep profilesketch name = " << profileName << endl;
	cout << " sweep guidesketch name = " << guideName << endl;
	
	_profileSketch = (FSketch*)(GetPart()->GetFeatureByName(profileName));
	_guideSketch = (FSketch*)(GetPart()->GetFeatureByName(guideName));

}

void FeatureSOLIDCreateProtrusionSweep::ToUG()
{
	try
	{
		using namespace NXOpen;

		Features::SweepAlongGuideBuilder * builder;
		builder = _Part->_nxPart->Features()->CreateSweepAlongGuideBuilder(NULL);

		builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeCreate);

		string profileSketJID;
		profileSketJID = g_pRefManager->GetJIDByTCName(_profileSketch->GetFeatureName());
	
		vector<Features::Feature *> profileSket;
		Features::SketchFeature * profileSketFeat = (Features::SketchFeature *)(_Part->_nxPart->Features()->FindObject(profileSketJID));
		profileSket.push_back(profileSketFeat);

		CurveFeatureRule * profileCFR;
		profileCFR = _Part->_nxPart->ScRuleFactory()->CreateRuleCurveFeature(profileSket);

		builder->Section()->AllowSelfIntersection(true);

		vector<SelectionIntentRule *> profileRule;
		profileRule.push_back(profileCFR);

		Point3d hp(0, 0, 0);
		builder->Section()->AddToSection(profileRule, (NXObject*)NULL, (NXObject*)NULL, (NXObject*)NULL, hp, (Section::Mode)0, false);


		string guideSketJID;
		guideSketJID = g_pRefManager->GetJIDByTCName(_guideSketch->GetFeatureName());
	
		vector<Features::Feature *> guideSket;
		Features::SketchFeature * guideSketFeat = (Features::SketchFeature *)(_Part->_nxPart->Features()->FindObject(guideSketJID));
		guideSket.push_back(guideSketFeat);

		CurveFeatureRule * guideCFR;
		guideCFR = _Part->_nxPart->ScRuleFactory()->CreateRuleCurveFeature(guideSket);

		builder->Guide()->AllowSelfIntersection(true);

		vector<SelectionIntentRule *> guideRule;
		guideRule.push_back(guideCFR);

		builder->Guide()->AddToSection(guideRule, (NXObject*)NULL, (NXObject*)NULL, (NXObject*)NULL, hp, (Section::Mode)0, false);

		Features::Feature * sweepFeat;
		sweepFeat = builder->CommitFeature();

		builder->Destroy();

		/** Sweep feature JID ÀúÀå **/
		g_pRefManager->AddNewJID(_spFeature, sweepFeat->JournalIdentifier().GetUTF8Text(), "");
		cout << "Sweep Feature JID: " << sweepFeat->JournalIdentifier().GetUTF8Text() << endl;
	}

	catch ( const NXOpen::NXException &ex)
	{
		cout << "   Error location [ Sweep feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}