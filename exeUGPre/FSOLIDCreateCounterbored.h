#pragma once

#include "Feature.h"

class Part;
class FSketchHolePoint;

class FSOLIDCreateCounterbored :
	public Feature
{
public:

	FSOLIDCreateCounterbored(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);
	virtual ~FSOLIDCreateCounterbored(void);


	virtual void GetUGInfo();
	virtual void ToTransCAD();

private :
	static int _cntbrdCnt;		// Fillet Count
protected:
	FSketchHolePoint * _refSket;

	Point3D _origin;
	Direct3D _xdir;
	Direct3D _ydir;

	double head_diameter;
	double head_depth;
	double lower_hole_diameter;
	double depth;
	double bottom_angle;

	NXOpen::NXString holeSketRefPlaneJID;
	vector<NXOpen::Point3d> points;
};