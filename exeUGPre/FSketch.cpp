#pragma once
#define _USE_MATH_DEFINES
#include "FSketch.h"
#include "Feature.h"
#include "stdafx.h"
#include "FSKETCHCreate2DLIne2Points.h"
#include "FSKETCHCreate2DCircleCenterPoint.h"
#include "FSKETCHCreate2DArcCenterEnds.h"
#include "FSKETCHCreate2DCenterline.h"

#include <NXOpen/Sketch.hxx>
#include <NXOpen/NXObject.hxx>
#include <NXOpen/Features_SketchFeature.hxx>
#include <NXOpen/Features_DatumPlaneFeature.hxx>
#include <NXOpen/DatumPlane.hxx>
#include <NXOpen/Plane.hxx>
#include <NXOpen/NXMatrix.hxx>
#include <NXOpen/Point.hxx>
#include <NXOpen/Curve.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/Line.hxx>
#include <NXOpen/LineCollection.hxx>
#include <NXOpen/Arc.hxx>
#include <NXOpen/ArcCollection.hxx>
#include <NXOpen/SketchCollection.hxx>
#include <NXOpen/ModelingViewCollection.hxx>
#include <NXOpen/SelectNXObjectList.hxx>

#include <math.h>
#include <iostream>
using namespace std;


int FSketch::_nSketCnt = 1;

FSketch::FSketch(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList) :
	Feature(part,ugfeature, featureList)
{
	SetFType(FType::SKETCH);
}

FSketch::~FSketch(void)
{
	std::vector<Feature*>::iterator i = _sketObjList.begin();
	while ( i != _sketObjList.end() )	{delete *i; i++;}
	_sketObjList.clear();
}

Feature * FSketch::GetCLine()
{
	for (size_t i=0; i < GetSketObjSize() ; i++)
	{
		Feature * pFeature = GetSketObj((int)i);
		if ( pFeature->GetFType() == SKETCH_Create_2D_Centerline )
			return pFeature;
	}
	return 0;
}

void FSketch::SetCLine(Feature* pFeature)
{
	if (GetCLine()) return;
	_sketObjList.insert(_sketObjList.begin(), pFeature);
}


