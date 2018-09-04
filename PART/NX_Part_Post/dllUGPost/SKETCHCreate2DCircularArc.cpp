#include "stdafx.h"
#include "SKETCHCreate2DCircularArc.h"

#include "Part.h"
#include "FSketch.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Arc.hxx>
#include <NXOpen\CurveCollection.hxx>
#include <NXOpen\SketchCollection.hxx>

using namespace std;

SKETCHCreate2DCircularArc::SKETCHCreate2DCircularArc(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
	: SKETCHItem(pFSketch, spItem)
{

}

SKETCHCreate2DCircularArc::~SKETCHCreate2DCircularArc(void) {}

void SKETCHCreate2DCircularArc::GetInfo()
{
	TransCAD::IStdSketchCircularArcPtr spItem = GetTransCADGeometryItem();
	
	Point3D startPoint_LC(spItem->StartPoint->X, spItem->StartPoint->Y, 0);
	Point3D endPoint_LC(spItem->EndPoint->X, spItem->EndPoint->Y, 0);
	Point3D centerPoint_LC(spItem->CenterPoint->X, spItem->CenterPoint->Y, 0);

	/** Local coordinate to Global coordinate **/
	double A[12];
	double startPoint_WC[3];
	double endPoint_WC[3];
	double centerPoint_WC[3];

	for (int i = 0; i < 9; ++i)
	{
		A[i] = _pFSketch->_CSYS[i];
	}

	A[9] = startPoint_LC.X(); A[10] = startPoint_LC.Y(); A[11] = startPoint_LC.Z();
	Map2(A, startPoint_WC);

	A[9] = endPoint_LC.X(); A[10] = endPoint_LC.Y(); A[11] = endPoint_LC.Z();
	Map2(A, endPoint_WC);

	A[9] = centerPoint_LC.X(); A[10] = centerPoint_LC.Y(); A[11] = centerPoint_LC.Z();
	Map2(A, centerPoint_WC);


	/** Arc의 SP, EP, CP를 맴버변수로 입력 **/
	_startPoint.SetXYZ(startPoint_WC[0], startPoint_WC[1], startPoint_WC[2]);
	_endPoint.SetXYZ(endPoint_WC[0], endPoint_WC[1], endPoint_WC[2]);
	_centerPoint.SetXYZ(centerPoint_WC[0], centerPoint_WC[1], centerPoint_WC[2]);

	TransCADName = spItem->Name;

	_startAngle = spItem->StartAngle;
	_endAngle = spItem->EndAngle;
	_radius = spItem->Radius;

	cout << "CenterPoint -> (" << _centerPoint[0] << ", " << _centerPoint[1] << ", " << _centerPoint[2] << ")" << endl;
	cout << "StartPoint -> (" << _startPoint[0] << ", " << _startPoint[1] << ", " << _startPoint[2] << ")" << endl;
	cout << "EndPoint -> (" << _endPoint[0] << ", " << _endPoint[1] << ", " << _endPoint[2] << ")" << endl;
}

void SKETCHCreate2DCircularArc::ToUG()
{
	try
	{
		using namespace NXOpen;

		NXOpen::Arc * arc;

		/*Point3D sketOrigin(_pFSketch->_CSYS[6], _pFSketch->_CSYS[7], _pFSketch->_CSYS[8]);
		Point3D CP, SP, EP;
		CP = sketOrigin.operator+(_centerPoint);
		SP = sketOrigin.operator+(_startPoint);
		EP = sketOrigin.operator+(_endPoint);*/

		/** TC type to NXOpen type : Point3D -> Point3d **/
		Point3d _ugCP;
		Point3d _ugSP;
		Point3d _ugEP;
	
		_ugCP.X = _centerPoint.X();	_ugCP.Y = _centerPoint.Y();	_ugCP.Z = _centerPoint.Z();
		_ugSP.X = _startPoint.X();	_ugSP.Y = _startPoint.Y();	_ugSP.Z = _startPoint.Z();
		_ugEP.X = _endPoint.X();	_ugEP.Y = _endPoint.Y();	_ugEP.Z = _endPoint.Z();

		string sketJID = g_pRefManager->GetJIDByTCName(_pFSketch->GetFeatureName());
		string sketchName = _pFSketch->GetSketchName();

		Sketch * targetSketch;
		targetSketch = _pFSketch->_Part->_nxPart->Sketches()->FindObject(sketchName);

		NXMatrix * nxMTX;
		nxMTX = targetSketch->Orientation();

		arc = _pFSketch->_Part->_nxPart->Curves()->CreateArc(_ugCP, nxMTX, _radius, _startAngle, _endAngle);

		targetSketch->AddGeometry(arc);
	}

	catch ( const NXOpen::NXException &ex )
	{
		cout << "   Error location [ Create arc feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}