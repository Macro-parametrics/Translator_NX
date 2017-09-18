#pragma once
#include "stdafx.h"
#include "FSKETCHCreate2DCircleCenterPoint.h"

#include <NXOpen/Arc.hxx>
#include <NXOpen/ArcCollection.hxx>

#include <iostream>
using namespace std;

int FSKETCHCreate2DCircleCenterPoint::_circle2DCPCnt = 1;

FSKETCHCreate2DCircleCenterPoint::FSKETCHCreate2DCircleCenterPoint(Part* part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection* featureList, NXOpen::NXString jID, FSketch* pFSketch)
	: Feature(part, ugFeature, featureList)
{
	_pFSketch = pFSketch;
	_jID = jID;
}

FSKETCHCreate2DCircleCenterPoint::~FSKETCHCreate2DCircleCenterPoint(void)
{
}

void FSKETCHCreate2DCircleCenterPoint::GetUGInfo()
{
	cout << "		//////////////////////////////////////"<< endl;
	cout << "		Get FSKETCHCreate2DCircleCenterPoint Info" << endl;
	// To get curve arc coordinate infomation from a object feature Tag
	NXOpen::Arc* arc1 = _pPart->_ugPart->Arcs()->FindObject(_jID);
	double centPnt[3] = {arc1->CenterPoint().X, arc1->CenterPoint().Y, arc1->CenterPoint().Z};
	double radius = arc1->Radius();

	// To translate center point from world to local coord
	double sketInfo[12];
	GetFSketch()->GetSketInfo(sketInfo);
	Map(sketInfo,centPnt);
	ValRnd(radius);
	centPnt[2] = 0;
	// set center point and dR
	SetCntPnt(centPnt);
	SetRadius(radius);
	
	cout <<	"		Center Point: (" << centPnt[0] << " , " << centPnt[1] << " , " << centPnt[2] << ")" << endl; 
	cout << "		Radius: " << radius << endl;

	//----------  Set Result_Object_Name  ----------//
	char buffer[20];
	_itoa( _circle2DCPCnt ++ , buffer, 10 );
    SetName("circle2DCP" + (string)buffer);
}

void FSKETCHCreate2DCircleCenterPoint::ToTransCAD()
{
	//cout << "	//////////////////////////////////////"<< endl;
	cout << "	Translate FSKETCHCreate2DCircleCenterPoint to TransCAD" << endl;

	// Get SketchEditorPtr
	TransCAD::ISketchEditorPtr spSketchEditor = GetFSketch()->GetSketchEditorPtr();
	// Create sketch lines
	spSketchEditor->Create2DCircleCenterPoint(GetName().c_str(), _cP.X(), _cP.Y(), GetRadius());
	
	//DEBUG
	//cout << "  Center Point " << _cP[0] << " " << _cP[1] << " " << _cP[2] << endl;
}