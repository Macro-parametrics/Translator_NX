#pragma once

#include <math.h>


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

	double X(void) const {return _x;}
	double Y(void) const {return _y;}
	double Z(void) const {return _z;}
	void SetX(double x) {_x=x;}
	void SetY(double y) {_y=y;}
	void SetZ(double z) {_z=z;}
	void SetXYZ(double sz[3]) {_x=sz[0];_y=sz[1];_z=sz[2];}
	void SetXYZ(double x, double y, double z) {_x=x;_y=y;_z=z;}

	double & operator[](int i) {return *(&_x + i);}

	Vector3D operator+(const Vector3D & rhs){
		return Vector3D(_x + rhs._x, _y + rhs._y, _z + rhs._z);
	}
	Vector3D operator-(const Vector3D & rhs){
		return Vector3D(_x - rhs._x, _y - rhs._y, _z - rhs._z);
	}
	Vector3D operator*(double d){
		return Vector3D(_x * d, _y * d, _z * d);
	}
	Vector3D operator/(double d){
		return Vector3D(_x / d, _y / d, _z / d);
	}
	// Dot product
	double operator*(Vector3D & d){		
		return (_x*d._x+_y*d._y+_z*d._z);
	}
	// Cross Product
	Vector3D operator^(Vector3D & d) {	
		double i = _y * d._z - _z * d._y;
		double j = _z * d._x - _x * d._z;
		double k = _x * d._y - _y * d._x;
		return Vector3D(i,j,k);
	}
	bool operator==(const Vector3D & rhs)
	{
		return _x == rhs._x && _y == rhs._y && _z == rhs._z;
	}
	void operator=(const Vector3D & rhs) {
		_x = rhs._x; _y = rhs._y; _z = rhs._z;
	}
	
	double Size() const { return sqrt(_x*_x + _y*_y + _z*_z); }
	Vector3D Norm(void) { return Vector3D(_x,_y,_z) / Size() ; }

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

//-------------- Functions ----------------//

extern void Map(const double A[12],double B[3]);
extern void Cir_Map (const double A[12], double Center[3] );
extern void ValRndPnt(double C[3]);
extern double ValRnd(double tmp);
extern void ValRndPnt6(double C[3]);
extern double ValRnd6(double tmp);
extern int report_error( char *file, int line, char *call, int irc);
extern int WaitGetEnter(void);


enum FType {
	FEATURE,
		DATUM_PLANE,	// not in the draft
		SKETCH,			// not in the draft
		SKETCH_HOLE_POINT,
		SKETCH_Create_2D_Line_2Points,
		SKETCH_Create_2D_Centerline,
		SKETCH_Create_2D_Point,
		SKETCH_Create_2D_Arc_CenterEnds,
		SKETCH_Create_2D_Arc_3Points,
		SKETCH_Create_2D_Circle_CenterPoint,
		SOLID_Create_Protrusion_Extrude,
		SOLID_Create_Protrusion_Revolve,
		SOLID_Create_Protrusion_Sweep,
		SOLID_Create_Cut_Extrude,
		SOLID_Operate_Filleting_Fillet_Constant,
		SOLID_Operate_Filleting_Chamfer_Constant,
		SOLID_Operate_Pattern_Rectangular,
		SOLID_Operate_Pattern_Circular,
		SOLID_Create_Countersunk,
		SOLID_Create_Counterbored
};






