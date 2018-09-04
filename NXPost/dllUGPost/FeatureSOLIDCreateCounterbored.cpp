#include "stdafx.h"
#include "FeatureSOLIDCreateCounterbored.h"

#include "Part.h"
#include "UGReferenceManager.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen/GeometricUtilities_BooleanOperation.hxx>
#include <NXOpen/GeometricUtilities_EndHoleData.hxx>
#include <NXOpen/GeometricUtilities_MiddleHoleData.hxx>
#include <NXOpen/GeometricUtilities_ProjectionOptions.hxx>
#include <NXOpen/GeometricUtilities_StartHoleData.hxx>
#include <NXOpen\Body.hxx>
#include <NXOpen\BodyCollection.hxx>
#include <NXOpen\Point.hxx>
#include <NXOpen\PointCollection.hxx>
/////////////
#include <NXOpen\Sketch.hxx>
#include <NXOpen\SketchInPlaceBuilder.hxx>
#include <NXOpen\SketchCollection.hxx>
#include <NXOpen\Features_SketchFeature.hxx>
#include <NXOpen\PointCollection.hxx>
#include <NXOpen\DatumCollection.hxx>
#include <NXOpen\SelectISurface.hxx>
#include <NXOpen\SelectBody.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\DatumPlane.hxx>
#include <NXOpen\Face.hxx>
#include <NXOpen\Features_Feature.hxx>

#include <NXOpen\Edge.hxx>
#include <NXOpen\GeometricAnalysis_GeometricProperties.hxx>
#include <NXOpen\GeometricAnalysis_AnalysisManager.hxx>
#include <NXOpen\FeaturePointsRule.hxx>
#include <NXOpen\ScRuleFactory.hxx>//
#include <NXOpen\Features_SketchFeature.hxx>
#include <NXOpen\Direction.hxx>

using namespace std;

FeatureSOLIDCreateCounterbored::FeatureSOLIDCreateCounterbored(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart, spFeature)
{
}

FeatureSOLIDCreateCounterbored::~FeatureSOLIDCreateCounterbored(void) {}

void FeatureSOLIDCreateCounterbored::GetInfo()
{
	TransCAD::IStdSolidHoleCounterboredFeaturePtr  spFeature=GetTransCADFeature();
	TransCAD::IReferencePtr spReference =spFeature->GetTargetFace();

	_targetFace=spReference->GetReferenceeName();
	spFeature->GetCenterPoint(&_center[0],&_center[1],&_center[2]);
	_startDepth=	spFeature->StartDepth;
	_endDepth=spFeature->EndDepth;
	_startDiameter=2*(spFeature->StartRadius);
	_endDiameter=2*(spFeature->EndRadius);
	_angle=spFeature->GetAngle();
	
	cout << "\n  FeatureSOLIDCreateCounterbored startDepth = " << _startDepth << endl;
	cout << "  FeatureSOLIDCreateCounterbored endDepth   = " << _endDepth << endl;
	cout << "FeatureSOLIDCreateCounterbored startDepth = " << _startDiameter << endl;
	cout << "  FeatureSOLIDCreateCounterbored endDepth   = " << _endDiameter << endl;
}

void FeatureSOLIDCreateCounterbored::ToUG()
{
	try
	{
		using namespace NXOpen;

		Features::Feature *cntbrd;
		Features::HolePackageBuilder *builder;
		builder=this->HolePakageBuilderSet();
		

		vector<Body *> targetBodies;
		
		/*Point3D pnt(_center[0],_center[1],_center[2]);
		string _targetFeature=GetBodyJID(pnt);*/
		string _targetFeature=g_pRefManager->ConvertToBRepFeature(_targetFace);
		Feature *tempFeat=_Part->GetFeatureByName(_targetFeature);
		
		if(tempFeat->GetTransCADFeature()->Type==3)
		{
			string strBF=g_pRefManager->GetJIDByTCName2(_targetFeature);
			Body *targetBody=_Part->_nxPart->Bodies()->FindObject(strBF);
			targetBodies.push_back(targetBody);
		}
		else
		{
			string strBF=g_pRefManager->GetJIDByTCName(_targetFeature);
			Body *targetBody=_Part->_nxPart->Bodies()->FindObject(strBF);
			targetBodies.push_back(targetBody);
		}

		cntbrd=builder->CommitFeature();

		g_pRefManager->AddNewJID(_spFeature,cntbrd->JournalIdentifier().GetUTF8Text(),targetBodies[0]->JournalIdentifier().GetUTF8Text());
		cout<<"counterbored feature JID: "<<cntbrd->JournalIdentifier().GetUTF8Text()<<endl;

		builder->Destroy();
	}
	catch(const NXOpen::NXException &ex)
	{
		cout<<"Error location [counterbored]"<<endl;
		cout<<"Error code->"<<ex.ErrorCode()<<endl;
		cout<<"Error message->"<<ex.Message()<<endl;
	}
}

