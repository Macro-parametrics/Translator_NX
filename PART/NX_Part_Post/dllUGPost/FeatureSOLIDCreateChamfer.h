#pragma once

#include "Feature.h"
#include "Definition.h"
#include <vector>

#include <NXOpen\Edge.hxx>

class Part;

class FeatureSOLIDCreateChamfer :
	public Feature
{
public:
	FeatureSOLIDCreateChamfer(Part * pPart, TransCAD::IFeaturePtr spFeature);

	virtual ~FeatureSOLIDCreateChamfer(void);

	virtual void GetInfo();
	virtual void ToUG();

	vector<NXOpen::Edge *> GetEdges();

protected:
	double  _point1[ 3 ];
	double  _point2[ 3 ]; 
	
	double	_value;
	double	_length;
	double _chamferLength, _chamferAngle;

	vector<string> _featureName;
	int _edgeCount;
	vector<Point3D> _startP;
	vector<Point3D> _endP;
	vector<int> _edgeType;	// 0: line edge, 1: circle edge
};
