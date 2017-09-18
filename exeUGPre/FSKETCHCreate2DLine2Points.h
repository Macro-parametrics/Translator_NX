#pragma once

#include "Feature.h"
#include "FSketch.h"

class Part;
class FSketch;
class Feature;

class FSKETCHCreate2DLine2Points :
	public Feature
{
public:

	FSKETCHCreate2DLine2Points(Part* part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection* featureList, NXOpen::NXString jID, FSketch* pFSketch);
	virtual ~FSKETCHCreate2DLine2Points(void);

	FSketch * GetFSketch() {return _pFSketch;}

	void GetStaPnt(Point3D & sP) {sP = _sP;}
	void SetStaPnt(const Point3D & sP) {_sP = sP;}
	void GetEndPnt(Point3D & eP) {eP = _eP;}
	void SetEndPnt(const Point3D & eP) {_eP = eP;}
	void GetPickPnt(Point3D & pickPnt) {pickPnt = (_sP + _eP) * 0.5;}//ÁßÁ¡
	
	virtual void GetUGInfo();
	virtual void ToTransCAD();
	
private:
	static int _nLineCnt;
	NXOpen::NXString _jID;
	FSketch *	_pFSketch;
	Point3D		_sP;
	Point3D		_eP;
};