NXOpen::Features::HolePackageBuilder * FeatureSOLIDCreateCounterbored::HolePakageBuilderSet()
{
	try
	{
		using namespace NXOpen;

		Features::HolePackageBuilder *builder;
		builder=_Part->_nxPart->Features()->CreateHolePackageBuilder(NULL);

		/* counterbored를 위한 sketch*/
		SketchInPlaceBuilder *bSket;
		bSket=_Part->_nxPart->Sketches()->CreateNewSketchInPlaceBuilder(NULL);
		Point3d Origin(_center[0],_center[1],_center[2]);
		Point *bOrigin=_Part->_nxPart->Points()->CreatePoint(Origin);
		bSket->SetSketchOrigin(bOrigin);
		bSket->SetPlaneOption(Sketch::PlaneOption::PlaneOptionExistingPlane);

		string baseFeatNameByBRep=g_pRefManager->ConvertToBRepFeature(_targetFace);
		string baseFeatJID=g_pRefManager->GetJIDByTCName(baseFeatNameByBRep);
		Features::Feature *baseFtr=_Part->_nxPart->Features()->FindObject(baseFeatJID);
		
		Point3D ori(_center[0],_center[1],_center[2]);
		string ugFace=GetFaceJID(ori);
		Face *skchRefFace=dynamic_cast<Face *>(baseFtr->FindObject(ugFace));

		bSket->PlaneOrFace()->SetValue(skchRefFace);
		
		Sketch * pSkch(NULL);
		NXObject *skchObject=bSket->Commit();
		pSkch=(Sketch *)skchObject;
		
		Features::Feature *skchFeat;
		skchFeat=pSkch->Feature();
		string thisSkch=pSkch->JournalIdentifier().GetUTF8Text();
		string thisSkchJID=skchFeat->JournalIdentifier().GetUTF8Text();
		g_pRefManager->AddNewJID(_spFeature,thisSkchJID,"");
		pSkch->Activate(Sketch::ViewReorientFalse);
		bSket->Destroy();

		//Point3d orgInSket(0.0,0.0,0.0);
		//Point *pOrgInSket=_Part->_nxPart->Points()->CreatePoint(orgInSket);
		Point3d orgInSket(_center[0],_center[1],_center[2]);
		Point *pOrgInSket=_Part->_nxPart->Points()->CreatePoint(orgInSket);
		Sketch *tSket;
		tSket=_Part->_nxPart->Sketches()->FindObject(thisSkch);
		tSket->AddGeometry(pOrgInSket);

		pSkch->Deactivate(Sketch::ViewReorientFalse,Sketch::UpdateLevelSketchOnly);
		/*sketch 끝*/
		
		vector<Features::Feature *> profileSket;
		
		Features::SketchFeature * profileSketFeat = (Features::SketchFeature *)(_Part->_nxPart->Features()->FindObject(thisSkchJID));
		profileSket.push_back(profileSketFeat);

		FeaturePointsRule * FPR;
		FPR = _Part->_nxPart->ScRuleFactory()->CreateRuleFeaturePoints(profileSket);

		vector<SelectionIntentRule *> rule;
		rule.push_back(FPR);

		Point3d hp(0, 0, 0);
		
		builder->HolePosition()->AddToSection(rule, (NXObject*)NULL, (NXObject*)NULL, (NXObject*)NULL, hp, Section::ModeCreate, false);
		builder->HolePosition()->SetAllowedEntityTypes(NXOpen::Section::AllowTypesOnlyPoints);
		builder->HolePosition()->AllowSelfIntersection(true);

		vector<Body *> targetbodies;
		//Body *bdy=_Part->_nxPart->Bodies()->FindObject("EXTRUDE(2)");
		string tbdy=GetBodyJID(ori);
		Body *bdy=_Part->_nxPart->Bodies()->FindObject(tbdy);
		targetbodies.push_back(bdy);
		builder->BooleanOperation()->SetType(GeometricUtilities::BooleanOperation::BooleanTypeSubtract);
		builder->BooleanOperation()->SetTargetBodies(targetbodies);
		

		Direction *nulldir(NULL);
		builder->ProjectionDirection()->SetProjectVector(nulldir);

		builder->SetGeneralHoleForm(Features::HolePackageBuilder::HoleFormsCounterbored);
		builder->GeneralCounterboreDiameter()->SetRightHandSide(to_string(_startDiameter));
		builder->GeneralCounterboreDepth()->SetRightHandSide(to_string(_startDepth));

		builder->GeneralCounterboreHoleDiameter()->SetRightHandSide(to_string(_endDiameter));
		builder->GeneralCounterboreHoleDepth()->SetRightHandSide(to_string(_endDepth));

		builder->GeneralTipAngle()->SetRightHandSide(to_string(_angle));

		//builder->SetParentFeatureInternal(false);
		////////////////////
		builder->SetTolerance(0.0001);
		builder->SetParentFeatureInternal(true);
    
		return builder;
	}
	catch(const NXOpen::NXException &ex)
	{
		cout<<"Error location [counterbored]"<<endl;
		cout<<"Error code->"<<ex.ErrorCode()<<endl;
		cout<<"Error message->"<<ex.Message()<<endl;
	}
}

