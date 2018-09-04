#include "stdafx.h"
#include "FeatureSOLIDCreateFilletConstant.h"

/** UG Post's header files **/
#include "Part.h"
#include "UGReferenceManager.h"

/** UG NXOpen header files **/
#include <NXOpen\NXException.hxx>

#include <NXOpen\Features_EdgeBlendBuilder.hxx>
#include <NXOpen\Features_FeatureCollection.hxx>
#include <NXOpen\Edge.hxx>

#include <NXOpen\EdgeMultipleSeedTangentRule.hxx>
#include <NXOpen\ScCollectorCollection.hxx>
#include <NXOpen\ScRuleFactory.hxx>

#include <NXOpen\BodyCollection.hxx>
#include <NXOpen\Body.hxx>

#include <NXOpen\GeometricAnalysis_GeometricProperties.hxx>
#include <NXOpen\GeometricAnalysis_AnalysisManager.hxx>

using namespace std;

FeatureSOLIDCreateFillet::FeatureSOLIDCreateFillet(Part * pPart, TransCAD::IFeaturePtr spFeature)
	: Feature(pPart, spFeature)
{

}

FeatureSOLIDCreateFillet::~FeatureSOLIDCreateFillet(void) {}

void FeatureSOLIDCreateFillet::GetInfo()
{
	TransCAD::IStdSolidFilletConstantFeaturePtr spFeature = GetTransCADFeature();
	_radius = spFeature->Radius;
	

	TransCAD::IReferencesPtr spReferences = spFeature->FilletEdges;
	_edgeCount = spReferences->Count;
	

	for ( int i = 1; i <= _edgeCount; ++i )
	{
		TransCAD::IReferencePtr spReference = spReferences->Item[i];
		_featureName.push_back((string)spReference->ReferenceeName);

		Point3D sP(spFeature->GetStartPosXAt(i-1), spFeature->GetStartPosYAt(i-1), spFeature->GetStartPosZAt(i-1));
		Point3D eP(spFeature->GetEndPosXAt(i-1), spFeature->GetEndPosYAt(i-1), spFeature->GetEndPosZAt(i-1));
		_startP.push_back(sP);
		_endP.push_back(eP);

		if ( sP.operator==(eP) )
			_edgeType.push_back(1);		// in case of circle edge
		else
			_edgeType.push_back(0);		// in case of line edge
	}
}

void FeatureSOLIDCreateFillet::ToUG()
{
	try
	{
		using namespace NXOpen;

		Features::EdgeBlendBuilder * builder;
		builder = _Part->_nxPart->Features()->CreateEdgeBlendBuilder(NULL);

		vector<Edge *> seedEdges;
		seedEdges = GetEdges();

		string strRadius = to_string(_radius);

		builder->SetTolerance(0.0254);
		builder->SetAllInstancesOption(false);
		builder->SetRemoveSelfIntersection(true);
		builder->SetConvexConcaveY(false);
		builder->SetRollOverSmoothEdge(true);
		builder->SetRollOntoEdge(true);
		builder->SetMoveSharpEdge(true);
		builder->SetOverlapOption(NXOpen::Features::EdgeBlendBuilder::OverlapAnyConvexityRollOver);
		builder->SetBlendOrder(NXOpen::Features::EdgeBlendBuilder::OrderOfBlendingConvexFirst);
		builder->SetSetbackOption(NXOpen::Features::EdgeBlendBuilder::SetbackSeparateFromCorner);

		ScCollector * pscCollector = _Part->_nxPart->ScCollectors()->CreateCollector();

		for ( int i = 0; i < seedEdges.size(); ++i )
		{
			vector<Edge *> seedEdge(i+1);

			for ( int j = 0; j < i+1; ++j )
			{
				seedEdge[j] = seedEdges[j];
				cout << seedEdges[j]->JournalIdentifier().GetUTF8Text() << endl;
			}

			EdgeMultipleSeedTangentRule *edgeMultipleSeedTangentrule = _Part->_nxPart->ScRuleFactory()->CreateRuleEdgeMultipleSeedTangent(seedEdge, 0.5, true);
			vector<NXOpen::SelectionIntentRule *> rules(1);
			rules[0] = edgeMultipleSeedTangentrule;
			pscCollector->ReplaceRules(rules, false);
		}

		int csIndex;
		csIndex = builder->AddChainset(pscCollector, strRadius);
	
		Features::Feature* filletFeat;
		filletFeat = builder->CommitFeature();

		builder->Destroy();

		/** Fillet feature JID 저장 **/
		g_pRefManager->AddNewJID(_spFeature, filletFeat->JournalIdentifier().GetUTF8Text(), "");
		cout << "Fillet Feature JID: " << filletFeat->JournalIdentifier().GetUTF8Text() << endl;
	}

	catch (const NXOpen::NXException &ex)
	{
		cout << "   Error location [ Fillet feature ]" << endl;
		cout << "Error code -> " << ex.ErrorCode() << endl;
		cout << "Error message -> " << ex.Message() << endl;
	}
}

