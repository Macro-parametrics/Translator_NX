#pragma once

#include "dllUGPost.h"
#include <string>

class FSketch;

using namespace std;

class SKETCHItem
{
public:
	SKETCHItem(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem);
	~SKETCHItem(void);

	/** 맴버변수 접근 함수 **/
	FSketch *	GetSketchFeat() { return _pFSketch; }
	string		GetSketchItemName() { return _sketItemName; }

	TransCAD::IStdSketchGeometryPtr GetTransCADGeometryItem() { return _spItem; }
	TransCAD::StdSketchGeometryType GetSketType() { return _spItem->Type; }

	virtual void GetInfo();
	virtual void ToUG();

protected:
	FSketch *	_pFSketch;
	size_t		_sketchSize;
	string		_sketItemName;
	TransCAD::IStdSketchGeometryPtr _spItem;
};