#pragma once
#include "stdafx.h"
#include "FSKETCHCreate2DArcCenterEnds.h"

#include <NXOpen/Arc.hxx>
#include <NXOpen/ArcCollection.hxx>

#include <iostream>
#include <math.h>

using namespace std;

int FSKETCHCreate2DArcCenterEnds::_2DArcCtrEndsCnt = 1;
FSKETCHCreate2DArcCenterEnds::FSKETCHCreate2DArcCenterEnds(Part* part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection* featureList, NXOpen::NXString jID, FSketch* pFSketch)
	: Feature(part, ugFeature, featureList)
{
	_fType = SKETCH_Create_2D_Arc_CenterEnds;
	_pFSketch = pFSketch;
	_jID = jID;
}

FSKETCHCreate2DArcCenterEnds::~FSKETCHCreate2DArcCenterEnds(void)
{
}

void FSKETCHCreate2DArcCenterEnds::GetUGInfo()
{
	cout << "		//////////////////////////////////////"<< endl;
	cout << "		Get FSKETCHCreate2DArcCenterEnds Info" << endl;
	// To get curve arc coordinate infomation from a object feature Tag
	NXOpen::Arc* arc1 = _pPart->_ugPart->Arcs()->FindObject(_jID);
	//cout << arc1->StartAngle()<<","<<arc1->EndAngle()<<endl;

	double centPnt[3] = {arc1->CenterPoint().X, arc1->CenterPoint().Y, arc1->CenterPoint().Z};
	double radius = arc1->Radius();

	// To translate center point from world to local coord
	double sketInfo[12];//sketch 평면의 3차원 공간상의 orientation 정보
	GetFSketch()->GetSketInfo(sketInfo);
	Map(sketInfo,centPnt);
	ValRnd(radius);

	cout <<	"		Center Point: (" << centPnt[0] << " , " << centPnt[1] << " , " << centPnt[2] << ")" << endl; 
	
	// set center point and dR
	SetCntPnt(centPnt);
	SetRadius(radius);

	//-------------  Set sP, eP  -------------//
	_sP[0] = _cP.X() + _dR * cos(arc1->StartAngle());//
	_sP[1] = _cP.Y() + _dR * sin(arc1->StartAngle());
	_sP[2] = 0;
	_eP[0] = _cP.X() + _dR * cos(arc1->EndAngle());
	_eP[1] = _cP.Y() + _dR * sin(arc1->EndAngle());
	_eP[2] = 0;
	
	cout <<	"		Start Point: (" << _sP[0] << " , " << _sP[1] << " , " << _sP[2] << ")" << endl; 
	cout <<	"		End Point: (" << _eP[0] << " , " << _eP[1] << " , " << _eP[2] << ")" << endl; 

	
	//----------  Set Result_Object_Name  ----------//
	char buffer[20];
	_itoa( _2DArcCtrEndsCnt ++ , buffer, 10 );
	SetName("arc2DCtrEnds" + (string)buffer);
}

void FSKETCHCreate2DArcCenterEnds::ToTransCAD()
{
	//cout << "	//////////////////////////////////////"<< endl;
	cout << "	Translate FSKETCHCreate2DArcCenterEnds to TransCAD" << endl;

	// Get SketchEditorPtr
	TransCAD::ISketchEditorPtr spSketchEditor = GetFSketch()->GetSketchEditorPtr();
	// Create sketch 2D arc 3 points
	spSketchEditor->Create2DArcCenterStartEnd(GetName().c_str(), _cP[0], _cP[1], _sP[0], _sP[1], _eP[0], _eP[1]);
	
}