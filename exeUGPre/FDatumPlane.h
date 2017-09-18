#pragma once
#include "Feature.h"
//	 FDatumPlane class

class Feature;

class FDatumPlane : public Feature
{

public:

	FDatumPlane(Part * part, NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);	//!< A constructor
	virtual ~FDatumPlane(void);					//!< A Destructor

	void SetOrigin(const Point3D & origin);	/*!< \param origin Origin Point3D */ 
	//void SetNormal(const Direct3D & normal);	/*!< \param normal Normal vector Direct3D */
	void SetXDir(const Direct3D & xDir);
	void SetYDir(const Direct3D & yDir);

    Point3D	GetOrigin();		/*!< \return Cartesian Point struct */
	//Direct3D GetNormal();		/*!< \return Direct3D Vector struct */
	Direct3D GetXDir();
	Direct3D GetYDir();

	virtual void	GetUGInfo();	//!< Translate UG feature into needed infomation
	virtual	void	ToTransCAD();		//!< Write feature infomation into TransCAD


protected:
	bool isReverse;
	NXOpen::NXString offsetRefPlaneJID;
	static int _nDtPlaneCnt;	// FDatumPlane count
	Point3D	_origin;		//!< origin Point3D struct
	//Direct3D _normal;		//!< normal vector Direct3D vector struct
	Direct3D _xDir;
	Direct3D _yDir;

	double _offset;
	//string			_planeXYZ;		//!< plane name string
	
};
