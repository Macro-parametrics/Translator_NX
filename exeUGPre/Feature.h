#pragma once
#include "Definition.h"
#include "Part.h"

#include <NXOpen/Features_Feature.hxx>
#include <NXOpen/Features_FeatureCollection.hxx>

#include <string>
using namespace std;

class Part;
class Feature
{
public:

	Feature(Part * part, NXOpen::Features::Feature* ugFeature, NXOpen::Features::FeatureCollection * featureList);
	virtual ~Feature(void);

	Part*	GetPart() {return _pPart;}
	size_t	GetFIndex() {return _index;}
	FType	GetFType() const {return _fType;}
	string	GetName() {return _fName;}
	NXOpen::NXString GetJID() {return _featJID;}
		
	void	SetFType(FType fType) {_fType = fType; }
	void	SetName(string name) {_fName = name;}

	virtual void GetUGInfo();	// Get Features' Info. from UG
	virtual void ToTransCAD();		// Put Features' Info. Into TransCAD

protected:
	NXOpen::Features::Feature*           _ugFeature;
	NXOpen::Features::FeatureCollection* _featureList;
	Part*		                 _pPart;
	size_t		                         _index;		// Index in feature list vector
	FType                              	 _fType;
	NXOpen::NXString                     _featJID;
	string		                         _fName;		// result object name string
};