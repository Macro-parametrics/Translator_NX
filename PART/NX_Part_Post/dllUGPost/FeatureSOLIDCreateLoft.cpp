#include "stdafx.h"
#include "FeatureSOLIDCreateLoft.h"

#include "Part.h"
#include "FSketch.h"
#include "UGReferenceManager.h"

/** UG NXOpen Header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Body.hxx>
#include <NXOpen\BodyCollection.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\Features_ThroughCurves.hxx>
#include <NXOpen\Point.hxx>
#include <NXOpen\PointCollection.hxx>
#include <NXOpen\Sketch.hxx>
#include <NXOpen\SketchCollection.hxx>
#include <NXOpen\Features_SketchFeature.hxx>
#include <NXOpen\CurveFeatureRule.hxx>
#include <NXOpen\ScRuleFactory.hxx>
#include <NXOpen\Section.hxx>
#include <NXOpen\SectionCollection.hxx>
#include <NXOpen\SectionList.hxx>
#include <NXOpen\SelectionIntentRule.hxx>


FeatureSOLIDCreateLoft::FeatureSOLIDCreateLoft(Part * pPart, TransCAD::IFeaturePtr spFeature):Feature(pPart,spFeature) {}

FeatureSOLIDCreateLoft::~FeatureSOLIDCreateLoft(void) {}


void FeatureSOLIDCreateLoft::GetInfo()
{
	TransCAD::IStdSolidLoftSectionsFeaturePtr _spFeature=GetTransCADFeature();
	TransCAD::IReferencePtr _spReference;
	TransCAD::IReferencesPtr _spReferences=_spFeature->GetReferences();
	_sketchNumber=_spReferences->Count;
	_isCut=_spFeature->IsCut;
	
	_tcSketch=new string[_sketchNumber];

	for (int i=1;i<=_sketchNumber;++i)
	{
		_spReference=_spReferences->Item[i];
		_tcSketch[i-1]=_spReference->GetReferenceeName();
		//cout<<"********"<<_tcSketch[i-1]<<"***********"<<endl;
	}
}

void FeatureSOLIDCreateLoft::ToUG()
{
	try
	{
		using namespace NXOpen;

		Features::Feature *loftFeat;
		Features::ThroughCurvesBuilder *builder;
		builder=this->loftBuilderSet();

		vector<Body *> targetBodies;

		loftFeat=builder->CommitFeature();

		bool judgeWhetherRecord=FALSE;///////////

		for(int i=1;i<=_sketchNumber;++i)
		{
			FSketch *_psket=(FSketch *)(GetPart()->GetFeatureByName(_tcSketch[i-1]));
			if(_psket->_refType==1)
			{
				vector<Body *> bodyInPart=_Part->GetNXBodyList();

				Body *toolBody;
				vector<Body *> tempTargetBody;

				for(int j=0;j<bodyInPart.size();++j)
				{
					string testBodyJID=bodyInPart[j]->JournalIdentifier().GetUTF8Text();
					string toolBodyJID=loftFeat->JournalIdentifier().GetUTF8Text();

					if(testBodyJID.compare(toolBodyJID)==0)
						toolBody=bodyInPart[j];
					else
						tempTargetBody.push_back(bodyInPart[j]);
				}
				for(int j=0;j<tempTargetBody.size();++j)
				{
					targetBodies.push_back(tempTargetBody[j]);
				}
			}
			else if(_psket->_refType==2)
			{
				string refName=_psket->GetRefName();
				string featNameByBRep=g_pRefManager->ConvertToBRepFeature(refName);

				Feature *tempFeat=_Part->GetFeatureByName(featNameByBRep);
				if(tempFeat->GetTransCADFeature()->Type==3)
				{
					string strBF=g_pRefManager->GetJIDByTCName2(featNameByBRep);
					Body *targetBody=_Part->_nxPart->Bodies()->FindObject(strBF);
					targetBodies.push_back(targetBody);
				}
				else
				{
					string strBF = g_pRefManager->GetJIDByTCName(featNameByBRep);
					Body *targetBody = _Part->_nxPart->Bodies()->FindObject(strBF);
					targetBodies.push_back(targetBody);
				}
				g_pRefManager->AddNewJID(_spFeature,loftFeat->JournalIdentifier().GetUTF8Text(),targetBodies[0]->JournalIdentifier().GetUTF8Text());
				cout<<"Loft Feature JID: "<<loftFeat->JournalIdentifier().GetUTF8Text()<<endl;
				judgeWhetherRecord=TRUE;////////////
			}
		}

		if(targetBodies.empty())
		{
			g_pRefManager->AddNewJID(_spFeature,loftFeat->JournalIdentifier().GetUTF8Text(),"");
			cout<<"Loft Feature JID: "<<loftFeat->JournalIdentifier().GetUTF8Text()<<endl;
		}
		else if(!(targetBodies.empty()) && (judgeWhetherRecord==FALSE))
		{
			g_pRefManager->AddNewJID(_spFeature,loftFeat->JournalIdentifier().GetUTF8Text(),targetBodies[0]->JournalIdentifier().GetUTF8Text());
			cout<<"Loft Feature JID: "<<loftFeat->JournalIdentifier().GetUTF8Text()<<endl;
		}
		
		builder->Destroy();
	}
	catch(const NXOpen::NXException &ex)
	{
		cout<<"Error location [loft feature]"<<endl;
		cout<<"Error code->"<<ex.ErrorCode()<<endl;
		cout<<"Error message->"<<ex.Message()<<endl;
	}

	delete []_tcSketch;
}

NXOpen::Features::ThroughCurvesBuilder *FeatureSOLIDCreateLoft::loftBuilderSet()
{
	try
	{
		using namespace NXOpen;

		Features::ThroughCurvesBuilder *builder;
		builder=_Part->_nxPart->Features()->CreateThroughCurvesBuilder(NULL);

		builder->SetPreserveShape(false);
		builder->SetPatchType(Features::ThroughCurvesBuilder::PatchTypesMultiple);

		builder->Alignment()->AlignCurve()->SetDistanceTolerance(0.01);
		builder->Alignment()->AlignCurve()->SetChainingTolerance(0.0095);
		builder->SectionTemplateString()->SetDistanceTolerance(0.01);
		builder->SectionTemplateString()->SetChainingTolerance(0.0095);
		builder->Alignment()->AlignCurve()->SetAngleTolerance(0.5);
		builder->SectionTemplateString()->SetAngleTolerance(0.5);

		Features::SketchFeature *nxSketchFeature;
		CurveFeatureRule *CFR;
		string _oneOfTargetSketch;
		Point3d helpPoint(0.0,0.0,0.0);
		NXObject *nullObj(NULL);
		Section **sctn= new Section*[_sketchNumber];
		//Section *sctn[4];
		
		for (int i=1;i<_sketchNumber+1;++i)
		{	
			sctn[i-1]=_Part->_nxPart->Sections()->CreateSection(0.0095, 0.01, 0.5);

			builder->SectionsList()->Append(sctn[i-1]);

			sctn[i-1]->SetAllowedEntityTypes(Section::AllowTypesCurvesAndPoints);

			vector<Features::Feature *> ftr;
			_oneOfTargetSketch=g_pRefManager->GetJIDByTCName(_tcSketch[i-1]);
			nxSketchFeature=dynamic_cast<Features::SketchFeature *>(_Part->_nxPart->Features()->FindObject(_oneOfTargetSketch));
			ftr.push_back(nxSketchFeature);
			CFR=_Part->_nxPart->ScRuleFactory()->CreateRuleCurveFeature(ftr);
			sctn[i-1]->AllowSelfIntersection(false);

			vector<SelectionIntentRule *> SIR;
			SIR.push_back(CFR);

			sctn[i-1]->AddToSection(SIR,nullObj,nullObj,nullObj,helpPoint,Section::ModeCreate,false);
			
			vector<Section *>sctns;
			for (int j=0;j<i;++j)
			{
				sctns.push_back(sctn[j]);
			}
			builder->Alignment()->SetSections(sctns);
		}

		for (int i=0;i<_sketchNumber;i++)
		{
			delete sctn[i];
		}

		delete []sctn;
		return builder;
	}

	catch(const NXOpen::NXException &ex)
	{
		std::cout << ex.Message() <<std::endl;
	}
}