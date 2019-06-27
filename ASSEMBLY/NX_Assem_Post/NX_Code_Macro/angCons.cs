using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NXOpen;
using NXOpen.Assemblies;
using NXOpen.Positioning;

namespace NX_Code_Macro
{
    class angCons
    {
        public void ApplyAng(Part Assem, ComponentPositioner componentPositioner1, ConstData constinfo)
        {
            Constraint constraint1 = componentPositioner1.CreateConstraint(true); ///1
            ComponentConstraint compoConst = (NXOpen.Positioning.ComponentConstraint)constraint1; ///2
            compoConst.ConstraintType = NXOpen.Positioning.Constraint.Type.Angle; //3

            Component[] comp = new Component[2];
            Edge[] edg = new Edge[2];
            Face[] faces = new Face[2];
            ConstraintReference[] constref = new ConstraintReference[2];

            comp[0] = (NXOpen.Assemblies.Component)Assem.ComponentAssembly.RootComponent.FindObject(constinfo.ConstRef1[0]);
            comp[1] = (NXOpen.Assemblies.Component)Assem.ComponentAssembly.RootComponent.FindObject(constinfo.ConstRef2[0]);

            faces[0] = (NXOpen.Face)comp[0].FindObject(constinfo.ConstRef1[1]);
            faces[1] = (NXOpen.Face)comp[1].FindObject(constinfo.ConstRef2[1]);
            for (int i = 0; i < 2; i++)
            {
                constref[i] = compoConst.CreateConstraintReference(comp[i], faces[i], false, false, false);
            }

            compoConst.SetExpression("");
        }
    }
}
