#include "stdafx.h"
#include "FSOLIDCreateCountersunk.h"
#include "FSketchHolePoint.h"

#include <NXOpen/Sketch.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/Features_HoleFeatureBuilder.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_Hole.hxx>
#include <NXOpen/Features_HolePackage.hxx>
#include <NXOpen/Features_HolePackageBuilder.hxx>
#include <NXOpen/PointCollection.hxx>
#include <NXOpen/Point.hxx>

#include "Part.h"

#include <iostream>
using namespace std;

int FSOLIDCreateCountersunk::_cntsnkCnt = 1;

FSOLIDCreateCountersunk::FSOLIDCreateCountersunk(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList)
	: Feature(part,ugfeature, featureList)
{
	_fType = SOLID_Create_Countersunk;
}

FSOLIDCreateCountersunk::~FSOLIDCreateCountersunk(void)
{
	
}

void FSOLIDCreateCountersunk::GetUGInfo()
{
	cout << "NX Feature Name: " << _ugFeature->GetFeatureName().GetUTF8Text() << endl;
	cout << "Get FSOLIDCreateCountersunk Info" << endl;
	
	//string JID = _ugFeature->JournalIdentifier().GetUTF8Text();
	/*unsigned first = JID.find("(");
	unsigned last = JID.find(":");
	string strNew = JID.substr (first+1,last-first-1);
	NXOpen::Features::Feature* cbore_holePack = _featureList->FindObject("COUNTERBORED HOLE(" + strNew + ")");
	*/	
	cout << "Hole Package's Parent Features: " << endl;
	vector<NXOpen::Features::Feature* > parentFeat = _ugFeature->GetParents();

	NXOpen::NXString holeSketJID;
	NXOpen::Features::Feature* holeSketch;
	for(int i = 0; i<parentFeat.size(); i++){
		cout << "	" << parentFeat.at(i)->JournalIdentifier().GetUTF8Text();
		if(!strcmp(parentFeat.at(i)->FeatureType().GetUTF8Text(),"SKETCH")){
			holeSketJID = parentFeat.at(i)->JournalIdentifier();
			//holeSketch = parentFeat.at(i);
			cout << " <- Hole Sketch";
		}
		cout << endl;
	}

	_refSket = (FSketchHolePoint *)(GetPart()->Get_pFeatureByJID(holeSketJID) );

	_origin = _refSket->GetOrigin();
	_xdir =_refSket->GetX();
	_ydir =_refSket->GetY();

	//cout << "Hole Sketch's Parent Features: " << endl;
	//vector<NXOpen::Features::Feature* > holeSketchParentFeat = holeSketch->GetParents();
	//for(int i = 0; i<holeSketchParentFeat.size(); i++){
	//	cout << "	" << holeSketchParentFeat.at(i)->JournalIdentifier().GetUTF8Text() << endl;
	//	for(int j=0; j<holeSketchParentFeat.at(i)->GetParents().size(); j++){
	//		holeSketRefPlaneJID = holeSketchParentFeat.at(i)->GetParents().at(j)->JournalIdentifier();
	//		cout << "		" << holeSketchParentFeat.at(i)->GetParents().at(j)->JournalIdentifier().GetUTF8Text() << endl;
	//	}
	//}
		
	vector<NXOpen::NXObject *> geoms = _refSket->_ugSketch->GetAllGeometry();

	cout << "Get Hole Point info from Hole Sketch" << endl;
	cout << "	Num of Sketch Objects in Hole Sketch: " << geoms.size() << endl;
 	for(int i = 0; i<geoms.size(); i++){
		Feature * pSketchObjectFeature = 0;
		const char * sketObjName = geoms.at(i)->Name().GetUTF8Text();
		if(!_strnicmp(sketObjName, "Point", 1)){
			NXOpen::Point* point = _pPart->_ugPart->Points()->FindObject(geoms.at(i)->JournalIdentifier());
			points.push_back(point->Coordinates());
			cout <<	"	Point: (" << point->Coordinates().X << " , " << point->Coordinates().Y << " , " << point->Coordinates().Z << ")" << endl; 
		}else{
			cout << "	" << geoms.at(i)->Name().GetUTF8Text() << " is unsupported sketch object" <<endl;
		}
	}

	NXOpen::Features::HolePackage * hole(dynamic_cast<NXOpen::Features::HolePackage *>(_ugFeature));
	NXOpen::Features::HolePackageBuilder* holePackBuilder = _featureList->CreateHolePackageBuilder(hole);
		
	diameter = ValRnd(holePackBuilder->DrillSizeHoleDiameter()->Value());
	depth = ValRnd(holePackBuilder->DrillSizeHoleDepth()->Value());
	head_diameter = ValRnd(holePackBuilder->GeneralCountersinkDiameter()->Value());
	head_angle = ValRnd(holePackBuilder->GeneralCountersinkAngle()->Value());
	bottom_angle = ValRnd(holePackBuilder->GeneralTipAngle()->Value());
	
	
	cout << "Diameter: " << diameter << endl;
	cout << "Depth: " << depth << endl;
	cout << "Countersink Diameter: " << head_diameter << endl;
	cout << "Head Angle: " << head_angle << endl;
	cout << "Tip Angle: " << bottom_angle << endl;
	cout << endl;

	/*cout << _ugFeature->JournalIdentifier().GetUTF8Text() << endl;
	NXOpen::Features::HoleFeatureBuilder* holeFeatBuilder = _featureList->CreateHoleFeatureBuilder(_ugFeature);
*/
	//double cbDepth = holeFeatBuilder->CounterboreDepth()->Value();

	//NXOpen::Features::HolePackageBuilder *holeCBbuilder;

	//holeCBbuilder->HolePosition()->GetSectionData();
}


