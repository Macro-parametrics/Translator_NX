using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NXOpen;
using NXOpen.Assemblies;
using NXOpen.Positioning;
using NXOpen.UF; 

namespace NX_Code_Macro
{
    class CoaxialConstraintClass: ConstraintClass
    {
        List<Point3d> NXrefPoints1 = new List<Point3d>();
        List<Point3d> NXrefPoints2 = new List<Point3d>();
       
    }
}
