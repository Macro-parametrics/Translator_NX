#pragma once
#include "Feature.h"
#include "Definition.h"

class Part;

class FDatumPlaneOffset : public Feature
{
private:
	Point3D		_origin;
	Direct3D	_normal;

	double		_distance;
	string		_coordinate;
	string		_featurename;
	BOOL		_isFlip;

public:
	FDatumPlaneOffset(Part * pPart, TransCAD::IFeaturePtr spFeatrue);
	virtual ~FDatumPlaneOffset(void);

	virtual void GetInfo();
	virtual void ToUG();

	/* 맴버변수 접근 함수 */
	void GetOrigin(Point3D &origin) { origin = _origin; }
	void GetNormal(Direct3D &normal) { normal = _normal; }
	double GetOffsetDist() { return _distance; }
	string GetCoordinate() { return _coordinate; }
	string GetFeatName() { return _featurename; }
};