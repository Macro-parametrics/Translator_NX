using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NX_Code_Macro
{
    class ConstraintClass:Assembly
    {
        public TransCAD.StdAssemConstraints Constraints;
        public TransCAD.IStdAssemConstraint[] GetConstraints;

        public TransCAD.StdAssemblyConstraintType[] ConstType;

        public TransCAD.StdAssemConstraintCoaxial ConstraintCoaxial;
        public TransCAD.StdAssemConstraintIncidence ConstraintIncidence;

        public string[] ConsPrtName;
        public string[] ConsGeoName;
        public int NumConstraints;
        public int NumOfConstraint(Assembly assemManager)
        {
            Constraints = assemManager.tAssem.Constraints;
            return NumConstraints = Constraints.Count;
        }
     


    }
}
