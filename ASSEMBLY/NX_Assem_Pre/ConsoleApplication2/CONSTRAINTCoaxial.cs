using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using NXOpen; 
using NXOpen.Assemblies;
using NXOpen.Positioning;
using NXOpen.UF; 

namespace ConsoleApplication2
{
    class CONSTRAINTCoaxial : CONSTRAINT
    {
        public CONSTRAINTCoaxial(NXOpen.Positioning.Constraint nxConstraint, ASSEMBLY assemManager)
            : base(nxConstraint, assemManager)
        {
        }
        public override void getNXInfo()
        {
            for (int i = 0; i < nx_RefGeoms.Length; i++)
            {
                if (nx_RefGeoms[i].ToString().Contains("Face"))                
                {
                    Face tempconst = (Face)nx_RefGeoms[i];
                    Edge[] circularedges = tempconst.GetEdges();
                    Edge circularEdge = circularedges[0];
                    Edge.EdgeType edgeType = circularEdge.SolidEdgeType;
                    Point3d a, b;
                    circularEdge.GetVertices(out a, out b); //a, b are same since circular edge starts and ends at same position

                    convertWorldToLocal worldToLocalConv = new convertWorldToLocal();
                    Point3d tcad_Coord = worldToLocalConv.calculateLocalCoord(a, refGeomPartPositions[i], refGeomPartOrientations[i]);

                    tcad_RefGeoms[i] = tcad_refGeomRelatedParts[i].SelectCurveBy3Points(tcad_Coord.X, tcad_Coord.Y, tcad_Coord.Z, tcad_Coord.X, tcad_Coord.Y, tcad_Coord.Z, tcad_Coord.X, tcad_Coord.Y, tcad_Coord.Z);
                    if (tcad_RefGeoms[i] == null)
                    {
                        tcad_RefGeoms[i] = tcad_refGeomRelatedParts[i].SelectCurveBy3Points(-tcad_Coord.X, tcad_Coord.Y, tcad_Coord.Z, -tcad_Coord.X, tcad_Coord.Y, tcad_Coord.Z, -tcad_Coord.X, tcad_Coord.Y, tcad_Coord.Z);
                    }
                }
            }
        }
        public override void toTransCAD()
        {
            assemManager_.tConstraints.AddNewAssemblyCoaxialConstraint("Coaxial", tcad_refGeomRelatedParts[0], tcad_RefGeoms[0], tcad_refGeomRelatedParts[1], tcad_RefGeoms[1]);
            assemManager_.tAssemDoc.Update();
        }
    }
}
