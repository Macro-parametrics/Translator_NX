#pragma once

#include "Feature.h"
#include <NXOpen\Body.hxx>
#include <NXOpen/Features_HolePackageBuilder.hxx>

#include <vector>

#include <NXOpen\Face.hxx>

class Part;

class FeatureSOLIDCreateCounterbored : public Feature
{
public:
	FeatureSOLIDCreateCounterbored(Part *pPart, TransCAD::IFeaturePtr spFeature);
	~FeatureSOLIDCreateCounterbored();

	virtual void GetInfo();
	virtual void ToUG();
	NXOpen::Features::HolePackageBuilder *HolePakageBuilderSet();
	vector<NXOpen::Face *> GetFaces();
	
private:
	string _targetFace;
	double _center[3];
	double _startDepth;
	double _endDepth;
	double _startDiameter;
	double _endDiameter;
	double _angle;

	string GetFaceJID(Point3D origin);
	string GetBodyJID(Point3D origin);
};