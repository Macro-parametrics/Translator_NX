#pragma once

#include "Feature.h"

class Part;
class FSketch;

class FSOLIDCreateCutRevolve :
	public Feature
{
public:

	FSOLIDCreateCutRevolve(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);
	virtual ~FSOLIDCreateCutRevolve(void);

	FSketch * GetProSket() {return _pProFSket;}

	void SetStaAng(double sA) {_dStaAng = sA;}
	void SetEndAng(double eA) {_dEndAng = eA;}
	double GetStaAng() {return _dStaAng;}
	double GetEndAng() {return _dEndAng;}

	virtual void GetUGInfo();
	virtual void ToTransCAD();

private :
	static int _nCutRevCnt;	//!< Solid Create Protrusion Extrude count

protected:
	FSketch * _pProFSket;
	double	_dStaAng;
	double	_dEndAng;

	VARIANT_BOOL _bFlip;
	EndType	_StaType;	
	EndType	_EndType;
	
};
