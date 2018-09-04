#pragma once

#include "Feature.h"

class Part;
class FSketchHolePoint;

class FSOLIDCreateCountersunk :
	public Feature
{
public:

	FSOLIDCreateCountersunk(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);
	virtual ~FSOLIDCreateCountersunk(void);


	virtual void GetUGInfo();
	virtual void ToTransCAD();

private :
	static int _cntsnkCnt;		// Count
protected:
	FSketchHolePoint * _refSket;

	Point3D _origin;
	Direct3D _xdir;
	Direct3D _ydir;


	double diameter;
	double depth;
	double head_diameter;
	double head_angle;
	double bottom_angle;

	NXOpen::NXString holeSketRefPlaneJID;
	vector<NXOpen::Point3d> points;
};