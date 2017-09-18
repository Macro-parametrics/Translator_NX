#pragma once
#include "stdafx.h"
#include "FSKETCHCreate2DCenterline.h"

#include <NXOpen/Line.hxx>
#include <NXOpen/LineCollection.hxx>

#include <iostream>
using namespace std;

int FSKETCHCreate2DCenterline::_nCLineCnt = 1;

FSKETCHCreate2DCenterline::FSKETCHCreate2DCenterline(Part* part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection* featureList, NXOpen::NXString jID, FSketch* pFSketch)
	: Feature(part, ugFeature, featureList)
{
	_fType = SKETCH_Create_2D_Centerline;
	_pFSketch = pFSketch;
	_jID = jID;
}

FSKETCHCreate2DCenterline::~FSKETCHCreate2DCenterline(void)
{
}

void FSKETCHCreate2DCenterline::GetUGInfo()
{	
	cout << "		//////////////////////////////////////"<< endl;
	cout << "		Get FSKETCHCreate2DCenterline Info" << endl;
	NXOpen::Line* line1 = _pPart->_ugPart->Lines()->FindObject(_jID);
	
	double staPnt[3] = {line1->StartPoint().X, line1->StartPoint().Y, line1->StartPoint().Z};
	double endPnt[3] = {line1->EndPoint().X, line1->EndPoint().Y, line1->EndPoint().Z};

	double sketInfo[12];
	GetFSketch()->GetSketInfo(sketInfo);
	
	Map(sketInfo,staPnt);       // XY plane(local) 값으로 ...
	Map(sketInfo,endPnt);         // XY plane(local) 값으로 ...

	// To set both start and end points
	SetStaPnt(staPnt);
	SetEndPnt(endPnt);

	cout << "		CLine Start Point: (" << staPnt[0] << " , " << staPnt[1] << " , " << staPnt[2] << ")" << endl;
	cout << "		CLine End Point: (" << endPnt[0] << " , " << endPnt[1] << " , " << endPnt[2] << ")" << endl;
		
	//----------  Set Result_Object_Name  ----------//
	char buffer[20];
	_itoa(_nCLineCnt ++ , buffer, 10 );
    SetName("centerline2D" + (string)buffer);
	
}

void FSKETCHCreate2DCenterline::ToTransCAD()
{
	//cout << "	//////////////////////////////////////"<< endl;
	cout << "	Translate FSKETCHCreate2DCenterline to TransCAD" << endl;

	// Get SketchEditorPtr
	TransCAD::ISketchEditorPtr spSketchEditor = GetFSketch()->GetSketchEditorPtr();
	// Create Centerline
	spSketchEditor->Create2DCenterline2Points(GetName().c_str(),_sP.X(),_sP.Y(),_eP.X(),_eP.Y());
}
