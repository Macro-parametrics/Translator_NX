#include "stdafx.h"
#include "SKETCHCreate2DCircleCenterPoint.h"

#include "Part.h"
#include "FSketch.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Arc.hxx>
#include <NXOpen\CurveCollection.hxx>
#include <NXOpen\SketchCollection.hxx>

SKETCHCreate2DCircleCenterPoint::SKETCHCreate2DCircleCenterPoint(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
	: SKETCHItem(pFSketch, spItem)
{

}

SKETCHCreate2DCircleCenterPoint::~SKETCHCreate2DCircleCenterPoint(void) {}

void SKETCHCreate2DCircleCenterPoint::GetInfo()
{
	TransCAD::IStdSketchCirclePtr spItem = GetTransCADGeometryItem();


	Point3D centerPoint_LC(spItem->CenterPoint->X, spItem->CenterPoint->Y, 0);
	
	/** Circle의 '반지름'을 맴버변수에 입력 **/
	_radius = spItem->Radius;
	_startAngle = 0.0;
	_endAngle = 360.0 * DEGRA;

	/** Local coordinate to Global coordinate **/
	double A[12];
	double WC[3];

	for (int i = 0; i < 9; ++i)
	{
		A[i] = _pFSketch->_CSYS[i];
	}
	A[9] = centerPoint_LC.X(); A[10] = centerPoint_LC.Y(); A[11] = centerPoint_LC.Z();

	Map2(A, WC);	// Definition.cpp 참조
	
	/** Circle의 중심을 TransCAD에서 맴버변수로 입력 **/
	_centerPoint.SetXYZ(WC[0], WC[1], WC[2]);

	cout << "CenterPoint -> (" << _centerPoint[0] << ", " << _centerPoint[1] << ", " << _centerPoint[2] << ")" << endl;
	cout << "Radius : " << _radius << endl;
	cout << "Sketch origin -> (" << _pFSketch->_CSYS[6] << " " << _pFSketch->_CSYS[7] << " " << _pFSketch->_CSYS[8] << ")" << endl;
	//cout << "Sketch normal : " << _pFSketch->_sketNormal.X() << ", " << _pFSketch->_sketNormal.Y() << ", " << _pFSketch->_sketNormal.Z() << endl;
}

void SKETCHCreate2DCircleCenterPoint::ToUG()
{
	try
	{
		using namespace NXOpen;

		/** TC type to NXOpen type : Point3D -> Point3d **/
		Point3d circleCenter(_centerPoint.X(), _centerPoint.Y(), _centerPoint.Z());
	
		Vector3d xDir(_pFSketch->_CSYS[0], _pFSketch->_CSYS[1], _pFSketch->_CSYS[2]);
		Vector3d yDir(_pFSketch->_CSYS[3], _pFSketch->_CSYS[4], _pFSketch->_CSYS[5]);
	
		NXOpen::Arc * pCircle;
		pCircle = _pFSketch->_Part->_nxPart->Curves()->CreateArc(circleCenter, xDir, yDir, _radius, _startAngle, _endAngle);

		string sketJID = g_pRefManager->GetJIDByTCName(_pFSketch->GetFeatureName());
		string sketchName = _pFSketch->GetSketchName();

		Sketch * targetSketch;
		targetSketch = _pFSketch->_Part->_nxPart->Sketches()->FindObject(sketchName);

		targetSketch->AddGeometry(pCircle);
	
		// DEBUG cout << "Target sketch name : " << sketchName << endl;
		// DEBUG cout << "Target sketch JID : " << sketJID << endl;
	}

	catch ( const NXOpen::NXException &ex )
	{
		cout << "   Error location [ Create circle feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}