vector<NXOpen::Edge *> FeatureSOLIDCreateFillet::GetEdges()
{
	using namespace NXOpen;

	vector<Edge *> edgeInBody;
	vector<Edge *> seedEdges;

	for (int i = 1; i <= _edgeCount; ++i)
	{
		int edgeNo = -1;

		vector<Body *> bodyList = _Part->GetNXBodyList();

		for ( int bIndex = 0; bIndex < bodyList.size(); ++bIndex )
		{
			string featNameByBRep = g_pRefManager->ConvertToBRepFeature(_featureName[i-1]);
			string strBF = g_pRefManager->GetJIDByTCName(featNameByBRep);
			
			if ( strBF.compare(bodyList[bIndex]->JournalIdentifier().GetUTF8Text()) == 0 )
				edgeInBody = bodyList[bIndex]->GetEdges();
		}

		if ( _edgeType[i-1] == 0 )	// line edge, count단위라서 i = 1부터 시작
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
					
					sp.X = ValRnd9(sp.X);
					sp.Y = ValRnd9(sp.Y);
					sp.Z = ValRnd9(sp.Z);
					ep.X = ValRnd9(ep.X);
					ep.Y = ValRnd9(ep.Y);
					ep.Z = ValRnd9(ep.Z);

					if ( sp.X == _startP[i-1].X() && sp.Y == _startP[i-1].Y() && sp.Z == _startP[i-1].Z() &&
						 ep.X == _endP[i-1].X()	&& ep.Y == _endP[i-1].Y()	  && ep.Z == _endP[i-1].Z() )
					{
						seedEdges.push_back(pEdge);
						edgeNo = eIndex;
						cout << "Edge JID = " << pEdge->JournalIdentifier().GetUTF8Text() << endl;
					}

					else if ( ep.X == _startP[i-1].X() && ep.Y == _startP[i-1].Y() && ep.Z == _startP[i-1].Z() &&
							  sp.X == _endP[i-1].X()	 && sp.Y == _endP[i-1].Y()   && sp.Z == _endP[i-1].Z()  )
					{
						seedEdges.push_back(pEdge);
						edgeNo = eIndex;
						cout << "Edge JID = " << pEdge->JournalIdentifier().GetUTF8Text() << endl;
					}
				}
			}
		}

		else if ( _edgeType[i-1] == 1 )	// circular edge
		{
			for ( int eIndex = 0; eIndex < edgeInBody.size(); ++eIndex )
			{
				Edge * pEdge = edgeInBody[eIndex];

				Point3d targetPoint(_startP[0].X(), _startP[0].Y(), _startP[0].Z());

				NXObject * sourceEdge = (NXObject *)pEdge;
				GeometricAnalysis::GeometricProperties::Edge edgeProp;

				GeometricAnalysis::GeometricProperties * geometryObject = _Part->_nxPart->AnalysisManager()->CreateGeometricPropertiesObject();
	
				geometryObject->GetEdgeProperties(sourceEdge, targetPoint, &edgeProp);

				Point3d p = edgeProp.PositionInWcs;

				p.X = ((int)(p.X * pow(10.0, 5)) / pow(10.0, 5));
				p.Y = ((int)(p.Y * pow(10.0, 5)) / pow(10.0, 5));
				p.Z = ((int)(p.Z * pow(10.0, 5)) / pow(10.0, 5));

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