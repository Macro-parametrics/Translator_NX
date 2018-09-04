#include "stdafx.h"
#include "FSketch.h"

#include <iostream>

/** UG Post's header files **/
#include "Part.h"
#include "SKETCHItem.h"
#include "SKETCHCreate2DLine2Points.h"
#include "SKETCHCreate2DCircleCenterPoint.h"
#include "SKETCHCreate2DCircularArc.h"
//#include "SKETCHCreate2DPoint.h"
#include "SKETCHCreate2DCenterline.h"


/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>
#include <NXOpen\Sketch.hxx>
#include <NXOpen\SketchInPlaceBuilder.hxx>
#include <NXOpen\SketchCollection.hxx>
#include <NXOpen\PointCollection.hxx>
#include <NXOpen\DatumCollection.hxx>
#include <NXOpen\SelectISurface.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\DatumPlane.hxx>
#include <NXOpen\Face.hxx>
#include <NXOpen\Features_Feature.hxx>

#include <NXOpen\BodyCollection.hxx>
#include <NXOpen\Body.hxx>
#include <NXOpen\Edge.hxx>
#include <NXOpen\GeometricAnalysis_GeometricProperties.hxx>
#include <NXOpen\GeometricAnalysis_AnalysisManager.hxx>



using namespace std;


FSketch::FSketch(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart, spFeature)
{
	_Part = pPart;
	string sketchName = spFeature->GetName();
	/** Set the coordinate system : Origin, Xdir, Ydir **/
	((TransCAD::IStdSketchFeaturePtr)spFeature)->GetCoordinateSystem(
		_CSYS + 6, _CSYS + 7, _CSYS + 8,		// X direction
		_CSYS + 0, _CSYS + 1, _CSYS + 2,		// Y direction
		_CSYS + 3, _CSYS + 4, _CSYS + 5);		// Origin

	/** Set sketch plane reference **/
	_spReference = ((TransCAD::IStdSketchFeaturePtr) spFeature)->SketchPlane;

	/** Set sketch object group, Geometries **/
	_spGeometries = ((TransCAD::IStdSketchFeaturePtr) spFeature)->Geometries;
	
	Direct3D sketXdir(_CSYS[0], _CSYS[1], _CSYS[2]);
	Direct3D sketYdir(_CSYS[3], _CSYS[4], _CSYS[5]);

	_sketNormal = sketXdir.operator^(sketYdir);
}

FSketch::~FSketch(void) 
{
	Clear();
}

void FSketch::Clear()
{
	vector<SKETCHItem *>::iterator i = _sketchItemList.begin();
	
	while ( i != _sketchItemList.end() )
	{
		delete *i;
		++i;
	}

	_sketchItemList.clear();
}

void FSketch::GetInfo()
{
	// 레퍼런스 이름, 타입
	_refName	= _spReference->ReferenceeName;
	_refType	= _spReference->Type;

	// DEBUG
	//cout << "\n  Ref name:" << _spReference->Name << " Type= " << _spReference->Type << endl;
	//cout << "\n  RefFullName: " << _refName << endl << endl;


	/** Create sketch item list in Geometries **/
	SKETCHItem *pSketchItem;

	for ( int i = 1; i <= _spGeometries->Count; i++ )
	{
		TransCAD::IStdSketchGeometryPtr spItem = _spGeometries->Item[i];
		
		pSketchItem = CreateSketchItem(spItem);
	
		if ( !pSketchItem )
			cout << "**** Cannot support the sketch type in UG Post! ****" << endl;
	}

	/** Translate SketchItems in list **/
	for ( size_t i = 0; i < GetSketchItemsSize(); i++ )
	{
		SKETCHItem *pItem = GetSketchItem((int)i);

		pItem->GetInfo();	
	}
}

