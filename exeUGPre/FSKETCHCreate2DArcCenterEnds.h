#pragma once

#include "Feature.h"
#include "FSketch.h"

class Part;
class FSketch;
class Feature;

class FSKETCHCreate2DArcCenterEnds :
	public Feature
{
public:

	FSKETCHCreate2DArcCenterEnds(Part* part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection* featureList, NXOpen::NXString jID, FSketch* pFSketch);
	virtual ~FSKETCHCreate2DArcCenterEnds(void);
	
	FSketch * GetFSketch() {return _pFSketch;}

	void SetCntPnt(const Point3D & cP) {_cP = cP;}
	void SetRadius(const double dR) {_dR = dR;}
	void GetCntPnt(Point3D & cP) {cP = _cP;}
	double GetRadius() const {return _dR;}
	
	virtual void GetUGInfo();
	virtual void ToTransCAD();

private:
	static int _2DArcCtrEndsCnt;
	NXOpen::NXString _jID;
	Point3D		_cP;
	Point3D     _sP;
	Point3D     _eP;
    double		_dR;
	FSketch *	_pFSketch;
};
