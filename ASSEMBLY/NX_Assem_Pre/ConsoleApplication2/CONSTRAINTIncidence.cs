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
    class CONSTRAINTIncidence : CONSTRAINT
    {
        List<Point3d> NXrefPoints1=new List<Point3d>();
        List<Point3d> NXrefPoints2 = new List<Point3d>();

        findGeometry pointMatch = new findGeometry();
        Constraint nxConst;
        string ConstType;

        public CONSTRAINTIncidence(NXOpen.Positioning.Constraint nxConstraint, ASSEMBLY assemManager)
            : base(nxConstraint, assemManager)
        {
            nxConst = nxConstraint;
        }
        public override void getNXInfo()
        {
            for (int i = 0; i < nx_RefGeoms.Length; i++)
            {
                if (nx_RefGeoms[i].ToString().Contains("Face"))
                {
                    Face planarFace = (Face)nx_RefGeoms[i];
                    Face.FaceType faceType = planarFace.SolidFaceType;
                    Edge[] faceEdges = planarFace.GetEdges();
                    Point3d[] v1 = new Point3d[planarFace.GetEdges().Count()];
                    Point3d[] v2 = new Point3d[planarFace.GetEdges().Count()];
                    for (int j = 0; j < planarFace.GetEdges().Count(); j++)
                    {
                        faceEdges[j].GetVertices(out v1[j], out v2[j]);
                        convertWorldToLocal LocalConv = new convertWorldToLocal();
                        Point3d localPoint1 = LocalConv.calculateLocalCoord(v1[j], refGeomPartPositions[i], refGeomPartOrientations[i]);
                        Point3d localPoint2 = LocalConv.calculateLocalCoord(v2[j], refGeomPartPositions[i], refGeomPartOrientations[i]);

                        switch (i)
                        {
                            case 0:
                                NXrefPoints1.Add(localPoint1);
                                break;
                            case 1:
                                NXrefPoints2.Add(localPoint1);
                                break;
                        }      
                    }
                }
            }
            ConstType = nxConst.ConstraintType.ToString();
        }
        public override void toTransCAD()
        {
            string[] relTCFace=new string[2];
            int countMatch;
            TransCAD.Reference[] refGeom = new TransCAD.Reference[2];

            for (int i = 0; i < assemManager_.numComponents; i++)
            {
                int numFaces = assemManager_.tcad_PartList[i].Solid.Faces.Count;

                for (int j = 1; j <= numFaces; j++)
                {
                    int numVertices = assemManager_.tcad_PartList[i].Solid.Faces[j].Vertices.Count;
                    string faceName = assemManager_.tcad_PartList[i].Solid.Faces[j].Name;
                    Console.WriteLine(faceName);
                    countMatch = 0;
                    for (int k = 1; k <= numVertices; k++)
                    {
                        TransCAD.Vertex tVtx=assemManager_.tcad_PartList[i].Solid.Faces[j].Vertices[k];

                        Console.Write(" (" + tVtx.X + "," + tVtx.Y + "," +tVtx.z+ ") ");
                        if (pointMatch.vertexMatching(tVtx, NXrefPoints1, NXrefPoints2))
                            countMatch++;

                        if (countMatch == 3)
                        {
                            relTCFace[i] = assemManager_.tcad_PartList[i].Solid.Faces[j].Name;
                            break;
                        }
                    }
                    Console.WriteLine();
                }
                refGeom[i] = assemManager_.tcad_PartList[i].SelectBrepByName(relTCFace[i]);
            }
            if (ConstType == "Touch")
                assemManager_.tConstraints.AddNewAssemblyIncidenceConstraint("Incidence", assemManager_.tcad_PartList[0], refGeom[0], assemManager_.tcad_PartList[1], refGeom[1], TransCAD.StdAssemblyIncidenceType.Opposite);
            else
                assemManager_.tConstraints.AddNewAssemblyIncidenceConstraint("Incidence", assemManager_.tcad_PartList[0], refGeom[0], assemManager_.tcad_PartList[1], refGeom[1], TransCAD.StdAssemblyIncidenceType.Same);
            
            assemManager_.tAssemDoc.Update();

            //assemManager_.tcad_PartList[i];
            //assemManager_.tConstraints.AddNewAssemblyIncidenceConstraint("Incidence", tcad_refGeomRelatedParts[0], tcad_RefGeoms[0], tcad_refGeomRelatedParts[1], tcad_RefGeoms[1], TransCAD.StdAssemblyIncidenceType.Opposite);
            //assemManager_.tAssemDoc.Update();
        }
       
    }
}
