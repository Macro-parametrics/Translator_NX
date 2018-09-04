#pragma once
#include "Feature.h"
#include "stdafx.h"

#include <NXOpen/Session.hxx>
#include <NXOpen/Part.hxx>
#include <NXOpen/PartCollection.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>
#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/NXString.hxx>

#include <string>
#include <vector>
using namespace std;

class Feature;

class Part
{
public:
	TransCAD::IPartDocumentPtr  _spDocument;	// TransCAD a document pointer
	TransCAD::IPartPtr          _spPart;		// TransCAD part class pointer
	TransCAD::IFeaturesPtr      _spFeatures;	// TransCAD Featurs pointer
	NXOpen::Part*				_ugPart;
	NXOpen::Session*			_ugSession;
	//int _UGFeatIndex;

private:
	string		                        _fileName;			// UG partfile name
	vector<Feature*>             _featureList;		// feature list for translation
	vector<NXOpen::Features::Feature *> _ugFeatlist;        // UG feauture list
	bool		                        _bFileOpened;

public:
	Part(TransCAD::IApplicationPtr transcad, string fileName, NXOpen::Session* NXSession);
	~Part(void);
	void Clear();

	void PartFileOpen();
	//tag_t GetPartTag() const {return _partTag;}
	bool IsUGFileOpen() const {return _bFileOpened;}

	string GetFileName() const {return _fileName;}
	Feature * GetFeature(int index) const {return _featureList.at(index);}
	size_t GetFeaturesSize() const {return _featureList.size();}
	Feature * Get_pFeatureByJID(NXOpen::NXString jID) const;

	void GetUGInfo();
	void ToTransCAD();

protected:
	Feature * Part::CreateFeature(NXOpen::Features::Feature* ugfeature, NXOpen::Features::FeatureCollection * featureList);
};