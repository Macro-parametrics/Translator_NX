using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections; 
using System.IO;

using NXOpen; 
using NXOpen.Assemblies;
using NXOpen.Positioning;
using NXOpen.UF; 

namespace ConsoleApplication2
{
    class Program
    {//D:\UG_MACRO\CODE\GitHub_NXTranslator_Team\ASSEMBLY\NX_Assem_Pre\TestModels\A1\
        private static NXOpen.Session nxSession;
        private static NXOpen.UF.UFSession ufSession;

        static void Main(string[] args)
        {
            nxSession = Session.GetSession();
            ufSession = UFSession.GetUFSession();

            PartLoadStatus partLoadStatus1;
            //ArrayList tcad_PartList = new ArrayList();
            string fileName = args[0];
            //"D:\\UG_MACRO\\TEST MODEL\\Amodels\\A1\\Pre\\A1.prt";
            NXOpen.Part assemblyFilePart = nxSession.Parts.OpenDisplay(fileName, out partLoadStatus1);

            ASSEMBLY assemManager = new ASSEMBLY();
            assemManager.InitializeTransCAD();
            assemManager.loadAssemblyInfo(assemblyFilePart);

            int numConstraints = assemManager.nxConstraints.Length;
            List<CONSTRAINT> preConstraints = new List<CONSTRAINT>();

            //for each constraint
            for (int i = 0; i < numConstraints; i++)
            {
                NXOpen.Positioning.Constraint nxConstraint = assemManager.nxConstraints[i];

                Constraint.Type constraintType = nxConstraint.ConstraintType;
                System.Console.WriteLine(constraintType.ToString());

                ConstraintReference[] nxConstraintRefGeoms = nxConstraint.GetReferences();

                if (constraintType == Constraint.Type.Touch)
                {
                    //validation
                    if (nxConstraintRefGeoms.Length != 2)
                    {
                        System.Console.WriteLine("Touch Align Constraint should have only 2 reference geometries!");
                        continue;
                    }

                    //Coaxial
                    /*
                    if (nxConstraintRefGeoms[0].GetUsesGeometryAxis() && nxConstraintRefGeoms[1].GetUsesGeometryAxis())
                    {
                        preConstraints.Add(new CONSTRAINTCoaxial(nxConstraint, assemManager));
                    }
                    */
                    //Incidence??
                    else
                    {
                        preConstraints.Add(new CONSTRAINTIncidence(nxConstraint, assemManager));
                    }
                }
                    //coaxial-edge-based-fit-Constraint in NX
                else if (constraintType == Constraint.Type.Fit)
                {
                    preConstraints.Add(new CONSTRAINTCoaxial(nxConstraint, assemManager));
                }

                else if (constraintType == Constraint.Type.Fix)
                {
                }

                else if (constraintType == Constraint.Type.Distance)
                {
                }

                else if (constraintType == Constraint.Type.Angle)
                {
                }
            }

            //for each constraint
            for (int i = 0; i < preConstraints.Count(); i++)
            {
                preConstraints[i].getNXInfo();
            }
            //for each constraint
            for (int i = 0; i < preConstraints.Count(); i++)
            {
                preConstraints[i].toTransCAD();
            }
            assemManager.UninitializeTransCAD();

            
        }
    }  
}