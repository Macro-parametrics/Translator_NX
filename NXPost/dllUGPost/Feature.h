#pragma once

#include "Definition.h"

using namespace std;

class Part;

class Feature
{
public:
	Feature(Part * pPart, TransCAD::IFeaturePtr spFeature);
	virtual ~Feature(void);

	Part * GetPart() { return _Part; }

	void	SetFeatureName(string featureName) { _tcFeatName = featureName; }
	size_t	GetFeatureIndex() { return _featSize; }
	string	GetFeatureName() { return _tcFeatName; }
	TransCAD::IFeaturePtr GetTransCADFeature() { return _spFeature; }

	virtual void GetInfo();
	virtual void ToUG();

/* private처리하면 다른 클래스에서 접근이 용이하지 않음. */
protected:
	Part *	_Part;
	size_t	_featSize;
	string	_tcFeatName;
	TransCAD::IFeaturePtr _spFeature;
};