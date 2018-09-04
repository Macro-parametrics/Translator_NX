#include "stdafx.h"
#include "FDatumPlaneOffset.h"
#include "UGReferenceManager.h"

#include "Part.h"

/* UG NXOpen headers */
#include <NXOpen\Features_DatumPlaneBuilder.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\DatumCollection.hxx>
#include <NXOpen\DatumPlane.hxx>
#include <NXOpen\Plane.hxx>
#include <NXOpen\Face.hxx>

using namespace std;

FDatumPlaneOffset::FDatumPlaneOffset(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart, spFeature)
{
	
}

FDatumPlaneOffset::~FDatumPlaneOffset(void) {}

void FDatumPlaneOffset::GetInfo()
{
	TransCAD::IStdDatumPlaneOffsetFeaturePtr spFeature = GetTransCADFeature();
	TransCAD::IReferencePtr spRefernce = spFeature->GetSelectedPlane();
	
	_featurename = spRefernce->ReferenceeName;
	_distance = spFeature->Distance;
	_isFlip = spFeature->IsFlip ? true : false;

	if(_isFlip)
		_distance=(-1)*_distance;
	
	cout << endl;
	cout << "	" << "Name           : " << spFeature->Name << endl;
	cout << "	" << "Distance       : " << spFeature->Distance << endl;
	cout << "	" << "ReferenceeName : " << spRefernce->ReferenceeName << endl;
	cout << "	" << "Type           : " << spRefernce->Type << endl;

	// 해당 피쳐가 좌표평면인 경우, 아래와 같이 저장. 차 후 아래와 같은 형태로 쓰이기 때문
	// UG Datum_CSYS에서 XY를 선택 시, YZ로 표기됨....
	if (_featurename == "XYPlane")
		_coordinate = "YZ plane";
	else if (_featurename == "YZPlane")
		_coordinate = "XZ plane";
	else if (_featurename == "ZXPlane")
		_coordinate = "XY plane";
}

void FDatumPlaneOffset::ToUG()
{
	using namespace NXOpen;
	
	Features::DatumPlaneBuilder * DPbuilder;
	DPbuilder = _Part->_nxPart->Features()->CreateDatumPlaneBuilder(NULL);

	/** DatumPlane을 생성할 때, 사용되는 plane을 refPlane에 넣고 관련 정보를 업데이트 **/
	Plane * refPlane;
	refPlane = DPbuilder->GetPlane();
	refPlane->SetMethod(PlaneTypes::MethodTypeDistance);		// Plane을 생성하는 방법 정의

	/** TransCAD에서 얻은 Reference plane정보를 이용하여 JID를 찾기 **/
	
	NXString refJID = g_pRefManager->GetJIDByTCName(_featurename);
	DatumPlane * refDP = (dynamic_cast<DatumPlane *>(_Part->_nxPart->Datums()->FindObject(refJID)));
	
	/** 에러처리: Reference datum plane에 정보가 있는지 없는지 확인 **/
	if (refDP->IsBlanked())
		MessageBoxA(NULL, "Empty reference datum plane.. plase check reference datum plane", "Warning", MB_OK);

	/** setGeometry를 위한 데이터 타입 변환: DatumPlane -> NXObject **/
	vector<NXObject *> objects;
	objects.push_back(refDP);
	refPlane->SetGeometry(objects);
	
	/** NXstring type의 Offset 값을 사용하기 위한 형 변환: double -> NXString(=string) **/
	string offsetVal = to_string((_distance));
	Expression * exp = refPlane->Expression();
	exp->SetRightHandSide(offsetVal);

	refPlane->Evaluate();
	/** DatumPlane feature를 생성하기 위해 commit **/
	Features::Feature * dpFeat;
	dpFeat = DPbuilder->CommitFeature();
	// DEBUG cout << "DatumPlaneOffset's JID : " << dpFeat->JournalIdentifier().GetUTF8Text() << endl;

	/** Reference manager에 JID 추가 **/
	g_pRefManager->AddNewJID(_spFeature, dpFeat->JournalIdentifier().GetUTF8Text(), "");

	// DEBUG cout << "	" << "Name	: " << dpFeat->JournalIdentifier().GetUTF8Text() << endl;

	DPbuilder->Destroy();


}