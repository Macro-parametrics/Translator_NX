#pragma once

#include "SKETCHItem.h"
#include "Definition.h"
#include "UGReferenceManager.h"

class FSketch;

class SKETCHCreate2DLine2Points : public SKETCHItem
{
public:
	SKETCHCreate2DLine2Points(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
	~SKETCHCreate2DLine2Points(void);


	/** 맴버변수 접근 함수 **/
	void SetStartPoint(Point3D & startPoint) { _startPoint = startPoint; }
	void SetEndPoint(Point3D & EndPoint) { _endPoint = EndPoint; }

	void GetStartPoint(Point3D & startPoint) { startPoint = _startPoint; }
	void GetEndPoint(Point3D & endPoint) { endPoint = _endPoint; }
	
	Point3D GetStartPoint() { return _startPoint; }
	Point3D GetEndPoint() { return _endPoint; }
	

	virtual void GetInfo();
	virtual void ToUG();

private:
	Point3D _startPoint;
	Point3D _endPoint;

	

};