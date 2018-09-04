#pragma once

#include "Feature.h"
#include <NXOpen\Body.hxx>
#include <NXOpen\Features_RevolveBuilder.hxx>

class Part;
class FSketch;

class FeatureSOLIDCreateCutRevolve :
	public Feature
{
public:
	FeatureSOLIDCreateCutRevolve(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~FeatureSOLIDCreateCutRevolve(void);

	FSketch * GetFeatureProfileSketch() {return _featureProfileSketch;}

	virtual void GetInfo();
	virtual void ToUG();

	NXOpen::Features::RevolveBuilder * revolveBuilderSet(FSketch * profileSketch, double sA, double eA);
	BOOL isIntersect(NXOpen::Body * toolBody, NXOpen::Body * targetBody);

public:
	bool	_flip;
	double	_startAngle;
	double	_endAngle;

	
protected:
	FSketch * _featureProfileSketch;
};