void FSketch::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSketch Info" << endl;

	vector<NXOpen::Features::Feature* > parentFeat = _ugFeature->GetParents();
	
	cout << "Parent Features: " << endl;

	for(int i = 0; i<parentFeat.size(); i++){
		cout << "	" << parentFeat.at(i)->JournalIdentifier().GetUTF8Text() << endl;
		for(int j=0; j<parentFeat.at(i)->GetParents().size(); j++){
			sketRefPlaneJID = parentFeat.at(i)->GetParents().at(j)->JournalIdentifier();
			cout << "		" << parentFeat.at(i)->GetParents().at(j)->JournalIdentifier().GetUTF8Text() << endl;
		}
	}

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

	cout << "Sketch Orientation Matrix: " << endl;
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

	cout << "Sketch Plane's X direction: (" 
		<< _sketInfo[0] << " , " << _sketInfo[1] << " , " << _sketInfo[2] << ")" << endl;
	
	cout << "Sketch Plane's Y direction: (" 
		<< _sketInfo[3] << " , " << _sketInfo[4] << " , " << _sketInfo[5] << ")" << endl;
	
	NXOpen::Point3d point1;
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

	cout << "Sketch Plane's Origin: (" 
		<< _sketInfo[9] << " , " << _sketInfo[10] << " , " << _sketInfo[11] << ")" << endl;

	vector<NXOpen::NXObject *> geoms = _ugSketch->GetAllGeometry();

	cout << "	Start Sketch Objects Creation" << endl;
	cout << "	Num of Sketch Objects: " << geoms.size() << endl;
 	for(int i = 0; i<geoms.size(); i++){
		Feature * pSketchObjectFeature = 0;
		const char * sketObjName = geoms.at(i)->Name().GetUTF8Text();
		if(!_strnicmp(sketObjName, "Line", 1)){
			NXOpen::Line* line = _pPart->_ugPart->Lines()->FindObject(geoms.at(i)->JournalIdentifier());
			
			//NXOpen::ConvertToFromReferenceBuilder * refLineCheckBuilder = _pPart->_ugPart->Sketches()->CreateConvertToFromReferenceBuilder();
			//refLineCheckBuilder->AddProjectFeatureCurves(line);
			if(!line->IsReference()){
				//NXOpen::Line* line1 = _pPart->_ugPart->Lines()->FindObject(geoms.at(i)->JournalIdentifier());
				//cout << line1->StartPoint().X <<","<<line1->StartPoint().Y<<","<<line1->StartPoint().Z<<endl;
				pSketchObjectFeature = new FSKETCHCreate2DLine2Points(GetPart(), _ugFeature, _featureList, geoms.at(i)->JournalIdentifier(), this);			
				cout << "		" << geoms.at(i)->Name().GetUTF8Text() << " is created as FSKETCHCreate2DLine2Points" << endl;
			}else{
				pSketchObjectFeature = new FSKETCHCreate2DCenterline(GetPart(), _ugFeature, _featureList, geoms.at(i)->JournalIdentifier(), this);		
				cout << "		" << geoms.at(i)->Name().GetUTF8Text() << " is created as FSKETCHCreate2DCenterline" << endl;
			}
			//refLineCheckBuilder->Destroy();
		}
		else if(!_strnicmp(sketObjName, "Arc", 1)){
			NXOpen::Arc* _arc = _pPart->_ugPart->Arcs()->FindObject(geoms.at(i)->JournalIdentifier());
			if ( (_arc->EndAngle() - _arc->StartAngle()) >= (2* M_PI) )
			{
				pSketchObjectFeature = new FSKETCHCreate2DCircleCenterPoint(GetPart(), _ugFeature, _featureList, geoms.at(i)->JournalIdentifier(), this);
				cout << "		" << geoms.at(i)->Name().GetUTF8Text() << " is created as FSKETCHCreate2DCircleCenterPoint" << endl;
			}else{
				pSketchObjectFeature = new FSKETCHCreate2DArcCenterEnds(GetPart(), _ugFeature, _featureList, geoms.at(i)->JournalIdentifier(), this);
				cout << "		" << geoms.at(i)->Name().GetUTF8Text() << " is created as FSKETCHCreate2DArcCenterEnds" << endl;
			}
			//NXOpen::Arc* arc1 = _pPart->_ugPart->Arcs()->FindObject(geoms.at(i)->JournalIdentifier());
			//cout << arc1->CenterPoint().X<<","<<arc1->CenterPoint().Y<<","<<arc1->CenterPoint().Z<<endl;
			//pSketchObjectFeature = new FSKETCHCreate2DArcCenterEnds(GetPart(), geoms.at(i)->JournalIdentifier(), this);
			//NXOpen::Line* line1 = _pPart->_ugPart->Lines()->FindObject(geom3.at(i)->JournalIdentifier());
			//cout << line1->StartPoint().X <<","<<line1->StartPoint().Y<<","<<line1->StartPoint().Z<<endl;
		}else{
			cout << "		" << geoms.at(i)->Name().GetUTF8Text() << " is unsupported sketch object" <<endl;
		}
		if(pSketchObjectFeature) _sketObjList.push_back(pSketchObjectFeature);
	}
	//NXOpen::Curve* curve1(dynamic_cast<NXOpen::Curve*>(_sketch->FindObject(geom2[0]->JournalIdentifier())));
	//cout << curve1->JournalIdentifier().GetUTF8Text()<<endl;
	cout << "	Loop Sketch Objects and GetInfo" << endl;
	//-------  Loop to translate sketch object features in list  --------//
	for ( size_t i=0 ; i < GetSketObjSize(); ++i )
	{
		GetSketObj((unsigned int)i)->GetUGInfo();
		//DEBUG
		//cout << "  " << (unsigned int)i << " : " << GetSketObj((unsigned int)i)->GetName() << " translated" << endl;
	}
	cout << "Sketch Objects' GetInfo Finished" << endl;
	//----------  Set Result_Object_Name  ----------//
	char buffer[20];
	_itoa(_nSketCnt ++ , buffer, 10 );
	SetName("Sketch" + (string)buffer);

	cout << endl;

	//cout<<"Sketch Origin: " <<_origin[0]<<" , "<<_origin[1]<<" , "<<_origin[2]<<endl;
}

