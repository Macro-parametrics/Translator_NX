#pragma once

#include "Feature.h"

class Part;

class FSketchHolePoint :
	public Feature
{
public:

	FSketchHolePoint(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);
	virtual ~FSketchHolePoint(void);

	void	 GetSketInfo(double csys[12]) {for(int i=0; i<12 ; i++) csys[i] = _sketInfo[i];}
	Point3D	GetOrigin(void) {return Point3D(_sketInfo[9], _sketInfo[10], _sketInfo[11]);}
	Direct3D GetX(void){return Direct3D(_sketInfo[0], _sketInfo[1], _sketInfo[2]);}
	Direct3D GetY(void){return Direct3D( _sketInfo[3], _sketInfo[4], _sketInfo[5]);}
	Direct3D GetZ(void){return Direct3D(_sketInfo[6], _sketInfo[7], _sketInfo[8]);}
	NXOpen::Sketch* _ugSketch;

	virtual void GetUGInfo();
	virtual void ToTransCAD();


private :
	
	static int _pntSketCnt;	//!< Solid Create Protrusion Extrude count

protected:
	
	double _sketInfo[12];	// sketch Info Array Origin:0~2, Xdir:3~5 : Ydir:6~8 

};