string FeatureSOLIDCreateCounterbored::GetFaceJID(Point3D origin)
{
	using namespace NXOpen;

	string faceJID;

	vector<Face *> faceList;
	vector<Body *> bodyList;

	bodyList = _Part->GetNXBodyList();
	
	for(int i=0;i<bodyList.size();++i)
	{
		faceList=bodyList[i]->GetFaces();
		
		for(int j=0;j<faceList.size();++j)
		{
			Point3d targetPoint(origin.X(),origin.Y(),origin.Z());
			
			NXObject *sourceFace=(NXObject *)faceList[j];
			GeometricAnalysis::GeometricProperties::Face faceProp;

			GeometricAnalysis::GeometricProperties *geometryObject=_Part->_nxPart->AnalysisManager()->CreateGeometricPropertiesObject();
			geometryObject->GetFaceProperties(sourceFace,targetPoint,&faceProp);
			Point3d p=faceProp.PositionInWcs;

			p.X=ValRnd9(p.X);
			p.Y=ValRnd9(p.Y);
			p.Z=ValRnd9(p.Z);

			if(p.X==targetPoint.X && p.Y==targetPoint.Y && p.Z==targetPoint.Z)
			{
				faceJID=faceList[j]->JournalIdentifier().GetUTF8Text();
				break;
			}
		}

		faceList.clear();
	}
	
	return faceJID;
}

string FeatureSOLIDCreateCounterbored::GetBodyJID(Point3D origin)
{
	using namespace NXOpen;

	string bodyJID;

	vector<Face *> faceList;
	vector<Body *> bodyList;

	bodyList = _Part->GetNXBodyList();
	
	for(int i=0;i<bodyList.size();++i)
	{
		faceList=bodyList[i]->GetFaces();
		
		for(int j=0;j<faceList.size();++j)
		{
			Point3d targetPoint(origin.X(),origin.Y(),origin.Z());
			
			NXObject *sourceFace=(NXObject *)faceList[j];
			GeometricAnalysis::GeometricProperties::Face faceProp;

			GeometricAnalysis::GeometricProperties *geometryObject=_Part->_nxPart->AnalysisManager()->CreateGeometricPropertiesObject();
			geometryObject->GetFaceProperties(sourceFace,targetPoint,&faceProp);
			Point3d p=faceProp.PositionInWcs;

			p.X=ValRnd9(p.X);
			p.Y=ValRnd9(p.Y);
			p.Z=ValRnd9(p.Z);

			if(p.X==targetPoint.X && p.Y==targetPoint.Y && p.Z==targetPoint.Z)
			{
				bodyJID=bodyList[i]->JournalIdentifier().GetUTF8Text();
				break;
			}
		}

		faceList.clear();
	}
	
	return bodyJID;
}