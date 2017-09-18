#pragma once

#include "Feature.h"
#include "FSketch.h"

class Part;
class FSketch;

class FSKETCHCreate2DCenterline :
	public Feature
{
public:

	FSKETCHCreate2DCenterline(Part* part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection* featureList, NXOpen::NXString jID, FSketch* pFSketch);
	virtual ~FSKETCHCreate2DCenterline(void);

	FSketch * GetFSketch() {return _pFSketch;}

	void SetStaPnt(const Point3D & sP) {_sP = sP;}
	void SetEndPnt(const Point3D & eP) {_eP = eP;}
	void GetStaPnt(Point3D & sP) {sP = _sP;}
	void GetEndPnt(Point3D & eP) {eP = _eP;}
	void GetPickPnt(Point3D & pickP) {pickP = (_sP+_eP)*0.5;}

	virtual void GetUGInfo();
	virtual void ToTransCAD();
	
private:
	static int _nCLineCnt;
	NXOpen::NXString _jID;

	Point3D		     _sP;
	Point3D		     _eP;
	FSketch * _pFSketch;
};