using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices; //For Marshal

using NXOpen;
using NXOpen.Assemblies;
using NXOpen.Positioning;
using NXOpen.UF;

namespace NX_Code_Macro
{
    class Assembly : NewNXfile
    {
        #region
        public TransCAD.Application tApp = null;
        public TransCAD.AssemDocument tAssemDoc = null;
        public TransCAD.Assem tAssem = null;
        public TransCAD.Component tComp = null;
        public TransCAD.StdAssemConstraints[] tConstraints = null;
        public TransCAD.Part[] tcad_PartList;
        public Component rootComponent;
        public Component[] nxComponents;
        public ComponentPositioner nxPositioner;
        public Constraint[] nxConstraints;
        public int numComponents;
        public Point3d basepoint1, matchingPoint;
        public Point3d[] TcadCV1, TcadCV01, TcadCV2, TcadCV02, CircularFace1, CircularFace2;
        public Matrix3x3 orientation1;
        public string[] nxCompNames;
        public NXOpen.Part nxRootAssem;
        public int compCount, partCount;
        public TransCAD.IStdAssemConstraint[] GetConstraints;
        public string[] compName, ConstNames1, ConstNames2, ConstNamesOrdered;
        public string[] partname = null;
        public string[] partFileName, EdgeNames, constNamesTemp;
        public string[,] constraintrefnx;
        public string name;
        public List<string> ConsNames, PartNamestoNX;
        Dictionary<int, Point3d[]> partOne = new Dictionary<int, Point3d[]>();
        Dictionary<int, Point3d[]> partTwo = new Dictionary<int, Point3d[]>();

