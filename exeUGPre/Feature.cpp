#pragma once
#include "Feature.h"
#include "stdafx.h"
#include <iostream>

Feature::Feature(Part * part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection * featureList)
{
	_ugFeature = ugFeature;
	_featureList = featureList;
	_pPart = part;
	_index = part->GetFeaturesSize();
	_fType = FEATURE;
	_featJID = ugFeature->JournalIdentifier();
	
}

Feature::~Feature(void)
{
}

void Feature::GetUGInfo()
{
}

void Feature::ToTransCAD()
{
}