void FSOLIDCreateCountersunk::ToTransCAD()
{
	/*TransCAD::IReferencePtr spSelectedPlane;
	if(strstr(holeSketRefPlaneJID.GetUTF8Text(),"DATUM_PLANE")){
		cout << "Selected Hole Sketch Ref Plane is Datum Plane" << endl;
		bstr_t datumPlaneName = GetPart()->Get_pFeatureByJID(holeSketRefPlaneJID)->GetName().c_str();
		spSelectedPlane = GetPart()->_spPart->SelectObjectByName(datumPlaneName);
	}else if(strstr(holeSketRefPlaneJID.GetUTF8Text(),"DATUM_CSYS")){
		cout << "Selected Hole Sketch Ref Plane is Reference Plane" << endl;
		if(abs(_xdir.X()) == 1){
			if(abs(_ydir.Y()) == 1){
				spSelectedPlane = GetPart()->_spPart->SelectObjectByName("XYPlane");
			}else{
				spSelectedPlane = GetPart()->_spPart->SelectObjectByName("ZXPlane");
			}
		}else{
			spSelectedPlane = GetPart()->_spPart->SelectObjectByName("YZPlane");
		}
	}else{
		cout << "Selected Hole Sketch Ref Plane is Body Face" << endl;
		spSelectedPlane = GetPart()->_spPart->SelectPlaneByAxis(_origin.X(), _origin.Y(), _origin.Z(), 
															_xdir.X(), _xdir.Y(), _xdir.Z(), _ydir.X(), _ydir.Y(), _ydir.Z());
	}
	
	cout << "Selected Hole Reference Plane in TransCAD: " << spSelectedPlane->GetReferenceeName() << endl;*/

	TransCAD::IReferencePtr spSelectedPlane = GetPart()->_spPart->SelectPlaneByAxis(_origin.X(), _origin.Y(), _origin.Z(), 
															_xdir.X(), _xdir.Y(), _xdir.Z(), _ydir.X(), _ydir.Y(), _ydir.Z());
	cout << "Hole Sketch Reference Plane in TransCAD: " << spSelectedPlane->GetReferenceeName() << endl;
	
	for (int i=0; i<points.size(); i++){
		
		//-------------------  Set Result_Object_Name  -------------------//
		char buffer[20];
		_itoa( _cntsnkCnt++, buffer, 10 );
		SetName("CountersunkHole" + (string)buffer);
		GetPart()->_spFeatures->AddNewSolidHoleCountersunkFeature(GetName().c_str(), spSelectedPlane, points.at(i).X, points.at(i).Y, points.at(i).Z, 
			diameter/2, depth, head_diameter/2, head_angle, bottom_angle);
		cout << "FSOLIDCreateCountersunk: " << GetName().c_str() << " Translated" << endl;
	}
	cout << endl;
}