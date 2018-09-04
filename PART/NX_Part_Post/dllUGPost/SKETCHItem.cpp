#include "stdafx.h"
#include "SKETCHItem.h"
#include "FSketch.h"

#include <iostream>


SKETCHItem::SKETCHItem(FSketch * pFSketch, TransCAD::IStdSketchGeometryPtr spItem)
{
	_pFSketch		= pFSketch;		// Set FSketch pointer
	_spItem			= spItem;		// Set TransCAD Sketch item pointer
	_sketchSize		= pFSketch->GetSketchItemsSize();		// Set total sketch items
	_sketItemName	= spItem->Name;	// Set Sketch item name
}

SKETCHItem::~SKETCHItem(void) {}

void SKETCHItem::GetInfo() {}

void SKETCHItem::ToUG() {}