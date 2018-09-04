#pragma once

#include "Feature.h"

class Part;

class FSOLIDOperatePatternRectangular :
	public Feature
{
public:

	FSOLIDOperatePatternRectangular(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);
	virtual ~FSOLIDOperatePatternRectangular(void);

	virtual void GetUGInfo();
	virtual void ToTransCAD();

private :
	static int _patRecCnt;	//!< Solid Create Protrusion Extrude count

protected:
	vector<Feature *> _spTargetFeatures;

	double ColumnSpacing;
	double ColDirX;
	double ColDirY;
	double ColDirZ;
	int ColumnNumber;

	double RowSpacing;
	double RowDirX;
	double RowDirY;
	double RowDirZ;
	int RowNumber;

};
