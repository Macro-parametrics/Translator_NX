#pragma once

#include "Feature.h"

class Part;

class FSOLIDOperatePatternCircular :
	public Feature
{
public:

	FSOLIDOperatePatternCircular(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);
	virtual ~FSOLIDOperatePatternCircular(void);

	virtual void GetUGInfo();
	virtual void ToTransCAD();

private :
	static int _patCirCnt;	//!< Solid Create Protrusion Extrude count

protected:
	vector<Feature *> _spTargetFeatures;

	double AngularSpacing;
	int AngularNumber;

	double RadialSpacing;
	int RadialNumber;

	double OrgX;
	double OrgY;
	double OrgZ;

	double DirX;
	double DirY;
	double DirZ;
};
