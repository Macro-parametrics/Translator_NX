#include "stdafx.h"
#include "FeatureSOLIDCreateChamfer.h"

/** UG Post's header files **/
#include "Part.h"
#include "UGReferenceManager.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>
#include <NXOpen\Features_ChamferBuilder.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\Edge.hxx>

#include <NXOpen\EdgeTangentRule.hxx>
#include <NXOpen\ScCollectorCollection.hxx>
#include <NXOpen\ScRuleFactory.hxx>

#include <NXOpen\BodyCollection.hxx>
#include <NXOpen\Body.hxx>

//#include <NXOpen\Features_Extrude.hxx>
#include <NXOpen\GeometricAnalysis_GeometricProperties.hxx>
#include <NXOpen\GeometricAnalysis_AnalysisManager.hxx>

using namespace std;

FeatureSOLIDCreateChamfer::FeatureSOLIDCreateChamfer(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart, spFeature)
{

}

FeatureSOLIDCreateChamfer::~FeatureSOLIDCreateChamfer(void) {}

void FeatureSOLIDCreateChamfer::GetInfo()
{
	TransCAD::IStdSolidChamferFeaturePtr spFeature = GetTransCADFeature();
	TransCAD::IReferencesPtr spEdges = spFeature->SelectedEdges;
	_edgeCount = spEdges->Count;
	
	_value = spFeature->Value;
	_length = spFeature->Length;
	
	for (int i = 1 ; i <= _edgeCount; ++i)
	{
		Point3D sP(spFeature->StartPosX, spFeature->StartPosY, spFeature->StartPosZ);
		Point3D eP(spFeature->EndPosX, spFeature->EndPosY, spFeature->EndPosZ);
		_startP.push_back(sP);
		_endP.push_back(eP);

		if ( sP.operator==(eP) )
			_edgeType.push_back(1);		// in case of circle edge
		else
			_edgeType.push_back(0);		// in case of line edge

		TransCAD::IReferencePtr spReference = spEdges->Item[i];
		_featureName.push_back((string)(spReference->ReferenceeName));

		cout << "	" << "Name           : " << spReference->Name << endl;
		cout << "	" << "ReferenceeName : " << spReference->ReferenceeName << endl;
		cout << "	" << "Type           : " << spReference->Type << endl;	// 1 : Brep
	
	}

	_chamferLength = _length;
	_chamferAngle = atan(_length/_value)*180/PI;
}