SKETCHItem * FSketch::CreateSketchItem(TransCAD::IStdSketchGeometryPtr spItem)
{
	SKETCHItem * pSketchItem = 0;

	//cout << "  SketchGeometry Type : " << spItem->Type << endl;
	
	switch ( spItem->Type )
	{
	case TransCAD::Line:
		pSketchItem = new SKETCHCreate2DLine2Points(this, spItem);
		//cout << " -> " << "SKETCHCreate2DLIne2Points class constructed !" << endl;
		break;
	case TransCAD::Circle:
		pSketchItem = new SKETCHCreate2DCircleCenterPoint(this, spItem);
		//cout << " -> " << "SKETCHCreate2DCircleCenterPoint class constructed !" << endl;
		break;
	case TransCAD::CircularArc:
		pSketchItem = new SKETCHCreate2DCircularArc(this, spItem);
		//cout << " -> " << "SKETCHCreate2DCircularArc class constructed !" << endl;
		break;
	//case TransCAD::ControlPoint:
	//	pSketchItem = new SKETCHCreate2DPoint(this, spItem);
	//	//cout << " -> " << "SKETCHCreate2DPoint class constructed !" << endl;
	//	break;
	case TransCAD::Centerline:
		pSketchItem = new SKETCHCreate2DCenterline(this, spItem);
		//cout << " -> " << "SKETCHCreate2DCenterline class constructed !" << endl;
		break;
	}

	if ( pSketchItem )
		_sketchItemList.push_back(pSketchItem);

	return pSketchItem;
}

