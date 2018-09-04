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
    class CONSTRAINT
    {
        protected ConstraintReference[] nxConstraintRefGeoms;
        protected int numRefGeoms;
        protected TransCAD.Part[] tcad_refGeomRelatedParts;
        protected TransCAD.Reference[] tcad_RefGeoms;
        protected Point3d[] refGeomPartPositions;
        protected Matrix3x3[] refGeomPartOrientations;
        protected NXObject[] nx_RefGeoms;
        protected ASSEMBLY assemManager_;

        public CONSTRAINT(NXOpen.Positioning.Constraint nxConstraint, ASSEMBLY assemManager)
        {
            assemManager_ = assemManager;

            nxConstraintRefGeoms = nxConstraint.GetReferences();

            //get number of reference geometry related with current constraint
            numRefGeoms = nxConstraintRefGeoms.Length;

            //Make array to save reference geometries and their corresponding parts in TransCAD
            tcad_refGeomRelatedParts = new TransCAD.Part[numRefGeoms];
            tcad_RefGeoms = new TransCAD.Reference[numRefGeoms];
            refGeomPartPositions = new Point3d[numRefGeoms];
            refGeomPartOrientations = new Matrix3x3[numRefGeoms];
            nx_RefGeoms = new NXObject[numRefGeoms];

            //for each reference geometry in constraint
            for (int i = 0; i < numRefGeoms; i++)
            {
                ConstraintReference nxConstraintRefGeom = nxConstraintRefGeoms[i];
                    
                nx_RefGeoms[i] = nxConstraintRefGeom.GetGeometry();
                                        
                //identify reference related part's index by name from NX
                int refGeomPartIdx = 0;
                string refCompName = nx_RefGeoms[i].OwningComponent.DisplayName;
                for (int k = 0; k < assemManager_.numComponents; k++)
                {
                    if (assemManager_.nxCompNames[k] == refCompName)
                    {
                        refGeomPartIdx = k;
                    }
                }
                    
                //get TransCAD part info related with current reference geometry
                tcad_refGeomRelatedParts[i] = assemManager_.tcad_PartList[refGeomPartIdx];

                //get current TransCAD part's position and orientation to convert world coordinate(from NX) to local coordinate(in TransCAD)
                refGeomPartPositions[i] = assemManager_.componentPositions[refGeomPartIdx];
                refGeomPartOrientations[i] = assemManager_.componentOrientations[refGeomPartIdx];
            }
        }
        public virtual void getNXInfo()
        {
        }
        public virtual void toTransCAD()
        {
        }
    }
}
