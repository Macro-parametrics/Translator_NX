#pragma once
#include "Feature.h"

class Part;

class FDatumPlane : public Feature
{
public:
	FDatumPlane(Part * pPart, Point3D &orign, Direct3D &normal);
	FDatumPlane(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FDatumPlane(void);

	/* 맴버변수 접근 함수 */
	void GetOrigin(Point3D &origin) { origin = _origin; }
	void GetNormal(Direct3D &normal) { normal = _normal; }


	virtual void GetInfo();
	virtual void ToUG();

private:
	Point3D		_origin;	// Plane origin coordinate
	Direct3D	_normal;	// Plane normal vector

};