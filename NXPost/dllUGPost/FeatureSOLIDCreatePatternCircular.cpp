#include "stdafx.h"
#include "FeatureSOLIDCreatePatternCircular.h"

/** UG Post's header files **/
#include "Part.h"
#include "UGReferenceManager.h"

/** UG NXOpen Header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Body.hxx>
#include <NXOpen\BodyCollection.hxx>
#include <NXOpen\Features_PatternFeature.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\Section.hxx>
#include <NXOpen\SectionCollection.hxx>

#include <NXOpen\Features_Extrude.hxx>
#include <NXOpen\Features_ExtrudeBuilder.hxx>
#include <NXOpen\Features_Revolve.hxx>
#include <NXOpen\Features_RevolveBuilder.hxx>
#include <NXOpen\Features_SweepAlongGuide.hxx>
#include <NXOpen\Features_SweepAlongGuideBuilder.hxx>


#include <NXOpen\Features_SelectFeatureList.hxx>
#include <NXOpen\GeometricUtilities_PatternOrientation.hxx>

#include <NXOpen\Direction.hxx>
#include <NXOpen\DirectionCollection.hxx>
#include <NXOpen\SmartObject.hxx>
#include <NXOpen\Point.hxx>
#include <NXOpen\PointCollection.hxx>
#include <NXOpen\Axis.hxx>
#include <NXOpen\AxisCollection.hxx>

#include <NXOpen\DatumAxis.hxx>
#include <NXOpen\DatumCollection.hxx>

//////////더 추가예정

using namespace std;

FeatureSOLIDCreatePatternCircular::FeatureSOLIDCreatePatternCircular(Part *pPart,TransCAD::IFeaturePtr spFeature):Feature(pPart,spFeature) {}

FeatureSOLIDCreatePatternCircular::~FeatureSOLIDCreatePatternCircular()
{}

void FeatureSOLIDCreatePatternCircular::GetInfo()
{
    TransCAD::IStdSolidOperatePatternCircularFeaturePtr spFeature=GetTransCADFeature();
    
	_angleIncrement=spFeature->GetAngleIncrement(); // 각 방향 간격
	_angleNum=spFeature->GetAngleNumber(); // 각 방향 갯수
	_radiNum=spFeature->GetRadialNumber(); // 반지름 방향 갯수
	_spaceByRadi=spFeature->GetRadialSpacing(); // 반지름 방향 간격
	_isAssign=spFeature->GetIsRadialAnignment(); // 원형 패턴 배치 형태
	spFeature->GetCenterAxis(&_centerAxis[0][0], &_centerAxis[0][1], &_centerAxis[0][2], &_centerAxis[1][0], &_centerAxis[1][1], &_centerAxis[1][2]); // 중심 축 벡터
	
    TransCAD::IReferencesPtr spReferences=spFeature->GetTargetFeatures();
	_numFtr=spReferences->GetCount();// 대상 특징 형상 갯수
    
	_targetFtrs=new string[_numFtr];
	/** 대상 특징 형상에 대한 정보 저장 **/
	for (int i=0;i<_numFtr;++i)
	{
		_targetFtrs[i]=spReferences->GetItem(i+1)->GetReferenceeName(); // GetItem은 매개변수가 1부터 시작, 0을 넘기면 런타임 에러 발생
	}
}
   
void FeatureSOLIDCreatePatternCircular::ToUG()
{
	try
	{
		using namespace NXOpen;

		Features::Feature *cptrnFeat;
		Features::PatternFeatureBuilder *builder;
		builder=this->patternBuilderSet();

		////////////////////////////////////////////////////////////////////////////////////////수정 필요
		/*vector<Body *> targetBodies;
		
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
		}*/

		cptrnFeat=builder->CommitFeature();

		//g_pRefManager->AddNewJID(_spFeature,cptrnFeat->JournalIdentifier().GetUTF8Text(),targetBodies[0]->JournalIdentifier().GetUTF8Text());
		cout<<"Circular feature JID: "<<cptrnFeat->JournalIdentifier().GetUTF8Text()<<endl;
		builder->Destroy();
	}
	catch(const NXOpen::NXException &ex)
	{
		cout<<"Error location [circular pattern feature]"<<endl;
		cout<<"Error code->"<<ex.ErrorCode()<<endl;
		cout<<"Error message->"<<ex.Message()<<endl;
	}
}


NXOpen::Features::PatternFeatureBuilder *FeatureSOLIDCreatePatternCircular::patternBuilderSet()
{
	try
	{
		using namespace NXOpen;

		Features::PatternFeatureBuilder *builder;
		builder=_Part->_nxPart->Features()->CreatePatternFeatureBuilder(NULL);
		
		for (int i=0;i<_numFtr;++i)
		{
			_targetFeature=_targetFtrs[i];
			Features::Feature * target_ftr(_Part->_nxPart->Features()->FindObject(g_pRefManager->GetJIDByTCName(_targetFeature)));
			builder->FeatureList()->Add(target_ftr);
		}

		builder->SetParentFeatureInternal(false);
		builder->PatternService()->SetPatternType(GeometricUtilities::PatternDefinition::PatternEnumCircular);
				
		Point3d centerAxisStart(_centerAxis[0][0],_centerAxis[0][1],_centerAxis[0][2]);
		Point3d centerAxisEnd(_centerAxis[1][0],_centerAxis[1][1],_centerAxis[1][2]);
		
		Point *startpoint=_Part->_nxPart->Points()->CreatePoint(centerAxisStart);
		Point *endpoint=_Part->_nxPart->Points()->CreatePoint(centerAxisEnd);

		Direction *centerVec;
		centerVec=_Part->_nxPart->Directions()->CreateDirection(startpoint,endpoint,SmartObject::UpdateOptionWithinModeling); // 중심축 세팅을 위한 방향 벡터 세팅
		Point *nullpoint(NULL);
		Axis *ctrAxis=_Part->_nxPart->Axes()->CreateAxis(nullpoint,centerVec,SmartObject::UpdateOptionWithinModeling); // 중심축 세팅
		builder->PatternService()->CircularDefinition()->SetRotationAxis(ctrAxis); // 패턴의 중심축 세팅
		builder->PatternService()->CircularDefinition()->SetRotationCenter(startpoint); // 원형 패턴에 필요한 회전 중심점 세팅
		/** 각 방향 세팅 **/
		builder->PatternService()->CircularDefinition()->AngularSpacing()->NCopies()->SetRightHandSide(to_string(_angleNum)); // 갯수
		builder->PatternService()->CircularDefinition()->AngularSpacing()->PitchAngle()->SetRightHandSide(to_string(_angleIncrement)); // 간격
		/** 반지름 방향 세팅 **/
		builder->PatternService()->CircularDefinition()->SetUseRadialDirectionToggle(true);
		builder->PatternService()->CircularDefinition()->RadialSpacing()->NCopies()->SetRightHandSide(to_string(_radiNum)); // 갯수
		builder->PatternService()->CircularDefinition()->RadialSpacing()->PitchDistance()->SetRightHandSide(to_string(_spaceByRadi)); // 간격
		
		return builder;
	}

	catch(const NXOpen::NXException &ex)
	{
		std::cout << ex.Message() <<std::endl;
	}

	return 0;
}