void FSketch::ToUG()
{
	using namespace NXOpen;

	try
	{	
		SketchInPlaceBuilder * builder;
		builder = _Part->_nxPart->Sketches()->CreateNewSketchInPlaceBuilder(NULL);
	
		/** Sketch의 원점을 설정 **/
		Point3d origin(_CSYS[6], _CSYS[7], _CSYS[8]);
		Point * pOrigin = _Part->_nxPart->Points()->CreatePoint(origin);

		builder->SetSketchOrigin(pOrigin);

		/** 번역하는 과정에서는 기존에 존재하는 datum plane 또는 face이므로 ExistingPlane으로 설정 **/
		builder->SetPlaneOption(Sketch::PlaneOption::PlaneOptionExistingPlane);

		/** Sketch reference datum plane 또는 face를 설정 **/
		if ( _refType == 1 )
		{
			DatumPlane * sketRefPlane(NULL);

			string sketRef = g_pRefManager->GetJIDByTCName(_refName);
		
			sketRefPlane = (dynamic_cast<DatumPlane *>(_Part->_nxPart->Datums()->FindObject(sketRef)));
			builder->PlaneOrFace()->SetValue((ISurface *)sketRefPlane);
		}

		else if ( _refType == 2 )
		{	
			/** BRep name에서 어느 feature인지 확인 **/
			string featNameByBRep = g_pRefManager->ConvertToBRepFeature(_refName);
			string baseFeatJID = g_pRefManager->GetJIDByTCName(featNameByBRep);
			Features::Feature * baseFeat = _Part->_nxPart->Features()->FindObject(baseFeatJID);

			/** 선택된 feature에서 face 탐색 및 sketch reference로 설정 **/
			Point3D ori(_CSYS[6], _CSYS[7], _CSYS[8]);
			string faceJID = GetFaceJID(ori, _sketNormal);
			Face * sketRefFace;

			try
			{
				sketRefFace = dynamic_cast<Face *>(baseFeat->FindObject(faceJID));
			}

			catch ( const NXException &ex2)
			{
				string flagString = g_pRefManager->ConvertToBRepFeature2(_refName);
				
				if ( !_Part->GetFeatureByName(flagString) )
				{
					string FeatName = g_pRefManager->ConvertToBRepFeature(_refName);
			
					string FeatJID = g_pRefManager->GetJIDByTCName2(FeatName);
					Features::Feature * baseFeat = _Part->_nxPart->Features()->FindObject(FeatJID);
			
					sketRefFace = dynamic_cast<Face *>(baseFeat->FindObject(faceJID));
				}

				else if ( _Part->GetFeatureByName(flagString) )		// K2처럼 두개의 feature를 Persistent name으로 가지는 경우
				{
					string secFeatName = g_pRefManager->ConvertToBRepFeature2(_refName);
			
					string secFeatJID = g_pRefManager->GetJIDByTCName(secFeatName);
					Features::Feature * baseFeat = _Part->_nxPart->Features()->FindObject(secFeatJID);
			
					sketRefFace = dynamic_cast<Face *>(baseFeat->FindObject(faceJID));

				}
				
				else
				{
					cout << "Error code -> " << ex2.ErrorCode() << endl;
					cout << "Error message -> " << ex2.Message() << endl;
				}
			}
			
			builder->PlaneOrFace()->SetValue(sketRefFace);
		}

		Sketch * pSketch(NULL);
		NXObject * sketObject = builder->Commit();
		pSketch = (Sketch *)sketObject;

		Features::Feature * sketFeat;
		sketFeat = pSketch->Feature();

		//cout << "Sketch JID: " << pSketch->JournalIdentifier().GetUTF8Text() << endl;	// sketch name
		//cout << "Sketch Feature JID: " << sketFeat->JournalIdentifier().GetUTF8Text() << endl;	// sketch JID
		//cout << "Sketch name: " << pSketch->Name().GetUTF8Text() << endl;
	
		_sketchName = pSketch->JournalIdentifier().GetUTF8Text();
		g_pRefManager->AddNewJID(_spFeature, sketFeat->JournalIdentifier().GetUTF8Text(), "");
		//cout << "Sketch Feature JID: " << sketFeat->JournalIdentifier().GetUTF8Text() << endl;

		pSketch->Activate(Sketch::ViewReorientFalse);
	
		builder->Destroy();

		/** Loop to get sketch items information into UG sketch **/
		for ( int i = 0; (unsigned)i < GetSketchItemsSize(); i++ )
		{
			GetSketchItem(i)->ToUG();
		}

		/** Terminate a sketch **/
		pSketch->Deactivate(Sketch::ViewReorientFalse, Sketch::UpdateLevelSketchOnly);
	}

	catch ( const NXOpen::NXException &ex )
	{
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}

string FSketch::GetFaceJID(Point3D origin, Vector3D normal)
{
	using namespace NXOpen;

	string faceJID;

	vector<Face *> faceList;
	vector<Body *> bodyList;

	bodyList = _Part->GetNXBodyList();

	for(int j=0;j<bodyList.size();++j) ///////////모든 바디의 모든 면을 탐색
	{
		faceList=bodyList[j]->GetFaces();
		//cout<<bodyList[j]->JournalIdentifier().GetUTF8Text()<<endl;///////////////////////////////////////////////////////////
		for ( int i = 0; i < faceList.size(); ++i )
		{
			//cout << "Face index [ " << i<< " ] -> " << faceList[i]->JournalIdentifier().GetUTF8Text() << endl;
			Point3d targetPoint(origin.X(), origin.Y(), origin.Z());
			Vector3d targetNormal(normal.X(), normal.Y(), normal.Z());
			//cout<<faceList[i]->JournalIdentifier().GetUTF8Text()<<endl;////////////////////////////////////////////////////////
			NXObject * sourceFace = (NXObject *)faceList[i];
			GeometricAnalysis::GeometricProperties::Face faceProp;

			GeometricAnalysis::GeometricProperties * geometryObject = _Part->_nxPart->AnalysisManager()->CreateGeometricPropertiesObject();
	
			geometryObject->GetFaceProperties(sourceFace, targetPoint, &faceProp);

			Point3d p = faceProp.PositionInWcs;
			Vector3d n = faceProp.NormalInWcs;
	
			p.X = ValRnd9(p.X);
			p.Y = ValRnd9(p.Y);
			p.Z = ValRnd9(p.Z);
			
			if ( (abs(p.X - targetPoint.X)<0.00001) && (abs(p.Y - targetPoint.Y)<0.00001) && (abs(p.Z - targetPoint.Z)<0.00001) &&
				 n.X == targetNormal.X && n.Y == targetNormal.Y && n.Z == targetNormal.Z )//////오차 문제로 바꿈
			{
				cout << "Journal Identifier for sketch plane in case of BRep reference -> ";
				cout << faceList[i]->JournalIdentifier().GetUTF8Text() << endl;
				cout << p.X << ", " << p.Y << ", " << p.Z << endl;
				faceJID = faceList[i]->JournalIdentifier().GetUTF8Text();
				break;
			}
		
		}
	faceList.clear();
	}
	return faceJID;
}
