#pragma once
#if ! defined ( __hp9000s800 ) && ! defined ( __sgi ) && ! defined ( __sun )
#   include <strstream>
#   include <iostream>
using std::ostrstream;
using std::endl;
using std::ends;
using std::cerr;
#else
#   include <strstream.h>
#   include <iostream.h>
#endif

#include <math.h>

#include <uf.h>

#define UF_CALL(X) (report_error( __FILE__, __LINE__, #X, (X)))

#define MAX_CHAR_LIMIT	20
#define MAX_DIGITS		18
#define MAX_DEPTH		1000000

//--------------  Types ----------------//
//-------------- Struct ----------------//
//-------------- Etc... ----------------//


typedef class Vector3D Point3D;
typedef class Vector3D Direct3D;

class Vector3D {
public:

	Vector3D(void) : _x(0.0), _y(0.0), _z(0.0) {}
	Vector3D(double x, double y, double z) : _x(x), _y(y), _z(z) {}
	Vector3D(double coord[]) : _x(coord[0]), _y(coord[1]), _z(coord[2]) {}

	double X(void) const { return _x; }
	double Y(void) const { return _y; }
	double Z(void) const { return _z; }

	void X(double x) { _x = x; }
	void Y(double y) { _y = y; }
	void Z(double z) { _z = z; }

	void SetXYZ(double x, double y, double z) { _x = x; _y = y; _z = z; }
	void SetXYZ(double rhs[3]) { _x = rhs[0]; _y = rhs[1]; _z = rhs[2]; }

	double & operator[](int i) { return *(&_x + i); }

	Vector3D operator+(const Vector3D & rhs) {
		return Vector3D(_x + rhs._x, _y + rhs._y, _z + rhs._z);
	}
	Vector3D operator-(const Vector3D & rhs) {
		return Vector3D(_x - rhs._x, _y - rhs._y, _z - rhs._z);
	}
	Vector3D operator*(double d) {
		return Vector3D(_x * d, _y * d, _z * d);
	}
	Vector3D operator/(double d) {
		return Vector3D(_x / d, _y / d, _z / d);
	}
	// Dot product
	double operator*(Vector3D & d) {
		return (_x*d._x + _y*d._y + _z*d._z);
	}
	// Cross Product
	Vector3D operator^(Vector3D & d) {
		double i = _y * d._z - _z * d._y;
		double j = _z * d._x - _x * d._z;
		double k = _x * d._y - _y * d._x;
		return Vector3D(i, j, k);
	}
	bool operator==(const Vector3D & rhs)
	{
		return _x == rhs._x && _y == rhs._y && _z == rhs._z;
	}
	void operator=(const Vector3D & rhs) {
		_x = rhs._x; _y = rhs._y; _z = rhs._z;
	}

	double Size() const { return sqrt(_x*_x + _y*_y + _z*_z); }
	Vector3D Norm(void) { return Vector3D(_x, _y, _z) / Size(); }

private:
	double _x, _y, _z;
};


enum EntityType {
	Entity_VERTEX,
	Entity_EDGE,
	Entity_FACE,
	Entity_FEATURE,
	Entity_SKETCHSEGMENT,
	Entity_SURFACE,
	Entity_PLANE,
	Entity_HDirection,
	Entity_VDirection,
	Entity_Origin
};


enum EndType {
	End_Dimension,
	End_ThroughAll,
	End_ThroughNext,
	End_UpToVertex,
	End_UpToSurface,
	End_UpToOffsetFromSurface,
	End_MidPlane,
	End_UpToPlane,
	End_UpToLast
};

enum PropagationType {
	Minimal,
	Tangency
};

enum SketchItemType {
	POINT2D,
	LINE2D,
	CIRCLE2D,
	CENTERLINE2D,
	ARC2D
};

//------------  External Functions  -------------//

extern void Map(const double A[12], double B[3]);
extern void Map2(const double A[12], double B[3]);
extern void Cir_Map(const double A[12], double C[3]);
extern void ValRndPnt(double C[3]);
extern double ValRnd(double tmp);
extern void ValRndPnt6(double C[3]);
extern double ValRnd6(double tmp);
extern int report_error(char *file, int line, char *call, int irc);
extern int WaitGetEnter(void);
extern double ValRnd9(double val);

/*
enum UG_FeatureType {
UG_INTERSECT,
UG_ABSOLUTE_DATUM_AXIS,
UG_ABSOLUTE_DATUM_PLANE,
UG_BEAD,
UG_BLEND,
UG_BLOCK,
UG_BOOLEAN,
UG_BOSS,
UG_BREP,
UG_BRIDGESRF_SHEET,
UG_CPROJ,
UG_CPROJ_CMB,
UG_DCURVE,
UG_EXTRACT_DATUM_AXIS,
UG_EXTRACT_DATUM_PLANE,
UG_EXTRACT_STRING,
UG_FACEPAIR_DEF,
UG_FACEPAIR_SEL,
UG_FACE_BLEND,
UG_FLANGE,
UG_GENERAL_POCKET,
UG_GENERAL_FLANGE,
UG_HELIX,
UG_HOLLOW,
UG_Heal,
UG_INSET_FLANGE,
UG_INSTANCE,
UG_INTERSECTION_CURVES,
UG_ISOCLINE_CURVE,
UG_JOIN_FACE,
UG_META,
UG_MIDSRF,
UG_MIRROR,
UG_OFFSET_CURVE,
UG_PATCH,
UG_PROJ_DCURVE,
UG_QUILT,
UG_SECTION_SURF,
UG_SEW,
UG_SIMPLE_SMHOLE,
UG_SKETCH,
UG_SKIN,
UG_SMCUTOUT,
UG_SMSLOT,
UG_SPLINE_LAW,
UG_SS,			// SMART SURFACE,
UG_SUBDIVIDE_FACE,
UG_SURFACE_REGION,
UG_SWEEP,
UG_SYMBOLIC_THREAD,
UG_THRU_CURVE,
UG_THRU_CURVE_MESH,
UG_TRIMMED_SH,
UG_UDF,			// A user defined feature defined prior to V15.0
UG_UDF_DEF,		// A user defined feature definition.
UG_UDF_INS,		// A user defined feature instantiation.
UG_VAR_OFFSET_SHEET
};
*/