#pragma once

#include "Feature.h"
#include "Definition.h"

#include <NXOpen\Body.hxx>
#include <NXOpen\Features_PatternFeatureBuilder.hxx>


class Part;

class FeatureSOLIDCreatePatternCircular:public Feature
{
public:
    FeatureSOLIDCreatePatternCircular(Part *pPart,TransCAD::IFeaturePtr spFeature);
    ~FeatureSOLIDCreatePatternCircular();

    virtual void GetInfo();
    virtual void ToUG();

    NXOpen::Features::PatternFeatureBuilder *patternBuilderSet();
	
private:
    double _angleIncrement;
	int _angleNum;
	int _radiNum;
	double _spaceByRadi;
	bool _isAssign;
	double _centerAxis[2][3];
	
	int _numFtr;
	string *_targetFtrs;
	string _targetFeature;

protected:
    //Feature * _featureBody;
};