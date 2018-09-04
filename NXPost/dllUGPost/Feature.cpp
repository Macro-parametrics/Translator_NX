#include "stdafx.h"
#include "Feature.h"

#include "Part.h"

using namespace std;

Feature::Feature(Part * pPart, TransCAD::IFeaturePtr spFeature)
{
	_Part = pPart;
	_spFeature = spFeature;
	_featSize = pPart->GetSize();
	
	if ( spFeature )
		_tcFeatName = spFeature->Name;
}

Feature::~Feature(void)
{

}

void Feature::GetInfo() {}

void Feature::ToUG() {}
