#pragma once

#include "dllUGPost.h"
//#include "UGReferenceManager.h"
#include <string>
#include <vector>

#include <NXOpen\Session.hxx>
#include <NXOpen\Part.hxx>

class Feature;

using namespace std;

class Part
{
public:
	Part(TransCAD::IApplicationPtr tc, char * nxFileName, NXOpen::Session * nxSession);
	~Part(void);
	void clear();
	
	Feature * GetFeature(int i) { return _featureList.at(i); }
	Feature * GetFeatureByName(string name);
	size_t GetSize() const { return _featureList.size(); }
	
	NXOpen::Part * CreateNXPart();
	vector<NXOpen::Body *> GetNXBodyList();
	void SaveNXPart();
	
	int GetToUGIndex() { return _ToUGIndex; }

	void GetInfo();
	Feature * CreateFeature(TransCAD::IFeaturePtr spFeature);
	void ToUG();

public:
	TransCAD::IPartDocumentPtr	_spDocument;	// TransCAD document
	TransCAD::IPartPtr			_spPart;		// TransCAD part
	TransCAD::IFeaturesPtr		_spFeatures;	// TransCAD feature's'
	NXOpen::Session *			_nxSession;		// UG NX session
	NXOpen::Part *				_nxPart;		// UG NX part
	
	int							_ToUGIndex;

private:
	string _tcPartName;	// Member variable of TransCAD Part name
	string _nxPartName;	// Member variable of UG Part name
	vector<Feature *> _featureList;	// List of all features
};