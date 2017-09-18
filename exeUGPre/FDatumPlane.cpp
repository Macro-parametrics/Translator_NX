#include "FDatumPlane.h"
#include "stdafx.h"
#include <NXOpen/Features_DatumPlaneBuilder.hxx>
#include <NXOpen/DatumPlane.hxx>
#include <NXOpen/Features_DatumPlaneFeature.hxx>
#include <NXOpen/Plane.hxx>
#include <NXOpen/NXException.hxx>

#include <iostream>
using namespace std;

int FDatumPlane::_nDtPlaneCnt = 1;

FDatumPlane::FDatumPlane(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList) :
	Feature(part,ugfeature, featureList)
{
	SetFType(FType::DATUM_PLANE);
	isReverse = FALSE;
}

FDatumPlane::~FDatumPlane(void)
{
}


//Public Functions
void FDatumPlane::SetOrigin(const Point3D & origin)
{
	_origin	= origin;
}

//void FDatumPlane::SetNormal(const Direct3D & normal)
//{
//	_normal = normal;
//}

void FDatumPlane::SetXDir(const Direct3D & xDir)
{
	_xDir = xDir;
}

void FDatumPlane::SetYDir(const Direct3D & yDir)
{
	_yDir = yDir;
}


Point3D FDatumPlane::GetOrigin()
{
	return _origin;
}

//Direct3D FDatumPlane::GetNormal()
//{
//	return _normal;
//}

Direct3D FDatumPlane::GetXDir()
{
	return _xDir;
}

Direct3D FDatumPlane::GetYDir()
{
	return _yDir;
}

//get UG information
void FDatumPlane::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FDatumPlane Info" << endl;
	
	double origin[3];
	double normal[3];
	double xDir[3];
	double yDir[3];
	NXOpen::Point3d point1;
	NXOpen::Point3d point2;
	NXOpen::Point3d point3;
	NXOpen::Point3d point4;

	vector<NXOpen::Features::Feature* > parentFeat = _ugFeature->GetParents();
	cout << "Parent Features: " << endl;

	for(int i = 0; i<parentFeat.size(); i++){
		offsetRefPlaneJID = parentFeat.at(i)->JournalIdentifier();
		cout << "	" << parentFeat.at(i)->JournalIdentifier().GetUTF8Text() << endl;
	}

	NXOpen::Features::DatumPlaneBuilder * datumPlaneBuilder = _featureList->CreateDatumPlaneBuilder(_ugFeature);
	NXOpen::Plane * plane = datumPlaneBuilder->GetPlane();

	bool fd = plane->Flip();
	if(plane->Flip()){
		isReverse = TRUE;
		cout << "Offset Flip Status: TRUE" << endl;
	}else{
		cout << "Offset Flip Status: FALSE" << endl;
	}
	
	string offset = plane->Expression()->RightHandSide().GetUTF8Text();
	_offset = stod(offset);
	cout << "Offset Value: " << _offset << endl;
	
	//----  Get the Origin(Picking Point) , Normal Vector of the Datum_Plane   ----
	
	// DatumPlaneFeature 객체 생성 및 현재 Feature정보 입력
	NXOpen::Features::DatumPlaneFeature * datumPlaneFeature(dynamic_cast<NXOpen::Features::DatumPlaneFeature *>(_ugFeature));
	NXOpen::DatumPlane * _datumPlane = datumPlaneFeature->DatumPlane();
	
	//cout << "Datum Plane JID: " << _datumPlane->JournalIdentifier().GetUTF8Text() <<endl;

	origin[0] = _datumPlane->Origin().X;
	origin[1] = _datumPlane->Origin().Y;
	origin[2] = _datumPlane->Origin().Z;
	cout << "Origin: (" << origin[0] << " , " << origin[1] << " , " << origin[2] << ")" << endl;

	//normal[0] = _datumPlane->Normal().X;
	//normal[1] = _datumPlane->Normal().Y;
	//normal[2] = _datumPlane->Normal().Z;

	_datumPlane->GetCornerPoints(&point1, &point2, &point3, &point4);

	xDir[0] = point2.X-point1.X;
	xDir[1] = point2.Y-point1.Y;
	xDir[2] = point2.Z-point1.Z;
	double n  =sqrt((xDir[0] * xDir[0]) + (xDir[1] * xDir[1]) + (xDir[2] * xDir[2]));
	xDir[0] = xDir[0]/n;
	xDir[1] = xDir[1]/n;
	xDir[2] = xDir[2]/n;

	cout << "X Direction: (" << xDir[0] << " , " << xDir[1] << " , " << xDir[2] << ")" << endl;

	yDir[0] = point4.X-point1.X;
	yDir[1] = point4.Y-point1.Y;
	yDir[2] = point4.Z-point1.Z;
	n  =sqrt((yDir[0] * yDir[0]) + (yDir[1] * yDir[1]) + (yDir[2] * yDir[2]));
	yDir[0] = yDir[0]/n;
	yDir[1] = yDir[1]/n;
	yDir[2] = yDir[2]/n;
		
	cout << "Y Direction: (" << yDir[0] << " , " << yDir[1] << " , " << yDir[2] << ")" << endl;

	/*origin[0] = ValRnd((point4.X+point2.X)/2);
	origin[1] = ValRnd((point4.Y+point2.Y)/2);
	origin[2] = ValRnd((point4.Z+point2.Z)/2);*/

	//cout<<"Point1 is " <<point1.X<<" , "<<point1.Y<<" , "<<point1.Z<<endl;
	//cout<<"Point2 is " <<point2.X<<" , "<<point2.Y<<" , "<<point2.Z<<endl;
	//cout<<"Point3 is " <<point3.X<<" , "<<point3.Y<<" , "<<point3.Z<<endl;
	//cout<<"Point4 is " <<point4.X<<" , "<<point4.Y<<" , "<<point4.Z<<endl;

	SetOrigin(origin);
	//SetNormal(normal);
	SetXDir(xDir);
	SetYDir(yDir);

	//-------------------  Set Result_Object_Name  -------------------//
	char buffer[20];
	_itoa( _nDtPlaneCnt++, buffer, 10 );
	SetName("DatumPlane" + (string)buffer);

	cout << endl;
}


