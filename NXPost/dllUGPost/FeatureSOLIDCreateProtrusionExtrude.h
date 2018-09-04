#pragma once

#include "Feature.h"
#include <NXOpen\Features_ExtrudeBuilder.hxx>
#include <NXOpen\Body.hxx>

class Part;
class FSketch;

class FeatureSOLIDCreateProtrusionExtrude : public Feature
{
public:
	FeatureSOLIDCreateProtrusionExtrude(Part * pPart, TransCAD::IFeaturePtr spFeature);
	~FeatureSOLIDCreateProtrusionExtrude(void);


	/** 맴버 변수 접근 함수 **/
	FSketch * GetFeatureProfileSketch() { return _featureProfileSketch; }
	double GetStartDepth() { return _startDepth; }
	double GetEndDepth() { return _endDepth; }

	virtual void GetInfo();
	virtual void ToUG();

	NXOpen::Features::ExtrudeBuilder * extrudeBuilderSet(FSketch * profileSketch);
	BOOL FeatureSOLIDCreateProtrusionExtrude::isIntersect(NXOpen::Body * toolBody, NXOpen::Body * targetBody);

public:
	/** 맴버 변수 **/
	double _startDepth;
	double _endDepth;
	bool _flip;
	TransCAD::StdExtrudeEndType _startCondition;
	TransCAD::StdExtrudeEndType _endCondition;

protected:
	FSketch * _featureProfileSketch;
};