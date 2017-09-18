#pragma once

#include "Feature.h"
#include "FSketch.h"

class Part;
class FSketch;
class Feature;

class FSKETCHCreate2DCircleCenterPoint :
	public Feature
{
public:

	FSKETCHCreate2DCircleCenterPoint(Part* part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection* featureList, NXOpen::NXString jID, FSketch* pFSketch);
	virtual ~FSKETCHCreate2DCircleCenterPoint(void);
	
	FSketch * GetFSketch() {return _pFSketch;}

	void SetCntPnt(const Point3D & cP) {_cP = cP;}
	void SetRadius(const double dR) {_dR = dR;}
	void GetCntPnt(Point3D & cP) {cP = _cP;}
	double GetRadius() const {return _dR;}
	
	virtual void GetUGInfo();
	virtual void ToTransCAD();

private:
	static int _circle2DCPCnt;
	NXOpen::NXString _jID;
	Point3D		_cP;
    double		_dR;
	FSketch *	_pFSketch;
};
