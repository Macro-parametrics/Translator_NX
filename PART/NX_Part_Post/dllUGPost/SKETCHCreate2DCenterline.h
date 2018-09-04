#pragma once

#include "SKETCHItem.h"
#include "Definition.h"

class FSketch;

//!	SKETCHCreate2DCenterline class
/*!
	2D Line class with two 2DPoint structs
*/
class SKETCHCreate2DCenterline : public SKETCHItem
{
public:

	// A constructor
	SKETCHCreate2DCenterline(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
	// A Desturctor
	virtual ~SKETCHCreate2DCenterline(void);

	void SetStartPoint(const Point3D & startPoint) {_startPoint = startPoint;}
	void SetEndPoint(const Point3D & endPoint) {_endPoint = endPoint;}

	string GetCenterLineJID() { return _centerLineJID; }
	string GetTCName() { return _TransCADName; }
	virtual void GetInfo();
	virtual void ToUG();

public:
	string _TransCADName;
	string _centerLineJID;

private:
	Point3D _startPoint;
	Point3D _endPoint;
};