void FeatureSOLIDCreateChamfer::ToUG()
{
	try
	{	
		using namespace NXOpen;

		Features::ChamferBuilder * builder;
		builder = _Part->_nxPart->Features()->CreateChamferBuilder(NULL);

		ScCollector *scCollector;
		scCollector = _Part->_nxPart->ScCollectors()->CreateCollector();

		vector<Edge *> seedEdges;
		seedEdges = GetEdges();

		if ( seedEdges.size() == 1 )
		{
			EdgeTangentRule *edgeTangentRule;
			edgeTangentRule = _Part->_nxPart->ScRuleFactory()->CreateRuleEdgeTangent(seedEdges[0], NULL, false, 0.5, true, false);

			vector<NXOpen::SelectionIntentRule *> rules;
			rules.push_back(edgeTangentRule);
			scCollector->ReplaceRules(rules, false);

			builder->SetSmartCollector(scCollector);
		}
	
		else
		{
			// Multi edge chamfer 구현
		}

		builder->FirstOffsetExp()->SetRightHandSide(to_string(_chamferLength));
    
		builder->SecondOffsetExp()->SetRightHandSide(to_string(_chamferLength));
    
		builder->SetAngle(to_string(_chamferAngle));
	
		Features::Feature* chamferFeat;
		chamferFeat = builder->CommitFeature();

		builder->Destroy();

		/** Fillet feature JID 저장 **/
		g_pRefManager->AddNewJID(_spFeature, chamferFeat->JournalIdentifier().GetUTF8Text(), "");
		cout << "Extrude Feature JID: " << chamferFeat->JournalIdentifier().GetUTF8Text() << endl;
	}

	catch (const NXOpen::NXException &ex)
	{
		cout << "   Error location [ Chamfer feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
	

	
}

vector<NXOpen::Edge *> FeatureSOLIDCreateChamfer::GetEdges()
{
	using namespace NXOpen;

	vector<Edge *> edgeInBody;
	vector<Edge *> seedEdges;

	for (int i = 1; i <= _edgeCount; ++i)
	{
		BodyCollection * bodyList = _Part->_nxPart->Bodies();
		BodyCollection::iterator itr;	
	
		for ( itr = bodyList->begin(); itr != bodyList->end(); ++itr )
		{
			Body * pBody = *itr;
			
			cout << pBody->JournalIdentifier().GetUTF8Text() << endl;
			
			string featNameByBRep = g_pRefManager->ConvertToBRepFeature(_featureName[i-1]);
			string strBF = g_pRefManager->GetJIDByTCName(featNameByBRep);
			
			if ( strBF == pBody->JournalIdentifier().GetUTF8Text() )
				edgeInBody = pBody->GetEdges();
		}

		if ( _edgeType[i-1] == 0 )
		{
			int edgeNo = -1;

			for ( int eIndex = 0; eIndex < edgeInBody.size(); ++eIndex )
			{
				Edge * pEdge = edgeInBody[eIndex];
			
				Point3d sp, ep;
				pEdge->GetVertices(&sp, &ep);

				if ( sp.X == _startP[i-1].X() && sp.Y == _startP[i-1].Y() && sp.Z == _startP[i-1].Z() &&
					 ep.X == _endP[i-1].X()	 && ep.Y == _endP[i-1].Y()	&& ep.Z == _endP[i-1].Z() )
				{
					seedEdges.push_back(pEdge);
					edgeNo = eIndex;
					cout << "Edge JID = " << pEdge->JournalIdentifier().GetUTF8Text() << endl;
				}

				else if ( ep.X == _startP[i-1].X() && ep.Y == _startP[i-1].Y() && ep.Z == _startP[i-1].Z() &&
						  sp.X == _endP[i-1].X()	  && sp.Y == _endP[i-1].Y()	 && sp.Z == _endP[i-1].Z() )
				{
					seedEdges.push_back(pEdge);
					edgeNo = eIndex;
					cout << "Edge JID = " << pEdge->JournalIdentifier().GetUTF8Text() << endl;
				}
			}

			if ( edgeNo == -1 )
			{
				for ( int eIndex = 0; eIndex < edgeInBody.size(); ++eIndex )
				{
					Edge * pEdge = edgeInBody[eIndex];

					Point3d sp, ep;
					pEdge->GetVertices(&sp, &ep);
			
					double spX = ((int)(sp.X * pow(10.0, 5)) / pow(10.0, 5));
					double spY = ((int)(sp.Y * pow(10.0, 5)) / pow(10.0, 5));
					double spZ = ((int)(sp.Z * pow(10.0, 5)) / pow(10.0, 5));
					double epX = ((int)(ep.X * pow(10.0, 5)) / pow(10.0, 5));
					double epY = ((int)(ep.Y * pow(10.0, 5)) / pow(10.0, 5));
					double epZ = ((int)(ep.Z * pow(10.0, 5)) / pow(10.0, 5));

					if ( spX == _startP[i-1].X() && spY == _startP[i-1].Y() && spZ == _startP[i-1].Z() &&
						 epX == _endP[i-1].X()	&& epY == _endP[i-1].Y()	  && epZ == _endP[i-1].Z() )
					{
						seedEdges.push_back(pEdge);
						edgeNo = eIndex;
						cout << "Edge JID = " << pEdge->JournalIdentifier().GetUTF8Text() << endl;
					}

					else if ( epX == _startP[i-1].X() && epY == _startP[i-1].Y() && epZ == _startP[i-1].Z() &&
							  spX == _endP[i-1].X()	 && spY == _endP[i-1].Y()   && spZ == _endP[i-1].Z()  )
					{
						seedEdges.push_back(pEdge);
						edgeNo = eIndex;
						cout << "Edge JID = " << pEdge->JournalIdentifier().GetUTF8Text() << endl;
					}
				}
			}
		}

		else if ( _edgeType[i-1] == 1 )
		{
			for ( int eIndex = 0; eIndex < edgeInBody.size(); ++eIndex )
			{
				Edge * pEdge = edgeInBody[eIndex];

				Point3d targetPoint(_startP[0].X(), _startP[0].Y(), _startP[0].Z());
				//Point3d targetPoint(85, 0, 81);	// K4모델 targetPoint 값이 (0, 81, 85)로 나옴

				NXObject * sourceEdge = (NXObject *)pEdge;
				GeometricAnalysis::GeometricProperties::Edge edgeProp;

				GeometricAnalysis::GeometricProperties * geometryObject = _Part->_nxPart->AnalysisManager()->CreateGeometricPropertiesObject();
	
				geometryObject->GetEdgeProperties(sourceEdge, targetPoint, &edgeProp);

				Point3d p = edgeProp.PositionInWcs;

				/*p.X = ((int)(p.X * pow(10.0, 5)) / pow(10.0, 5));
				p.Y = ((int)(p.Y * pow(10.0, 5)) / pow(10.0, 5));
				p.Z = ((int)(p.Z * pow(10.0, 5)) / pow(10.0, 5));*/
				p.X = ValRnd9(p.X);
				p.Y = ValRnd9(p.Y);
				p.Z = ValRnd9(p.Z);

				if ( p.X == targetPoint.X && p.Y == targetPoint.Y && p.Z == targetPoint.Z )
				{
					cout << "Journal Identifier -> " << endl;
					cout << pEdge->JournalIdentifier().GetUTF8Text() << endl;
					cout << p.X << ", " << p.Y << ", " << p.Z << endl;
					seedEdges.push_back(pEdge);
				}
			
			}
		}
	}
			
	return seedEdges;
}