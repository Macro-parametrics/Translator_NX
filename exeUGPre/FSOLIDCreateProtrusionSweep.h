#pragma once

#include "Feature.h"

class Part;
class FSketch;

class FSOLIDCreateProtrusionSweep :
	public Feature
{
public:
	FSOLIDCreateProtrusionSweep(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);
	virtual ~FSOLIDCreateProtrusionSweep(void);

	void SetProSket(FSketch * fps) {_pProFSket=fps;}
	FSketch *  GetProSket() {return _pProFSket;}
	void SetGuideCurve(FSketch * fgc) {_pGuiFSket=fgc;}
	FSketch *  GetGuideSketch() {return _pGuiFSket;}

	virtual void GetUGInfo();
	virtual void ToTransCAD();


private :
	static int _proSweepCount;	//!< Solid Create Protrusion Extrude Sweep count

protected:
	FSketch * _pProFSket;
	FSketch * _pGuiFSket;
};