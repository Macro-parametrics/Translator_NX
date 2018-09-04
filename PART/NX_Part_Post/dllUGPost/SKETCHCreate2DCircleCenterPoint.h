#pragma once

#include "SKETCHItem.h"
#include "Definition.h"
#include "UGReferenceManager.h"

class FSketch;

class SKETCHCreate2DCircleCenterPoint : public SKETCHItem
{


public:
	SKETCHCreate2DCircleCenterPoint(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
	~SKETCHCreate2DCircleCenterPoint(void);

	void SetCenterPoint(const Point3D & centerPoint) { _centerPoint = centerPoint; }
	void SetRadius(const double radius) { _radius = radius; }

	Point3D GetCenterPoint() { return _centerPoint; }
	double GetRadius() { return _radius; }

	virtual void GetInfo();
	virtual void ToUG();

private:
	Point3D _centerPoint;
	double _radius;
	double _startAngle;
	double _endAngle;
};