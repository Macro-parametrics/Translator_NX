//	FSketch Class

#pragma once
#include "Feature.h"

class Feature;
class FSketch : public Feature
{
public:
	FSketch(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);//!< A constructor
	virtual ~FSketch(void);
	void    Clear();

	Feature * GetSketObj(int index) const {return _sketObjList.at(index);}
	size_t	  GetSketObjSize() const {return _sketObjList.size();}
	
	void	 GetSketInfo(double csys[12]) {for(int i=0; i<12 ; i++) csys[i] = _sketInfo[i];}
	Point3D	GetOrigin(void) {return Point3D(_sketInfo[9], _sketInfo[10], _sketInfo[11]);}
	Direct3D GetX(void){return Direct3D(_sketInfo[0], _sketInfo[1], _sketInfo[2]);}
	Direct3D GetY(void){return Direct3D(_sketInfo[3], _sketInfo[4], _sketInfo[5]);}
	Direct3D GetZ(void){return Direct3D(_sketInfo[6], _sketInfo[7], _sketInfo[8]);}

	Feature * GetCLine(void);
	void SetCLine(Feature* pFeature);

	TransCAD::ISketchEditorPtr GetSketchEditorPtr();

	virtual void GetUGInfo();
	virtual void ToTransCAD();

private:
	//Feature * CreateSketObj(tag_t sketObjTag);

private:
	NXOpen::NXString sketRefPlaneJID;
	static int _nSketCnt;	// FSketch count
	//std::vector<NXOpen::NXObject *>* geom2;;
	//NXOpen::Part *  _ugPart1;
	NXOpen::Sketch* _ugSketch;

	std::vector<Feature*>	_sketObjList;
	double _sketInfo[12];	// sketch Info Array Origin:0~2, Xdir:3~5 : Ydir:6~8 


	TransCAD::ISketchEditorPtr _spSketEditor;	// TransCAD SketchEditorPtr
};