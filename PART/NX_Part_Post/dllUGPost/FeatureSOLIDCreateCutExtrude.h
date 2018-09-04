#pragma once

#include "Feature.h"
#include <NXOpen\Body.hxx>
#include <NXOpen\Features_ExtrudeBuilder.hxx>

class Part;
class FSketch;

class FeatureSOLIDCreateCutExtrude : public Feature
{
protected:
	FSketch * _featureProfileSketch;

public:
	FeatureSOLIDCreateCutExtrude(Part * pPart, TransCAD::IFeaturePtr spFeature);
	~FeatureSOLIDCreateCutExtrude(void);

	virtual void GetInfo();
	virtual void ToUG();

	NXOpen::Features::ExtrudeBuilder * cutExtrudeBuilderSet(FSketch * profileSketch);
	BOOL isIntersect(NXOpen::Body * toolBody, NXOpen::Body * targetBody);

private:
	/** ¸É¹ö º¯¼ö **/
	double _startDepth;
	double _endDepth;
	bool _flip;
	TransCAD::StdExtrudeEndType _startCondition;
	TransCAD::StdExtrudeEndType _endCondition;
};