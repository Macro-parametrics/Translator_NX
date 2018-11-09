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
    class Assembly:NewNXfile
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
        public Point3d basepoint1;
        public Point3d[] TcadCV1,TcadCV2;
        public Matrix3x3 orientation1;
        public string[] nxCompNames;
        public NXOpen.Part nxRootAssem;
        public int compCount, partCount;
        public TransCAD.IStdAssemConstraint[] GetConstraints;
        public string[] compName,ConstNames1,ConstNames2;
        public string[] partname = null;
        public string[] partFileName;
        public string[,] constraintrefnx;
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
                if (i == 0)
                {
                    ConstraintCoaxial = (TransCAD.StdAssemConstraintCoaxial)GetConstraints[i];
                    ConsPrtName[i] = ConstraintCoaxial.ConstrainedPart.Name;
                    ConsGeoName[i] = ConstraintCoaxial.ConstrainedGeometry.ReferenceeName;
                    RefPrtName[i] = ConstraintCoaxial.ReferencePart.Name;
                    RefGeoName[i] = ConstraintCoaxial.ReferenceGeometry.ReferenceeName;
                }
                else
                {
                    ConstraintIncidence = (TransCAD.StdAssemConstraintIncidence)GetConstraints[i];
                    ConsPrtName[i] = ConstraintIncidence.ConstrainedPart.Name;
                    ConsGeoName[i] = ConstraintIncidence.ConstrainedGeometry.ReferenceeName;
                    RefPrtName[i] = ConstraintIncidence.ReferencePart.Name;
                    RefGeoName[i] = ConstraintIncidence.ReferenceGeometry.ReferenceeName;

                }
                Console.WriteLine("Constraints Part" + i + " Name:: " + ConsPrtName[i]);
                Console.WriteLine("Constraints Geometry" + i + "Geometry:: " + ConsGeoName[i]);
                Console.WriteLine("Reference Part" + i + " Name:: " + RefPrtName[i]);
                Console.WriteLine("Reference Geometry" + i + "Geometry:: " + RefGeoName[i]);

            }
            int p=0,q = 0;
            ConstNames1 = new string[2];
            ConstNames2 = new string[2];
            for (int i = 0; i < 2; i++)
            {
                string partnam = tcad_PartList[i].Name;
                var getName = new GeometricNames();
                Console.WriteLine(partnam);
                int facecount = tcad_PartList[i].Solid.Faces.Count;
                
                
                //for(int p =0 ;p<4;)
                //{
                    for (int l = 0; l < 2; l++)
                    {
                        for (int k = 1; k < facecount + 1; k++)
                        {
                            string name = tcad_PartList[i].Solid.Faces[k].Name;
                            if ((ConsGeoName[l] == name)&(p<=1))
                            {
                                Console.WriteLine("The maching face in " + partnam + " is " + k + " for constraint " + l);
                                Console.WriteLine(name);

                                int numVertex = tcad_PartList[i].Solid.Faces[k].Vertices.Count;
                                Console.WriteLine("Number of Vertices in Constrained Geometry" + numVertex);
                                TcadCV1 = new Point3d[numVertex];
                                int n = 0;
                                {
                                    Console.WriteLine("Constrainted Geometry is :::");
                                    for (int m = 1; m < numVertex + 1; m++)
                                    {
                                        for (; n < numVertex; )
                                        {
                                            TransCAD.Vertex tv = tcad_PartList[i].Solid.Faces[k].Vertices[m];
                                            Console.WriteLine(" (" + tv.X + "," + tv.Y + "," + tv.z + ") ");
                                            TcadCV1[n].X = tv.X;
                                            TcadCV1[n].Y = tv.Y;
                                            TcadCV1[n].Z = tv.z;
                                            n = n + 1;
                                            break;
                                        }
                                    }
                                }
                                string fileDirect = "D:\\Macro\\A1post\\RotationPart.prt";
                                string consgeomname1 = getName.LoadName(nxSession, fileDirect, TcadCV1);
                                ConstNames1[p] = consgeomname1;
                                p = p + 1;
                                break;
                            }
                            if (RefGeoName[l] == name)
                            {
                                Console.WriteLine("The matching face in " + partnam + " is " + k + " for constraint " + l);
                                Console.WriteLine(name);
                                Console.WriteLine("Reference Geometry is :::");
                                int numVertex = tcad_PartList[i].Solid.Faces[k].Vertices.Count;
                                TcadCV2 = new Point3d[numVertex];
                                int n = 0;
                                Console.WriteLine("Number of Vertices in Reference Geometry" + numVertex);
                                {
                                    for (int m = 1; m < numVertex + 1; m++)
                                    {
                                        for (; n < numVertex; )
                                        {
                                            TransCAD.Vertex tv = tcad_PartList[i].Solid.Faces[k].Vertices[m];
                                            Console.WriteLine(" (" + tv.X + "," + tv.Y + "," + tv.z + ") ");
                                            TcadCV2[n].X = tv.X;
                                            TcadCV2[n].Y = tv.Y;
                                            TcadCV2[n].Z = tv.z;
                                            n = n + 1;
                                            break;
                                        }
                                    }
                                }
                                string fileDirect1 = "D:\\Macro\\A1post\\Body.prt";
                                string consgeomname2 = getName.LoadName(nxSession, fileDirect1, TcadCV2);
                                Console.WriteLine("Value of tCADlength " + TcadCV2.Length);
                                Console.WriteLine("Value of P:::::" + p);
                                ConstNames2[q] = consgeomname2;
                                q = q + 1;
                                break;
                            }

                        }
                    }
                //}

            }
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
                for(int j = 0; j<partCount ; j++)
                {
                    NXOpen.BasePart basePart1;
                    NXOpen.PartLoadStatus partLoadStatus,partLoadStatus2;
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
                for (int k = 1; k > -1; k--)
                    if (k == 1)
                    {
                        componentPositioner1[k] = nxSubAssem.ComponentAssembly.Positioner;
                        componentPositioner1[k].BeginAssemblyConstraints();
                        Constraint constraint1;
                        constraint1 = componentPositioner1[k].CreateConstraint(true);
                        componentConstraint1[k] = (NXOpen.Positioning.ComponentConstraint)constraint1;
                        //componentConstraint1[k].ConstraintAlignment = NXOpen.Positioning.Constraint.Alignment.ContraAlign;
                        componentConstraint1[k].ConstraintType = NXOpen.Positioning.Constraint.Type.Touch;


                        NXOpen.Face face1 = (NXOpen.Face)component01[0].FindObject(ConstNames1[k]);
                        //Face face4 = (NXOpen.Face)component01[0].FindObject(ConstNames1[1]);
                        //NXOpen.Face face1 = (NXOpen.Face)component01.FindObject("PROTO#.Features|EXTRUDE(3)|FACE 130 {(0,0,20) EXTRUDE(3)}");
                        NXOpen.Positioning.ConstraintReference constraintReference1;
                        constraintReference1 = componentConstraint1[k].CreateConstraintReference(component01[0], face1, false, false, false);


                        component01[1] = (NXOpen.Assemblies.Component)nxSubAssem.ComponentAssembly.RootComponent.FindObject("COMPONENT " + partname[1] + " 1");
                        NXOpen.Face face2 = (NXOpen.Face)component01[1].FindObject(ConstNames2[k]);
                        //Face face3 = (NXOpen.Face)component01[1].FindObject(ConstNames2[1]);
                        NXOpen.Positioning.ConstraintReference constraintReference2;
                        constraintReference2 = componentConstraint1[k].CreateConstraintReference(component01[1], face2, false, false, false);

                        /*
                        ComponentPositioner componentPositioner2;
                        componentPositioner2 = nxSubAssem.ComponentAssembly.Positioner;
                        componentPositioner2.BeginAssemblyConstraints();
                        Constraint constraint2;
                        constraint2 = componentPositioner2.CreateConstraint(true);
                        ComponentConstraint componentConstraint2 = (NXOpen.Positioning.ComponentConstraint)constraint2;
                        componentConstraint2.ConstraintType = NXOpen.Positioning.Constraint.Type.Touch;
                        ConstraintReference constraintReference3 = componentConstraint2.CreateConstraintReference(component01[1], face3, true, false, false);
                        //ConstraintReference constraintReference4 = componentConstraint2.CreateConstraintReference(component01[0], face4, true, false, false);
                        */
                        
                    }
                    else 
                    {
                        componentPositioner2[k] = nxSubAssem.ComponentAssembly.Positioner;
                        componentPositioner2[k].BeginAssemblyConstraints();
                        Constraint constraint1;
                        constraint1 = componentPositioner2[k].CreateConstraint(true);
                        componentConstraint2[k] = (NXOpen.Positioning.ComponentConstraint)constraint1;
                        componentConstraint2[k].ConstraintAlignment = NXOpen.Positioning.Constraint.Alignment.CoAlign;
                        componentConstraint2[k].ConstraintType = NXOpen.Positioning.Constraint.Type.Touch;


                        NXOpen.Face face1 = (NXOpen.Face)component01[0].FindObject(ConstNames1[k]);
                        //Line line1 = nxSubAssem.Lines.CreateFaceAxis(face1, NXOpen.SmartObject.UpdateOption.AfterModeling);
                        //Face face4 = (NXOpen.Face)component01[0].FindObject(ConstNames1[1]);
                        //NXOpen.Face face1 = (NXOpen.Face)component01.FindObject("PROTO#.Features|EXTRUDE(3)|FACE 130 {(0,0,20) EXTRUDE(3)}");
                        NXOpen.Positioning.ConstraintReference constraintReference3;
                        constraintReference3 = componentConstraint2[k].CreateConstraintReference(component01[0], face1, false, false, false);


                        component01[1] = (NXOpen.Assemblies.Component)nxSubAssem.ComponentAssembly.RootComponent.FindObject("COMPONENT " + partname[1] + " 1");
                        NXOpen.Face face2 = (NXOpen.Face)component01[1].FindObject(ConstNames2[k]);
                        //Line line2 = nxSubAssem.Lines.CreateFaceAxis(face2, NXOpen.SmartObject.UpdateOption.AfterModeling);
                        //Face face3 = (NXOpen.Face)component01[1].FindObject(ConstNames2[1]);
                        NXOpen.Positioning.ConstraintReference constraintReference4;
                        constraintReference4 = componentConstraint2[k].CreateConstraintReference(component01[1], face2, false, false, false);
                        constraintReference4.SetFixHint(true);
 
                    }
            }
            PartSaveStatus fileSave;
            fileSave = nxRootAssem.Save(BasePart.SaveComponents.True, BasePart.CloseAfterSave.False);
        }     
    }
}