        #endregion
        public bool InitializeTransCAD()
        {
            try
            {
                tApp = (TransCAD.Application)Marshal.GetActiveObject("TransCAD.Application");
            }
            catch
            {
                tApp = (TransCAD.Application)Activator.CreateInstance(Type.GetTypeFromProgID("TransCAD.Application"));
            }
            if (tApp == null)
                return false;

            tApp.Visible = true;
            return true;

        }
        public void loadAssemblyInfo()
        {
            tAssemDoc = (TransCAD.AssemDocument)tApp.ActiveDocument;
            tAssem = tAssemDoc.Assem;
            //Count Component Number
            compCount = tAssem.GetSize();
            compName = new string[compCount];
            for (int i = 0; i < compCount; i++)
            {

                TransCAD.Component tComp = tAssem.GetComponent(i);
                compName[i] = tComp.get_Name();
                Console.WriteLine("Name of Component[ " + i + " ] : " + tComp.get_Name());

                //Count Part Number in Component
                partCount = tComp.GetSize();
                partname = new string[partCount];
                if (compName[i] == "Default SubAssembly")
                {
                    continue;
                }
                tcad_PartList = new TransCAD.Part[partCount];
                partFileName = new string[partCount];
                NXObject[] nxGeom = new NXObject[partCount];
                for (int j = 0; j < partCount; j++)
                {
                    TransCAD.Part tPart = tComp.GetPart(j);
                    partname[j] = tPart.Name;
                    tcad_PartList[j] = tPart;
                    int facecount = tPart.Solid.Faces.Count;
                    string facenm = tPart.Solid.Faces[1].Name;
                    Console.WriteLine(facenm);
                    Console.WriteLine(" Name of Part[ " + j + " ] : " + tPart.Name);
                    partFileName[j] = tPart.Name;
                }



            }


        }
        public void FromTransCAD(Session nxSession)
        {
            Console.WriteLine("Number fo Const" + tAssem.Constraints.Count);
            TransCAD.IStdAssemConstraint[] GetConstraints = new TransCAD.IStdAssemConstraint[tAssem.Constraints.Count];
            TransCAD.StdAssemConstraintCoaxial ConstraintCoaxial;
            TransCAD.StdAssemConstraintIncidence ConstraintIncidence;
            string[] ConsPrtName = new string[tAssem.Constraints.Count];
            string[] ConsGeoName = new string[tAssem.Constraints.Count];
            string[] RefPrtName = new string[tAssem.Constraints.Count];
            string[] RefGeoName = new string[tAssem.Constraints.Count];


            for (int i = 0; i < tAssem.Constraints.Count; i++)
            {
                GetConstraints[i] = tAssem.Constraints.get_Constraint(i + 1);
                if (GetConstraints[i].Name == "Coaxial")
                {
                    ConstraintCoaxial = (TransCAD.StdAssemConstraintCoaxial)GetConstraints[i];
                    ConsPrtName[i] = ConstraintCoaxial.ConstrainedPart.Name;
                    if (ConstraintCoaxial.ConstrainedGeometry.ReferenceeName.Contains("#"))
                    {
                        string tempstr = ConstraintCoaxial.ConstrainedGeometry.ReferenceeName;
                        Console.WriteLine("Constains #");
                        int z = tempstr.IndexOf('#');
                        if (z >= 0) tempstr = tempstr.Substring(z + 1);
                        ConsGeoName[i] = tempstr;
                    }
                    else
                    {
                        ConsGeoName[i] = ConstraintCoaxial.ConstrainedGeometry.ReferenceeName;
                    }

                    RefPrtName[i] = ConstraintCoaxial.ReferencePart.Name;
                    if (ConstraintCoaxial.ReferenceGeometry.ReferenceeName.Contains("#"))
                    {
                        string tempstr = ConstraintCoaxial.ReferenceGeometry.ReferenceeName;
                        Console.WriteLine("Constains #");
                        int z = tempstr.IndexOf('#');
                        if (z >= 0) tempstr = tempstr.Substring(z + 1);
                        RefGeoName[i] = tempstr;
                    }
                    else
                    {

                        RefGeoName[i] = ConstraintCoaxial.ReferenceGeometry.ReferenceeName;
                    }
                }
                else if (GetConstraints[i].Name == "Incidence")
                {
                    ConstraintIncidence = (TransCAD.StdAssemConstraintIncidence)GetConstraints[i];
                    if (ConstraintIncidence.ConstrainedGeometry.ReferenceeName.Contains("#"))
                    {
                        Console.WriteLine("Constains #");
                    }
                    ConsPrtName[i] = ConstraintIncidence.ConstrainedPart.Name;
                    if (ConstraintIncidence.ReferenceGeometry.ReferenceeName.Contains("#"))
                    {
                        Console.WriteLine("Constains #");
                    }
                    ConsGeoName[i] = ConstraintIncidence.ConstrainedGeometry.ReferenceeName;
                    RefPrtName[i] = ConstraintIncidence.ReferencePart.Name;
                    RefGeoName[i] = ConstraintIncidence.ReferenceGeometry.ReferenceeName;

                }
                Console.WriteLine("Constraints Part" + i + " Name:: " + ConsPrtName[i]);
                Console.WriteLine("Constraints Geometry" + i + "Geometry:: " + ConsGeoName[i]);
                Console.WriteLine("Reference Part" + i + " Name:: " + RefPrtName[i]);
                Console.WriteLine("Reference Geometry" + i + "Geometry:: " + RefGeoName[i]);

            }
            int p = 0, q = 0;
            ConstNames1 = new string[2];
            ConstNames2 = new string[2];
            constNamesTemp = new string[2];
            ConsNames = new List<string>();
            PartNamestoNX = new List<string>();
            for (int i = 0; i < 2; i++)
            {
                var filedir = new NewNXfile();
                string constraintName = GetConstraints[i].Name;
                constNamesTemp[i] = constraintName;

                string partnam = tcad_PartList[i].Name;
                double ox, oy, oz, xx, xy, xz, yx, yy, yz;
                tcad_PartList[i].GetPlacement(out ox, out oy, out oz, out xx, out xy, out xz, out yx, out yy, out yz);
                Point3d tcad_Position = new Point3d(ox, oy, oz);
                Point3d tcad_X_Orientation = new Point3d(xx, xy, xz);
                Point3d tcad_Y_Orientation = new Point3d(yx, yy, yz);
                var getName = new GeometricNames();
                Console.WriteLine(partnam);
                int facecount = tcad_PartList[i].Solid.Faces.Count;
                string[] partname = new string[tcad_PartList[i].Solid.Faces.Count + 1];

                for (int i1 = 1; i1 <= facecount; i1++)
                {
                    partname[i1] = tcad_PartList[i].Solid.Faces[i1].Name;
                }
                string[] tempname = partname;
                for (int l = 0; l < 2; l++)
                {
                    for (int k = 1; k < facecount + 1; k++)
                    {
                        name = tcad_PartList[i].Solid.Faces[k].Name;

                        if ((ConsGeoName[l].Contains(name)) & (p <= 1))
                        {
                            Console.WriteLine("The maching face in " + partnam + " is " + k + " for constraint " + l);
                            Console.WriteLine(name);

                            int numVertex = tcad_PartList[i].Solid.Faces[k].Vertices.Count;
                            Console.WriteLine("Number of Vertices in Constrained Geometry" + numVertex);
                            TcadCV1 = new Point3d[numVertex];
                            TcadCV01 = new Point3d[numVertex];
                            int n = 0;
                            {
                                Console.WriteLine("Constrainted Geometry is :::");
                                for (int m = 1; m < numVertex + 1; m++)
                                {
                                    for (; n < numVertex; )
                                    {
                                        Point3d tempTV;
                                        TransCAD.Vertex tv = tcad_PartList[i].Solid.Faces[k].Vertices[m];
                                        Console.WriteLine(" (" + tv.X + "," + tv.Y + "," + tv.z + ") ");
                                        tempTV.X = tv.X;
                                        tempTV.Y = tv.Y;
                                        tempTV.Z = tv.z;
                                        TcadCV01[n] = tempTV;
                                        TcadCV1[n] = OrientedTranslated(tempTV, tcad_Position, tcad_X_Orientation, tcad_Y_Orientation);
                                        //TcadCV1[n].X = tv.X;
                                        //TcadCV1[n].Y = tv.Y;
                                        //TcadCV1[n].Z = tv.z;

                                        n = n + 1;
                                        break;
                                    }
                                }
                            }
                            partOne.Add(p, TcadCV1);
                            string fileDirect = "D:\\Macro\\A1post\\RotationPart.prt";
                            string consgeomname1 = getName.LoadName(nxSession, TcadCV01, filedir.partDir(partnam), "Face");
                            //ConstNames1[p] = consgeomname1;
                            p = p + 1;
                            ConsNames.Add(consgeomname1);
                            PartNamestoNX.Add(partnam);
                            break;
                        }
                        if (RefGeoName[l].Contains(name))
                        {
                            Console.WriteLine("The matching face in " + partnam + " is " + k + " for constraint " + l);
                            Console.WriteLine(name);
                            Console.WriteLine("Reference Geometry is :::");
                            int numVertex = tcad_PartList[i].Solid.Faces[k].Vertices.Count;
                            TcadCV2 = new Point3d[numVertex];
                            TcadCV02 = new Point3d[numVertex];
                            int n = 0;
                            Console.WriteLine("Number of Vertices in Reference Geometry" + numVertex);
                            {
                                for (int m = 1; m < numVertex + 1; m++)
                                {
                                    for (; n < numVertex; )
                                    {
                                        Point3d tempTV;
                                        TransCAD.Vertex tv = tcad_PartList[i].Solid.Faces[k].Vertices[m];
                                        Console.WriteLine(" (" + tv.X + "," + tv.Y + "," + tv.z + ") ");
                                        tempTV.X = tv.X;
                                        tempTV.Y = tv.Y;
                                        tempTV.Z = tv.z;
                                        TcadCV02[n] = tempTV;
                                        TcadCV2[n] = OrientedTranslated(tempTV, tcad_Position, tcad_X_Orientation, tcad_Y_Orientation);
                                        //TcadCV2[n].X = tv.X;
                                        //TcadCV2[n].Y = tv.Y;
                                        //TcadCV2[n].Z = tv.z;
                                        n = n + 1;
                                        break;
                                    }
                                }
                            }
                            partTwo.Add(q, TcadCV2);
                            string fileDirect1 = "D:\\Macro\\A1post\\Body.prt";
                            string consgeomname2 = getName.LoadName(nxSession, TcadCV02, filedir.partDir(partnam), "Face");
                            Console.WriteLine("Value of tCADlength " + TcadCV2.Length);
                            Console.WriteLine("Value of P:::::" + p);
                            //ConstNames2[q] = consgeomname2;
                            q = q + 1;
                            ConsNames.Add(consgeomname2);
                            PartNamestoNX.Add(partnam);
                            break;
                        }

                    }
                }
            }
            p = 0;
            q = 0;
            for (int z1 = 0; z1 < ConsNames.Count; z1++)
            {

                for (int z2 = 0; z2 < partname.Length; z2++)
                {

                    if (PartNamestoNX[z1].Contains(partname[z2]))
                    {
                        ConstNames1[p] = ConsNames[z1];
                        p = p + 1;
                        break;
                    }
                    else
                    {
                        ConstNames2[q] = ConsNames[z1];
                        q = q + 1;
                        break;
                    }
                }
            }
            Console.WriteLine("");
        }
        public void FindtheEdgeName(Session nxSession)
        {
            var getName = new GeometricNames();
            for (int i = 0; i < 2; i++)
            {
                if (partOne[i].Length == 2)
                {
                    CircularFace1 = partOne[i];
                }
                if (partTwo[i].Length == 2)
                {
                    CircularFace2 = partTwo[i];
                }
            }

            Point3d[] point1 = new Point3d[1];
            point1[0] = toOne(CircularFace1, partOne);
            Point3d[] point2 = new Point3d[1];
            point2[0] = toOne(CircularFace2, partTwo);
            var filedir = new NewNXfile();
            filedir.partDir("RotationPart");
            filedir.partDir("Body");
            string fileDirect1 = "D:\\Macro\\A1post\\RotationPart.prt";
            string consgeomname1 = getName.LoadName(nxSession, point1, filedir.partDir("RotationPart"), "Edge");
            string fileDirect2 = "D:\\Macro\\A1post\\Body.prt";
            string consgeomname2 = getName.LoadName(nxSession, point2, filedir.partDir("Body"), "Edge");

            EdgeNames = new string[2];
            EdgeNames[0] = consgeomname1;
            EdgeNames[1] = consgeomname2;

        }
        private Point3d toOne(Point3d[] CircularFace1, Dictionary<int, Point3d[]> partOne)
        {
            for (int j = 0; j < 2; j++)
            {
                if (partOne[j].Length != 2)
                {
                    Point3d[] PlanarFace = partOne[j];
                    for (int k = 0; k < 2; k++)
                        for (int l = 0; l < partOne[j].Length; l++)
                        {
                            if ((CircularFace1[k].X == PlanarFace[l].X) && (CircularFace1[k].Y == PlanarFace[l].Y) && (CircularFace1[k].Z == PlanarFace[l].Z))
                            {
                                matchingPoint = CircularFace1[k];
                                break;
                            }
                        }
                }
            }
            return matchingPoint;
        }
        public void NXRootFile(Session nxSession, NewNXfile fileDir)
        {
            NXOpen.FileNew nxRootCompFile = nxSession.Parts.FileNew();
            fileDir.NXfile(nxRootCompFile, fileDir.rootDir());
            NXOpen.NXObject nXObject_RootComp = nxRootCompFile.Commit();
            nxRootCompFile.MakeDisplayedPart = true;
            Part nxRootAssem = nxSession.Parts.Work;
            nxRootCompFile.Destroy();
        }
        public void ToNXFile(Session nxSession)
        {
            NXOpen.Part nxSubAssem;
            var newFile = new NewNXfile();
            var fileDir = new NewNXfile();
            for (int i = 0; i < compCount; i++)
            {
                nxRootAssem = nxSession.Parts.Work;
                if (compName[i] == "Default SubAssembly")
                {
                    continue;
                }
                NXOpen.FileNew compFile = nxSession.Parts.FileNew();
                newFile.NXfile(compFile, fileDir.subassemdir(compName[i]));
                compFile.MakeDisplayedPart = false;
                NXOpen.Assemblies.CreateNewComponentBuilder createNewComponentBuilder1;
                createNewComponentBuilder1 = nxRootAssem.AssemblyManager.CreateNewComponentBuilder();
                createNewComponentBuilder1.NewComponentName = compName[i];
                createNewComponentBuilder1.ReferenceSet = NXOpen.Assemblies.CreateNewComponentBuilder.ComponentReferenceSetType.EntirePartOnly;
                createNewComponentBuilder1.ReferenceSetName = "Entire Part";
                createNewComponentBuilder1.NewFile = compFile;
                NXOpen.NXObject nXObject_SubAssem = createNewComponentBuilder1.Commit();
                createNewComponentBuilder1.Destroy();
                nxSubAssem = (NXOpen.Part)nxSession.Parts.FindObject(compName[i]);
                NXObject[] nxGeom = new NXObject[partCount];
                for (int j = 0; j < partCount; j++)
                {
                    NXOpen.BasePart basePart1;
                    NXOpen.PartLoadStatus partLoadStatus, partLoadStatus2;
                    basePart1 = nxSession.Parts.OpenBase(fileDir.partdir(partFileName[j]), out partLoadStatus);
                    partLoadStatus.Dispose();
                    basepoint1 = new NXOpen.Point3d(0.0, 0.0, 0.0);
                    orientation1 = new NXOpen.Matrix3x3();
                    orientation1.Xx = 1.0;
                    orientation1.Xy = 0.0;
                    orientation1.Xz = 0.0;
                    orientation1.Yx = 0.0;
                    orientation1.Yy = 1.0;
                    orientation1.Yz = 0.0;
                    orientation1.Zx = 0.0;
                    orientation1.Zy = 0.0;
                    orientation1.Zz = 1.0;
                    NXOpen.Assemblies.Component component1;
                    component1 = nxRootAssem.ComponentAssembly.AddComponent(fileDir.partdir(partFileName[j]), "MODEL", "A1-1", basepoint1, orientation1, -1, out partLoadStatus2, true);
                    partLoadStatus2.Dispose();
                    NXOpen.Assemblies.Component[] origComponents1 = new NXOpen.Assemblies.Component[1];
                    origComponents1[0] = component1;
                    NXOpen.Assemblies.Component component2 = (NXOpen.Assemblies.Component)nXObject_SubAssem;
                    NXOpen.Assemblies.Component[] newComponents1;
                    NXOpen.ErrorList errorList1;
                    nxSubAssem.ComponentAssembly.RestructureComponents(origComponents1, component2, true, out newComponents1, out errorList1);
                    errorList1.Dispose();
                    nxGeom[i] = origComponents1[0];
                    string reef = nxGeom[i].OwningComponent.DisplayName;
                    Console.WriteLine("*************************" + reef);
                }
                /*Working Information with Hardcoded information;*/
                ComponentPositioner[] tempcomponentPositioner1 = new ComponentPositioner[2];
                ComponentConstraint[] tempcomponentConstraint1 = new ComponentConstraint[2];
                ComponentPositioner[] componentPositioner1 = new ComponentPositioner[2];
                ComponentPositioner[] componentPositioner2 = new ComponentPositioner[2];
                ComponentConstraint[] componentConstraint1 = new ComponentConstraint[2];
                ComponentConstraint[] componentConstraint2 = new ComponentConstraint[2];
                NXOpen.Assemblies.Component[] component01 = new Component[2];
                NXOpen.Assemblies.Component[] component02 = new Component[2];
                for (int m = 0; m < 2; m++)
                {
                    component01[m] = (NXOpen.Assemblies.Component)nxSubAssem.ComponentAssembly.RootComponent.FindObject("COMPONENT " + partname[m] + " 1");
                    //NXOpen.Assemblies.Component component02 = (NXOpen.Assemblies.Component)nxSubAssem.ComponentAssembly.RootComponent.FindObject("COMPONENT " + partname[0] + " 1");
                }
                /*
                tempcomponentPositioner1[0] = nxSubAssem.ComponentAssembly.Positioner;
                tempcomponentPositioner1[0].BeginAssemblyConstraints();
                Constraint tempconstraint1;
                tempconstraint1 = tempcomponentPositioner1[0].CreateConstraint(true);
                tempcomponentConstraint1[0] = (NXOpen.Positioning.ComponentConstraint)tempconstraint1;
                tempcomponentConstraint1[0].ConstraintType = NXOpen.Positioning.Constraint.Type.Concentric;
                Edge edge1 = (NXOpen.Edge)component01[0].FindObject(EdgeNames[0]);
                ConstraintReference ConsRef1 = tempcomponentConstraint1[0].CreateConstraintReference(component01[0],edge1,false,false,false);
                Edge edge2 = (NXOpen.Edge)component01[1].FindObject(EdgeNames[1]);
                ConstraintReference ConsRef2 = tempcomponentConstraint1[0].CreateConstraintReference(component01[1], edge2, false, false, false);
                */


                for (int k = 0; k < 2; k++)
                    if (constNamesTemp[k].Contains("Coaxial"))
                    {
                        componentPositioner1[k] = nxSubAssem.ComponentAssembly.Positioner;
                        componentPositioner1[k].BeginAssemblyConstraints();
                        Constraint constraint1;
                        constraint1 = componentPositioner1[k].CreateConstraint(true);
                        componentConstraint1[k] = (NXOpen.Positioning.ComponentConstraint)constraint1;
                        //componentConstraint1[k].ConstraintAlignment = NXOpen.Positioning.Constraint.Alignment.ContraAlign;
                        componentConstraint1[k].ConstraintType = NXOpen.Positioning.Constraint.Type.Fit;
                        NXOpen.Face face1 = (NXOpen.Face)component01[0].FindObject(ConstNames1[k]);
                        NXOpen.Positioning.ConstraintReference constraintReference1;
                        constraintReference1 = componentConstraint1[k].CreateConstraintReference(component01[0], face1, false, false, false);
                        component01[1] = (NXOpen.Assemblies.Component)nxSubAssem.ComponentAssembly.RootComponent.FindObject("COMPONENT " + partname[1] + " 1");
                        NXOpen.Face face2 = (NXOpen.Face)component01[1].FindObject(ConstNames2[k]);
                        //Face face3 = (NXOpen.Face)component01[1].FindObject(ConstNames2[1]);
                        NXOpen.Positioning.ConstraintReference constraintReference2;
                        constraintReference2 = componentConstraint1[k].CreateConstraintReference(component01[1], face2, false, false, false);

                    }
                    else
                    {
                        componentPositioner2[k] = nxSubAssem.ComponentAssembly.Positioner;
                        componentPositioner2[k].BeginAssemblyConstraints();
                        Constraint constraint1;
                        constraint1 = componentPositioner2[k].CreateConstraint(true);
                        componentConstraint2[k] = (NXOpen.Positioning.ComponentConstraint)constraint1;
                        //componentConstraint2[k].ConstraintAlignment = NXOpen.Positioning.Constraint.Alignment.InferAlign;
                        componentConstraint2[k].ConstraintType = NXOpen.Positioning.Constraint.Type.Touch;
                        NXOpen.Face face1 = (NXOpen.Face)component01[0].FindObject(ConstNames1[k]);
                        NXOpen.Positioning.ConstraintReference constraintReference3;
                        constraintReference3 = componentConstraint2[k].CreateConstraintReference(component01[0], face1, false, false, false);
                        component01[1] = (NXOpen.Assemblies.Component)nxSubAssem.ComponentAssembly.RootComponent.FindObject("COMPONENT " + partname[1] + " 1");
                        NXOpen.Face face2 = (NXOpen.Face)component01[1].FindObject(ConstNames2[k]);
                        NXOpen.Positioning.ConstraintReference constraintReference4;
                        constraintReference4 = componentConstraint2[k].CreateConstraintReference(component01[1], face2, false, false, false);
                        constraintReference4.SetFixHint(true);
                    }
                break;
            }
            PartSaveStatus fileSave;
            fileSave = nxRootAssem.Save(BasePart.SaveComponents.True, BasePart.CloseAfterSave.False);
        }
        private Point3d OrientedTranslated(Point3d vertex, Point3d Orientation, Point3d XVectors, Point3d YVectors)
        {
            Point3d Value = new Point3d(0, 0, 0);
            Point3d c, d;

            double adispX = vertex.X + Orientation.X;
            double adispY = vertex.Y + Orientation.Y;
            double adispZ = vertex.Z;
            double bdispX = vertex.X - Orientation.X;
            double bdispY = vertex.Y - Orientation.Y;
            double bdispZ = vertex.Z;
            if (Math.Abs(vertex.Z) == Math.Abs(Orientation.Z))
            {
                if (vertex.Z + Orientation.Z == 0)
                {
                    adispZ = vertex.Z + Orientation.Z;
                }
                else
                {
                    adispZ = vertex.Z + Orientation.Z;
                }
            }

            c.X = Math.Round(adispX * XVectors.X + adispY * XVectors.Y + adispZ * XVectors.Z, 6);
            c.Y = Math.Round(adispX * YVectors.X + adispY * YVectors.Y + adispZ * YVectors.Z, 6);
            c.Z = Math.Round(adispZ);

            d.X = Math.Round(bdispX * XVectors.X + bdispY * XVectors.Y + bdispZ * XVectors.Z, 6);
            d.Y = Math.Round(bdispX * YVectors.X + bdispY * YVectors.Y + bdispZ * YVectors.Z, 6);
            d.Z = Math.Round(bdispZ);

            Value = c;


            return Value;
        }
    }
}