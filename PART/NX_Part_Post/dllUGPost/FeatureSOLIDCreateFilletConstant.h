#pragma once

#include "Feature.h"
#include "Definition.h"
#include <vector>

#include <NXOpen\Edge.hxx>

class Part;

class FeatureSOLIDCreateFillet : public Feature
{
public:
	FeatureSOLIDCreateFillet(Part * pPart, TransCAD::IFeaturePtr spFeature);
	~FeatureSOLIDCreateFillet(void);
	
	virtual void GetInfo();
	virtual void ToUG();

	vector<NXOpen::Edge *> GetEdges();

private:
	double _radius;
	int _edgeCount;
	vector<Point3D> _startP;
	vector<Point3D> _endP;
	vector<int> _edgeType;	// 0: line edge, 1: circle edge

protected:
	vector<string> _featureName;
};