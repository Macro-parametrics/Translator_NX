#pragma once

#include "Feature.h"
#include <NXOpen\Body.hxx>
#include <NXOpen\Features_ThroughCurvesBuilder.hxx>

class Part;
class FSketch;

class FeatureSOLIDCreateLoft :
	public Feature
{
public:
	FeatureSOLIDCreateLoft(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FeatureSOLIDCreateLoft(void);

	FSketch * GetFeatureProfileSketch() {return _featureProfileSketch;}

	virtual void GetInfo();
	virtual void ToUG();

	NXOpen::Features::ThroughCurvesBuilder *loftBuilderSet();
	//BOOL isIntersect(NXOpen::Body * toolBody, NXOpen::Body * targetBody);

private:
	bool	_isCut;
	int _sketchNumber;
	string *_tcSketch;

protected:
	FSketch * _featureProfileSketch;
};