void FDatumPlane::ToTransCAD()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Start translate FDatumPlane to TransCAD" << endl;
	/*cout<<"This is plane "<<GetName()<<endl;
	cout<<"Origin is "<<_origin.X()<<" , "<<_origin.Y()<<" , "<<_origin.Z()<<endl;
	cout<<"X dir is " <<_xDir.X()<<" , "<<_xDir.Y()<<" , "<<_xDir.Z()<<endl;
	cout<<"Y dir is " <<_yDir.X()<<" , "<<_yDir.Y()<<" , "<<_yDir.Z()<<endl;*/
	
	//double temp[9] = {_origin.X(), _origin.Y(), _origin.Z(), _xDir.X(), _xDir.Y(), _xDir.Z(), _yDir.X(), _yDir.Y(), _yDir.Z()};
	/*TransCAD::IStdDefaultDatumPlaneFeaturePtr spPlane = GetPart()->_spFeatures->AddNewDatumPlaneOffsetFeature2(GetName().c_str(), 
		_origin.X(), _origin.Y(), _origin.Z(), _xDir.X(), _xDir.Y(), _xDir.Z(), _yDir.X(), _yDir.Y(), _yDir.Z(), FALSE); */
	TransCAD::IReferencePtr spSelectedPlane;

	if(strstr(offsetRefPlaneJID.GetUTF8Text(),"DATUM_PLANE")){
		cout << "Selected Offset Plane is Datum Plane" << endl;
		bstr_t datumPlaneName = GetPart()->Get_pFeatureByJID(offsetRefPlaneJID)->GetName().c_str();
		spSelectedPlane = GetPart()->_spPart->SelectObjectByName(datumPlaneName);
	}else if(strstr(offsetRefPlaneJID.GetUTF8Text(),"DATUM_CSYS")){
		cout << "Selected Offset Plane is Reference Plane" << endl;
		if(abs(_xDir.X()) > 0.5){
			if(abs(_yDir.Y()) > 0.5){
				spSelectedPlane = GetPart()->_spPart->SelectObjectByName("XYPlane");
			}else{
				spSelectedPlane = GetPart()->_spPart->SelectObjectByName("ZXPlane");
			}
		}else{
			spSelectedPlane = GetPart()->_spPart->SelectObjectByName("YZPlane");
		}
	}else{
		TransCAD::IStdDefaultDatumPlaneFeaturePtr spPlane = GetPart()->_spFeatures->AddNewDatumPlaneOffsetFeature2(GetName().c_str(), 
			_origin.X(), _origin.Y(), _origin.Z(), _xDir.X(), _xDir.Y(), _xDir.Z(), _yDir.X(), _yDir.Y(), _yDir.Z(), FALSE); 
		cout << "Selected Offset Plane's Reference is Body Face (Not yet treated as offset plane. Cannot Get Body Face Info from NX part), just a ref plane with origin and x/y direction)" << endl;
		cout << "FDatumPlane: " << GetName().c_str() << " Translated" << endl << endl;
		return;
	}
	cout << "Selected Offset Reference Plane in TransCAD: " << spSelectedPlane->GetReferenceeName() << endl;
	
	TransCAD::IStdDefaultDatumPlaneFeaturePtr spPlane = GetPart()->_spFeatures->AddNewDatumPlaneOffsetFeature(
		GetName().c_str(), spSelectedPlane, _offset, isReverse);
	cout << "FDatumPlane: " << GetName().c_str() << " Translated" << endl << endl;
}
