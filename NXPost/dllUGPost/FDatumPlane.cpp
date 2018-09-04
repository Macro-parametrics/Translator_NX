#include "stdafx.h"
#include "FDatumPlane.h"
#include "UGReferenceManager.h"

#include "Part.h"

/* UG NXOpen headers */
#include "NXOpen\DatumCollection.hxx"
#include "NXOpen\DatumPlane.hxx"


using namespace std;

/* 이 클래스 생성자는 사용하지 않는다. */
FDatumPlane::FDatumPlane(Part * pPart, Point3D &origin, Direct3D &normal)
	: Feature(pPart, NULL)
{
	_origin = origin;
	_normal = normal;
}

/* Part.cpp에서 사용하는 클래스 생성자는 이것 */
FDatumPlane::FDatumPlane(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart, spFeature)
{

}

FDatumPlane::~FDatumPlane(void) {}

void FDatumPlane::GetInfo()
{
	string tcfeatureName = GetFeatureName();
	
	_origin = Point3D(0, 0, 0);

	if ( tcfeatureName == string("XYPlane") )
	{
		_normal = Direct3D(0, 0, 1);
		// DEBUG
		// cout << " -> is translated ! " << endl;
	}
	else if ( tcfeatureName == string("YZPlane") )
	{
		_normal = Direct3D(1, 0, 0);
		// DEBUG
		//cout << " -> is translated ! " << endl;
	}
	else if ( tcfeatureName == string("ZXPlane") )
	{
		_normal = Direct3D(0, 1, 0);
		// DEBUG
		//cout << " -> is translated ! " << endl;
	}
}

void FDatumPlane::ToUG()
{
	//cout << "Basis Datum coordinate system already exist" << endl;
	
	/** DatumPlane JID 얻기 **/
	string planeName;

	// UG의 CSYS에서 XY를 선택하면 JID가 YZ로 표기
	if ( _normal == Direct3D(0, 0, 1) )
		planeName = "YZ";

	else if ( _normal == Direct3D(1, 0, 0) )
		planeName = "XZ";

	else if ( _normal == Direct3D(0, 1, 0) )
		planeName = "XY";

	NXOpen::DatumCollection * datumList = _Part->_nxPart->Datums();
	NXOpen::DatumCollection::iterator itr;

	for ( itr = datumList->begin(); itr != datumList->end(); ++itr )
	{
		NXOpen::DatumPlane * dP = (NXOpen::DatumPlane *)* itr;
		
		string JID = dP->JournalIdentifier().GetUTF8Text();

		if ( JID.find(planeName) != -1 )	// 동일한 문자를 찾으면 index, 찾지 못하면 -1 리턴
		{
			/** Reference manager에 JID 추가 **/
			g_pRefManager->AddNewJID(_spFeature, JID, "");

			// DEBUG
			// cout << "Basis Datum_CSYS JID : " << JID << endl;
		}
	}
}