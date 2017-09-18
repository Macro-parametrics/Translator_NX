#pragma once

#include "Feature.h"

class Part;

class FSOLIDOperateFilletingChamferConstant :
	public Feature
{
public:

	FSOLIDOperateFilletingChamferConstant(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);
	virtual ~FSOLIDOperateFilletingChamferConstant(void);

	/*double GetRadius() const {return _filRadius;}
	void GetStaPnt(Point3D & sP) {sP=_sP;}
	void GetEndPnt(Point3D & eP) {eP=_eP;}
	void GetPickPnt(Point3D & pP) {pP=_pP;}*/

	virtual void GetUGInfo();
	virtual void ToTransCAD();

private :
	static int _chamfCnt;		// Fillet Count
protected:
	int _numChainSet;
	double	_chamfLength;
	int     _edgeNum;//edge °¹¼ö
	vector<Point3D> _sP;
	vector<Point3D> _eP;
	vector<Point3D> _pP;	// picking point
};
