#pragma once

#include "FSketchHolePoint.h"
#include "Feature.h"
#include "stdafx.h"

#include <NXOpen/Sketch.hxx>
#include <NXOpen/NXObject.hxx>
#include <NXOpen/Features_SketchFeature.hxx>
#include <NXOpen/NXMatrix.hxx>
#include <NXOpen/SketchCollection.hxx>
#include <NXOpen/SelectNXObjectList.hxx>

#include <iostream>
using namespace std;


int FSketchHolePoint::_pntSketCnt = 1;

FSketchHolePoint::FSketchHolePoint(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList) :
	Feature(part,ugfeature, featureList)
{
	_fType = SKETCH_HOLE_POINT;
}

FSketchHolePoint::~FSketchHolePoint(void)
{
}

void FSketchHolePoint::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSketchHolePoint Info" << endl;

	NXOpen::Point3d point1;
	
	// SketchFeature 객체 생성 및 현재 Feature정보 입력
	NXOpen::Features::SketchFeature * sketchFeature(dynamic_cast<NXOpen::Features::SketchFeature *>(_ugFeature));
	_ugSketch = sketchFeature->Sketch();

	//cout << "Sketch JID: " << _ugSketch->JournalIdentifier().GetUTF8Text() <<endl;

	NXOpen::Matrix3x3 mat = _ugSketch->Orientation()->Element();
	double orientation[9]={mat.Xx,mat.Xy,mat.Xz,mat.Yx,mat.Yy,mat.Yz, mat.Zx,mat.Zy,mat.Zz};
	for (int i=0; i<9; i++)
	{
		double temp = orientation[i];
		orientation[i] = ValRnd(orientation[i]);
	}

	cout << "Hole Sketch Orientation Matrix: " << endl;
	cout << "|" << orientation[0] <<","<<orientation[1]<<","<<orientation[2]<< "|" << endl;
	cout << "|" << orientation[3] <<","<<orientation[4]<<","<<orientation[5]<< "|" << endl;
	cout << "|" << orientation[6] <<","<<orientation[7]<<","<<orientation[8]<< "|" << endl;
    
	double m00 = orientation[0];
	double m01 = orientation[1];
	double m02 = orientation[2];
	double m10 = orientation[3];
	double m11 = orientation[4];
	double m12 = orientation[5];
	double m20 = orientation[6];
	double m21 = orientation[7];
	double m22 = orientation[8];


	double det = m00*(m11*m22 - m12*m21) +
				m01*(m12*m20 - m10*m22) +
				m02*(m10*m21 - m11*m20);

	double invOrientation[9];

	// calculate inv
	invOrientation[0] =  (m11 * m22 - m12 * m21) / det;
	invOrientation[3] = -(m10 * m22 - m12 * m20) / det;
	invOrientation[6] =  (m10 * m21 - m11 * m20) / det;
	invOrientation[1] = -(m01 * m22 - m02 * m21) / det;
	invOrientation[4] =  (m00 * m22 - m02 * m20) / det;
	invOrientation[7] = -(m00 * m21 - m01 * m20) / det;
	invOrientation[2] =  (m01 * m12 - m02 * m11) / det;
	invOrientation[5] = -(m00 * m12 - m02 * m10) / det;
	invOrientation[8] =  (m00 * m11 - m01 * m10) / det;

	for (int i=0; i<9; i++)
	{
		double temp = invOrientation[i];
		if(abs(temp) == 0){
			invOrientation[i] = 0;}
	}

	_sketInfo[0] = invOrientation[0];
	_sketInfo[1] = invOrientation[3];
	_sketInfo[2] = invOrientation[6];

	_sketInfo[3] = invOrientation[1];
	_sketInfo[4] = invOrientation[4];
	_sketInfo[5] = invOrientation[7];

	_sketInfo[6] = invOrientation[2];
	_sketInfo[7] = invOrientation[5];
	_sketInfo[8] = invOrientation[8];

	cout << "Hole Sketch Plane's X direction: (" 
		<< _sketInfo[0] << " , " << _sketInfo[1] << " , " << _sketInfo[2] << ")" << endl;
	
	cout << "Hole Sketch Plane's Y direction: (" 
		<< _sketInfo[3] << " , " << _sketInfo[4] << " , " << _sketInfo[5] << ")" << endl;
	
	point1= _ugSketch->Origin();
	double _origin[3]={point1.X,point1.Y,point1.Z};
	for (int i=0; i<3; i++)
	{
		double temp = _origin[i];
		_origin[i] = ValRnd(temp);
	}
	_sketInfo[9] = _origin[0];
	_sketInfo[10] = _origin[1];
	_sketInfo[11] = _origin[2];

	cout << "Hole Sketch Plane's Origin: (" 
		<< _sketInfo[9] << " , " << _sketInfo[10] << " , " << _sketInfo[11] << ")" << endl;

	//----------  Set Result_Object_Name  ----------//
	char buffer[20];
	_itoa(_pntSketCnt ++ , buffer, 10 );
	SetName("HoleSketch" + (string)buffer);

	cout << endl;
}

void FSketchHolePoint::ToTransCAD()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "FSketchHolePoint is not translated to TransCAD (Only Used for Reference)" << endl;
	cout << endl;
}