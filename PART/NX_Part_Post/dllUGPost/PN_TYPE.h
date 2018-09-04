#ifndef __PN__TYPE__HEADER__INCLUDED__
#define __PN__TYPE__HEADER__INCLUDED__

#include <string>
#include <vector>
using namespace std;

struct pickingPoint
{
	double _x; double _y; double _z;
};

enum selection_type {
	etc_selection_type,
	MP_VERTEX,
	MP_EDGE,
	MP_FACE,
	MP_FEATURE,
	MP_SKETCHSEGMENT,
	MP_SURFACE,
	MP_PLANE,
	MP_HDirection,
	MP_VDirection,
	MP_Origin,
	MP_DEFAULTPLANE
};

enum bn_option {
	Sweep_Feature,
	Extrude_Feature,
	Revolve_Feature,
	Loft_Feature,
	Fillet_Feature,
	Chamfer_Feature,
	Shell_Feature,
	Pattern_Feature,
	Datum_Feature,
	Sketch_Segment
};



struct basic_naming
{
	string feature_id;
	string id1; // sketch
	string id2;  // sketch segment name
	string id3;
	string id4;
	string id5;
	bn_option option;
};

struct OSI
{
	int order;
	int total_num;
};

struct secondary_name
{
	int total_num;
	std::vector<basic_naming> _bn_list;
};

struct EN_F
{
	basic_naming _bn;
	OSI _osi;
	secondary_name _sn;
};

struct EN_E
{
	EN_F	_f1;
	EN_F	_f2;
	OSI		_osi;
};

struct EN_V
{
	EN_F	_f1;
	EN_F	_f2;
	EN_F	_f3;
	OSI		_osi;
};


#endif