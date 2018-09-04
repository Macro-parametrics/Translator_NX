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

namespace ConsoleApplication2
{
    class ASSEMBLY
    {
        // TransCAD 변수
        public TransCAD.Application tApp = null;
        public TransCAD.Documents tDocs = null;
        public TransCAD.AssemDocument tAssemDoc = null;
        public TransCAD.Assem tAssem = null;
        public TransCAD.Component tComp = null;
        public TransCAD.StdAssemConstraints tConstraints = null;
        public TransCAD.Part[] tcad_PartList;
        public Component rootComponent;
        public Component[] nxComponents;
        public ComponentPositioner nxPositioner;
        public Constraint[] nxConstraints;
        public int numComponents;
        public Point3d[] componentPositions;
        public Matrix3x3[] componentOrientations;
        public string[] nxCompNames;


        //// TransCAD 관련 함수
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

            tDocs = tApp.Documents;
            tAssemDoc = tDocs.AddAssemDocument();
            tAssem = tAssemDoc.Assem;

            tComp = tAssem.CreateComponent();
            tComp.set_Name("Components1");

            tAssem.AddComponent(tComp);
            tConstraints = tAssem.Constraints;

            tApp.Visible = true;
            return true;
        }

        public void loadAssemblyInfo(NXOpen.Part assemblyFilePart)
        {
            //Get root component which is top level assembly component
            rootComponent = assemblyFilePart.ComponentAssembly.RootComponent;

            //Get components and constraints in (root)assembly
            nxComponents = rootComponent.GetChildren();

            //Get constraint information
            nxPositioner = assemblyFilePart.ComponentAssembly.Positioner;
            nxConstraints = nxPositioner.Constraints.ToArray();

            numComponents = nxComponents.Length;

            tcad_PartList = new TransCAD.Part[numComponents];
            componentPositions = new Point3d[numComponents];
            componentOrientations = new Matrix3x3[numComponents];
            nxCompNames = new string[numComponents];

            for (int i = 0; i < numComponents; i++)
            {
                NXOpen.Assemblies.Component nxComponent = nxComponents[i];

                //save name of NX Part into string array
                nxCompNames[i] = nxComponent.DisplayName;

                //Get position and orientation of each part and save them into componentPositions, componentOrientations
                Point3d componentPosition;
                Matrix3x3 componentOrientation;
                nxComponent.GetPosition(out componentPosition, out componentOrientation);
                componentPositions[i] = componentPosition;
                componentOrientations[i] = componentOrientation;

                //Get file directory of each part in assembly
                NXOpen.Part nxPart = null;
                nxPart = (Part)nxComponent.Prototype;
                string xmlPartDir = Path.ChangeExtension(nxPart.FullPath, ".xml"); //chaneg extension from .prt to .xml

                //Load transCAD xml part file corresponding to each NX part using file name, the transCAD xml should be in same folder with same name except extension(.prt -> .xml)
                TransCAD.Part tcad_Part = tComp.CreatePart(nxComponent.DisplayName);
                tcad_Part = tComp.ImportPartFromXML(xmlPartDir, tcad_Part);
                //save transCAD part data into array "tcad_PartList"
                tcad_PartList[i] = tcad_Part;

                //Place each part in TransCAD
                double extraDisplacement = i * 100; //To see whether constraints are applied or not, intentionally, place part with extra displacement which is different from original NX part
                tComp.SetPartPlacement(tcad_Part, componentPosition.X, componentPosition.Y + extraDisplacement, componentPosition.Z - extraDisplacement,
                        componentOrientation.Zx, componentOrientation.Zy, componentOrientation.Zz,
                        componentOrientation.Xx, componentOrientation.Xy, componentOrientation.Xz);
                tComp.AddPart(tcad_Part);
            }
            tAssem.AddComponent(tComp);
            tAssemDoc.Update();
        }


        public void UninitializeTransCAD()
        {
            tApp = null;
            tDocs = null;
            tAssemDoc = null;
            tAssem = null;
            tComp = null;
            tConstraints = null;
            
        }
    }
}