TransCAD::ISketchEditorPtr FSketch::GetSketchEditorPtr()
{
	return _spSketEditor;
}

void FSketch::ToTransCAD()
{
	//static int k = 0;
	//k++;
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FSketch to TransCAD" << endl;

	Point3D origin = GetOrigin();
	Direct3D xdir = GetX();
	Direct3D ydir = GetY();
	//Direct3D zdir = GetZ();
	////cout << "    " << origin.X() << " " << origin.Y() << " " << origin.Z() << endl;
	////cout << "    " << xdir.X() << " " << xdir.Y() << " " << xdir.Z() << endl;
	////cout << "    " << ydir.X() << " " << ydir.Y() << " " << ydir.Z() << endl;
	////cout << "    " << zdir.X() << " " << zdir.Y() << " " << zdir.Z() << endl;

	// Select Reference Plane for sketch
	////if (k==4) spSelectedPlane = GetPart()->_spPart->SelectObjectByName("proExtrude0,sketch0,line2D2,0,0,0,ExtrudeFeature:0,0:0;0");

	TransCAD::IReferencePtr spSelectedPlane;

	if(strstr(sketRefPlaneJID.GetUTF8Text(),"DATUM_PLANE")){
		cout << "Selected Sketch Plane is Datum Plane" << endl;
		bstr_t datumPlaneName = GetPart()->Get_pFeatureByJID(sketRefPlaneJID)->GetName().c_str();
		spSelectedPlane = GetPart()->_spPart->SelectObjectByName(datumPlaneName);

	}else if(strstr(sketRefPlaneJID.GetUTF8Text(),"DATUM_CSYS")){
		cout << "Selected Sketch Plane is Reference Plane" << endl;
		if(abs(xdir.X()) == 1){
			if(abs(ydir.Y()) == 1){
				spSelectedPlane = GetPart()->_spPart->SelectObjectByName("XYPlane");
			}else{
				spSelectedPlane = GetPart()->_spPart->SelectObjectByName("ZXPlane");
			}
		}else{
			spSelectedPlane = GetPart()->_spPart->SelectObjectByName("YZPlane");
		}

	}else{
		cout << "Selected Sketch Plane is Body Face" << endl;
		spSelectedPlane = GetPart()->_spPart->SelectPlaneByAxis(origin.X(), origin.Y(), origin.Z(), 
															xdir.X(), xdir.Y(), xdir.Z(), ydir.X(), ydir.Y(), ydir.Z());
	}
	
	cout << "Selected Sketch Reference Plane in TransCAD: " << spSelectedPlane->GetReferenceeName() << endl;
	
	// Add new StdSketchFeature to TransCAD
 	TransCAD::IStdSketchFeaturePtr spSketch = GetPart()->_spFeatures->AddNewSketchFeature(GetName().c_str(), spSelectedPlane);

	//double coord[9] = {origin.X(), origin.Y(), origin.Z(), xdir.X(), xdir.Y(), xdir.Z(), ydir.X(), ydir.Y(), ydir.Z()};

	// Set Coordinate system of Sketch
	//spSketch->SetCoordinateSystem(coord[0], coord[1], coord[2], coord[3], coord[4], coord[5], coord[6], coord[7], coord[8]);
	spSketch->SetCoordinateSystem(origin.X(), origin.Y(), origin.Z(), xdir.X(), xdir.Y(), xdir.Z(), ydir.X(), ydir.Y(), ydir.Z());

	// TransCAD Sketch Open
	_spSketEditor = spSketch->OpenEditorEx(VARIANT_FALSE);
		

	//---------- Sketch Objects : ToTransCAD() ----------//
	for ( size_t i=0 ; i < GetSketObjSize() ; ++i )
		GetSketObj((unsigned int)i)->ToTransCAD();
	//// TransCAD Sketch Close
	_spSketEditor->Close();
	
	cout << "FSketch: " << GetName().c_str() << " Translated" << endl << endl;
}
