#pragma once

#include "Feature.h"
#include "Definition.h"

#include <NXOpen\Body.hxx>
#include <NXOpen\Features_PatternFeatureBuilder.hxx>


class Part;

class FeatureSOLIDCreatePatternRectangular:public Feature
{
public:
    FeatureSOLIDCreatePatternRectangular(Part *pPart,TransCAD::IFeaturePtr spFeature);
    ~FeatureSOLIDCreatePatternRectangular();

    virtual void GetInfo();
    virtual void ToUG();

    NXOpen::Features::PatternFeatureBuilder *patternBuilderSet();
	
private:
    int _numRow;
    int _numCol;
    double _rowSpace;
    double _colSpace;
    double _rowDir[3];
    double _colDir[3];
	
	//////////////////
	int _numFtr;
	string *_targetFtrs;
	///////////////////

	string _targetFeature;
	
	
protected:
   // Feature * _featureBody;
};