#include "stdafx.h"
#include "SKETCHCreate2DLine2Points.h"

/** UG Post's header files **/
#include "Part.h"
#include "Feature.h"
#include "FSketch.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Line.hxx>
#include <NXOpen\CurveCollection.hxx>
#include <NXOpen\SketchCollection.hxx>

#include <NXOpen\Features_SketchFeature.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>


using namespace std;

SKETCHCreate2DLine2Points::SKETCHCreate2DLine2Points(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
	: SKETCHItem(pFSketch, spItem)
{

}

SKETCHCreate2DLine2Points::~SKETCHCreate2DLine2Points(void) {}

void SKETCHCreate2DLine2Points::GetInfo()
{
	TransCAD::IStdSketchLinePtr spItem = GetTransCADGeometryItem();

	Point3D startPoint_LC(Point3D(spItem->StartPoint->X, spItem->StartPoint->Y, 0));
	Point3D endPoint_LC(Point3D(spItem->EndPoint->X, spItem->EndPoint->Y, 0));

	/** Local coordinate to Global coordinate **/
	double A[12];
	double startPoint_WC[3];
	double endPoint_WC[3];

	for (int i = 0; i < 9; ++i)
	{
		A[i] = _pFSketch->_CSYS[i];
	}

	A[9] = startPoint_LC.X(); A[10] = startPoint_LC.Y(); A[11] = startPoint_LC.Z();
	Map2(A, startPoint_WC);	// Definition.cpp 참조

	A[9] = endPoint_LC.X(); A[10] = endPoint_LC.Y(); A[11] = endPoint_LC.Z();
	Map2(A, endPoint_WC);

	/** Line의 Global좌표를 맴버변수에 담기 **/
	_startPoint.SetXYZ(startPoint_WC[0], startPoint_WC[1], startPoint_WC[2]);
	_endPoint.SetXYZ(endPoint_WC[0], endPoint_WC[1], endPoint_WC[2]);

	// cout << "Start point -> (" << _startPoint[0] << ", " << _startPoint[1] << ", " << _startPoint[2] << ")  &  ";
	// cout << "End point -> (" << _endPoint[0] << ", " << _endPoint[1] << ", " << _endPoint[2] << ")  &  ";
	// cout << "Sketch origin -> (" << _pFSketch->_CSYS[6] << " " << _pFSketch->_CSYS[7] << " " << _pFSketch->_CSYS[8] << ")" << endl;
}

void SKETCHCreate2DLine2Points::ToUG()
{
	try
	{
		using namespace NXOpen;

		Point3d _ugSP;
		Point3d _ugEP;

		/** TC type to NXOpen type : Point3D -> Point3d **/
		_ugSP.X = _startPoint.X();	_ugSP.Y = _startPoint.Y();	_ugSP.Z = _startPoint.Z();
		_ugEP.X = _endPoint.X();	_ugEP.Y = _endPoint.Y();	_ugEP.Z = _endPoint.Z();
		

		Line * pLine;
		pLine = _pFSketch->_Part->_nxPart->Curves()->CreateLine(_ugSP, _ugEP);
	
		/** TransCAD name을 이용하여 UG sketch JID를 찾는다. **/
		string sketJID = g_pRefManager->GetJIDByTCName(_pFSketch->GetFeatureName());
		string sketchName = _pFSketch->GetSketchName();

		Sketch * targetSketch;
		targetSketch = _pFSketch->_Part->_nxPart->Sketches()->FindObject(sketchName);

		targetSketch->AddGeometry(pLine);

		// DEBUG cout << "Target sketch name : " << sketchName << endl;
		// DEBUG cout << "Target sketch JID : " << sketJID << endl;		
	}

	catch ( const NXOpen::NXException &ex )
	{
		cout << "   Error location [ Create line feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
	
}