// UG Feature Type enum
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


	<!-- direction -->
	<xs:complexType name="direction">
		<xs:sequence maxOccurs="3" minOccurs="2">
			<xs:element name="direction_ratios" type="xs:double" />
		</xs:sequence>
	</xs:complexType>
	<!-- option_for_rotation_direction -->
	<xs:simpleType name="option_for_rotation_direction">
		<xs:restriction base="xs:string">
			<xs:enumeration value="cw" />
			<xs:enumeration value="ccw" />
		</xs:restriction>
	</xs:simpleType>
	<!-- helix_definition_type -->
	<xs:simpleType name="helix_definition_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="Pitch_Rev" />
			<xs:enumeration value="Height_Rev" />
			<xs:enumeration value="Height_Pitch" />
			<xs:enumeration value="Sprial" />
		</xs:restriction>
	</xs:simpleType>
	<!-- coupling_rule -->
	<xs:simpleType name="coupling_rule">
		<xs:restriction base="xs:string">
			<xs:enumeration value="ratio" />
			<xs:enumeration value="tangency" />
			<xs:enumeration value="vertex" />
			<xs:enumeration value="curvature" />
		</xs:restriction>
	</xs:simpleType>
	<!-- end_type -->
	<xs:simpleType name="end_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="Dimension" />
			<xs:enumeration value="ThroughAll" />
			<xs:enumeration value="ThroughNext" />
			<xs:enumeration value="UpToVertex" />
			<xs:enumeration value="UpToSurface" />
			<xs:enumeration value="UpToOffsetFromSurface" />
			<xs:enumeration value="MidPlane" />
			<xs:enumeration value="UpToPlane" />
			<xs:enumeration value="UpToLast" />
		</xs:restriction>
	</xs:simpleType>
	<!-- groove_definition_type -->
	<xs:simpleType name="groove_definition_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="rectangle_type" />
			<xs:enumeration value="ball_type" />
			<xs:enumeration value="U_type" />
		</xs:restriction>
	</xs:simpleType>
	<!-- limit_type -->
	<!-- DELETED
	<xs:simpleType name="limit_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="dimension" />
			<xs:enumeration value="up_to_next" />
			<xs:enumeration value="up_to_last" />
			<xs:enumeration value="up_to_plane" />
			<xs:enumeration value="up_to_surface" />
		</xs:restriction>
	</xs:simpleType>
	-->
	<!-- propagation_type -->
	<xs:simpleType name="propagation_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="minimal" />
			<xs:enumeration value="tangency" />
		</xs:restriction>
	</xs:simpleType>
	<!-- chamfer_type -->
	<xs:simpleType name="chamfer_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="chamfer_length" />
			<xs:enumeration value="chamfer_angle" />
		</xs:restriction>
	</xs:simpleType>
	<!-- extension_limit_type -->
	<!-- DELETED
	<xs:simpleType name="extension_limit_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="length" />
			<xs:enumeration value="element" />
		</xs:restriction>
	</xs:simpleType>
	-->
	<!-- continuity_type -->
	<xs:simpleType name="continuity_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="tangency" />
			<xs:enumeration value="curvature" />
			<xs:enumeration value="linear" />
		</xs:restriction>
	</xs:simpleType>
	<!-- entity_type -->
	<xs:simpleType name="entity_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="VERTEX" />
			<xs:enumeration value="EDGE" />
			<xs:enumeration value="FACE" />
			<xs:enumeration value="FEATURE" />
			<xs:enumeration value="SKETCHSEGMENT" />
			<xs:enumeration value="SURFACE" />
			<xs:enumeration value="PLANE" />
			<xs:enumeration value="HDirection" />			
			<xs:enumeration value="VDirection" />
			<xs:enumeration value="Origin" />
		</xs:restriction>
	</xs:simpleType>
	<!-- These types are not defined in the macro_NWI document. Only used for validity. -->
	<xs:simpleType name="plane_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="PlaneXY" />
			<xs:enumeration value="PlaneYZ" />
			<xs:enumeration value="PlaneZX" />
		</xs:restriction>
	</xs:simpleType>
	<xs:simpleType name="same_points_type">
		<xs:restriction base="xs:string">
			<xs:enumeration value="START_POINT" />
			<xs:enumeration value="END_POINT" />
			<xs:enumeration value="CENTER_POINT" />
			<xs:enumeration value="WHOLE" />
		</xs:restriction>
	</xs:simpleType>

*/
	