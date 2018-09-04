#pragma once

#include <vector>
#include <string>
#include "dllUGPost.h"
#include "Definition.h"

/** TransCAD feature type - feature name **/
typedef struct _featNameMap
{
	int _id;
	TransCAD::FeatureType _type;
	std::string _name;
} NAME_MAP;

typedef struct _JIDMap
{
	int _id;
	std::string _name;
	std::string _fJID;	// first JID
	std::string _sJID;	// second JID
} JID_MAP;


class UGReferenceManager
{
public:
	UGReferenceManager();
	~UGReferenceManager(void);

	/** Part.cpp에서 함수 콜 --> Feature정보를 저장 **/
	int AddNewFeature(TransCAD::IFeaturePtr spFeature);
	int AddNewJID(TransCAD::IFeaturePtr spFeature, std::string fJID, std::string sJID);
	int AddSecJID(TransCAD::IFeaturePtr spFeature, std::string secJID);
	
	int GetFeatureTypeByTCName(std::string strTCName, TransCAD::FeatureType &type);
	int GetFeatureIDByTCName(std::string strTCName, int &id);
	std::string GetJIDByTCName(std::string strTCName);
	std::string GetJIDByTCName2(std::string strTCName);

	std::string ConvertToBRepFeature(std::string strPersistentName);
	std::string ConvertToBRepFeature2(std::string strPersistentName);
	

private:
	int m_id;
	std::vector<NAME_MAP> _listNameMap;
	std::vector<JID_MAP> _listJIDMap;
};