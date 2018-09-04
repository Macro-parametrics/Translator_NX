#pragma once

#include "Feature.h"
#include <NXOpen\Body.hxx>
#include <NXOpen/Features_HolePackageBuilder.hxx>

#include <vector>

#include <NXOpen\Face.hxx>

class Part;

class FeatureSOLIDCreateCountersunk : public Feature
{
public:
	FeatureSOLIDCreateCountersunk(Part *pPart, TransCAD::IFeaturePtr spFeature);
	~FeatureSOLIDCreateCountersunk();

	virtual void GetInfo();
	virtual void ToUG();
	NXOpen::Features::HolePackageBuilder *HolePakageBuilderSet();
	vector<NXOpen::Face *> GetFaces();
	
private:
	string _targetFace;
	double _center[3];
	double _sinkAngle;
	double _sinkDiameter;
	double _bottomAngle;
	double _diameter;
	double _depth;

	string GetFaceJID(Point3D origin);
	string GetBodyJID(Point3D origin);
};