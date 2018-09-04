#include "stdafx.h"
#include "SKETCHCreate2DCenterline.h"

#include <iostream>

/** UG Post's header files **/
#include "Part.h"
#include "FSketch.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Line.hxx>
#include <NXOpen\SketchCollection.hxx>
#include <NXOpen\CurveCollection.hxx>
#include <NXOpen\SelectNXObjectList.hxx>


using namespace std;

SKETCHCreate2DCenterline::SKETCHCreate2DCenterline(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
	: SKETCHItem(pFSketch, spItem)
{
}

SKETCHCreate2DCenterline::~SKETCHCreate2DCenterline(void)
{
}

void SKETCHCreate2DCenterline::GetInfo()
{

	TransCAD::IStdSketchCenterlinePtr spItem = GetTransCADGeometryItem();

	_TransCADName = spItem->Name;

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

	//string sketchName = _pFSketch->GetFeatureName();

	cout << "Centerline 시작점 : " << _startPoint.X() << ", " << _startPoint.Y() << ", " << _startPoint.Z() << endl;
	cout << "Centerline 끝  점 : " << _endPoint.X() << ", " << _endPoint.Y() << ", " << _endPoint.Z() << endl;
	
}

void SKETCHCreate2DCenterline::ToUG()
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
	
		cout << pLine->JournalIdentifier().GetUTF8Text() << endl;

		/** TransCAD name을 이용하여 UG sketch JID를 찾는다. **/
		string sketJID = g_pRefManager->GetJIDByTCName(_pFSketch->GetFeatureName());
		string sketchName = _pFSketch->GetSketchName();

		Sketch * targetSketch;
		targetSketch = _pFSketch->_Part->_nxPart->Sketches()->FindObject(sketchName);

		targetSketch->AddGeometry(pLine);

		// DEBUG cout << "Target sketch name : " << sketchName << endl;
		// DEBUG cout << "Target sketch JID : " << sketJID << endl;

		ConvertToFromReferenceBuilder *builder;
		builder = _pFSketch->_Part->_nxPart->Sketches()->CreateConvertToFromReferenceBuilder();

		SelectNXObjectList * selectReferenceLine;
		selectReferenceLine = builder->InputObjects();

		selectReferenceLine->Add(pLine);

		builder->SetOutputState(ConvertToFromReferenceBuilder::OutputTypeReference);

		NXObject * centerLine;
		centerLine = builder->Commit();
	
		builder->Destroy();

		_centerLineJID = centerLine->JournalIdentifier().GetUTF8Text();
		/** Extrude feature JID 저장 **/
		//g_pRefManager->AddNewJID(_pFSketch->_Part->GetFeatureByName(_TransCADName), centerLine->JournalIdentifier().GetUTF8Text());	
		cout << "Center line JID : " << centerLine->JournalIdentifier().GetUTF8Text() << endl;
	}

	catch ( const NXOpen::NXException &ex )
	{
		cout << "   Error location [ Centerline feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
	
}