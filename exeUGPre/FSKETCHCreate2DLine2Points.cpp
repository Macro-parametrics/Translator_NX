#pragma once
#include "stdafx.h"
#include "FSKETCHCreate2DLine2Points.h"

#include <NXOpen/Line.hxx>
#include <NXOpen/LineCollection.hxx>

#include <iostream>
using namespace std;

int FSKETCHCreate2DLine2Points::_nLineCnt = 1;

FSKETCHCreate2DLine2Points::FSKETCHCreate2DLine2Points(Part* part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection* featureList, NXOpen::NXString jID, FSketch* pFSketch)
	: Feature(part, ugFeature, featureList)
{
	_fType = SKETCH_Create_2D_Line_2Points;
	_pFSketch = pFSketch;
	_jID = jID;
}

FSKETCHCreate2DLine2Points::~FSKETCHCreate2DLine2Points(void)
{
}

void FSKETCHCreate2DLine2Points::GetUGInfo()
{
	cout << "		//////////////////////////////////////"<< endl;
	cout << "		Get FSKETCHCreate2DLine2Points Info" << endl;
	NXOpen::Line* line1 = _pPart->_ugPart->Lines()->FindObject(_jID);


	double staPnt[3] = {line1->StartPoint().X, line1->StartPoint().Y, line1->StartPoint().Z};
	double endPnt[3] = {line1->EndPoint().X, line1->EndPoint().Y, line1->EndPoint().Z};
	// To get sketch infomation to tranlsate from world to local coordinate
	double sketInfo[12];
	GetFSketch()->GetSketInfo(sketInfo);
	
	Map(sketInfo,staPnt);       // XY plane(local) 값으로 ...
	Map(sketInfo,endPnt);         // XY plane(local) 값으로 ...

	// To set both start and end points
	SetStaPnt(staPnt);
	SetEndPnt(endPnt);

	cout <<	"		Start Point: (" << staPnt[0] << " , " << staPnt[1] << " , " << staPnt[2] << ")" << endl; 
	cout <<	"		End Point: (" << endPnt[0] << " , " << endPnt[1] << " , " << endPnt[2] << ")" << endl; 
	

	//----------  Set Result_Object_Name  ----------//
	char buffer[20];
	_itoa(_nLineCnt ++ , buffer, 10 );
    SetName("line2D" + (string)buffer);
}

void FSKETCHCreate2DLine2Points::ToTransCAD()
{
	//cout << "	//////////////////////////////////////"<< endl;
	cout << "	Translate FSKETCHCreate2DLine2Points to TransCAD" << endl;

	Point3D sP, eP;
	GetStaPnt(sP);
	GetEndPnt(eP);
	// Get SketchEditorPtr
	TransCAD::ISketchEditorPtr spSketchEditor = GetFSketch()->GetSketchEditorPtr();
	// Create sketch lines
	spSketchEditor->Create2DLine2Points(GetName().c_str(), sP.X(), sP.Y(), eP.X(), eP.Y() );
	
	//DEBUG
	//cout << "  " << GetName() << " : " << sP.X() << " " << sP.Y() << " " << eP.X() << " " << eP.Y() << endl;
}
