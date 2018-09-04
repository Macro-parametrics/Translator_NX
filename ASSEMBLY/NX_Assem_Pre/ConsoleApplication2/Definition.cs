using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices; //For Marshal

using NXOpen; 

namespace ConsoleApplication2
{
    class convertWorldToLocal 
    {
        public Point3d calculateLocalCoord (Point3d worldCoord, Point3d compPosition, Matrix3x3 compOrientation)
        {
            double dispX = worldCoord.X - compPosition.X;
            double dispY = worldCoord.Y - compPosition.Y;
            double dispZ = worldCoord.Z - compPosition.Z;
            Point3d localCoord;
            localCoord.X = Math.Round(dispX * compOrientation.Xx + dispY * compOrientation.Xy + dispZ * compOrientation.Xz, 6);//Round upto 6 decimal place
            localCoord.Y = Math.Round(dispX * compOrientation.Yx + dispY * compOrientation.Yy + dispZ * compOrientation.Yz, 6);
            localCoord.Z = Math.Round(dispX * compOrientation.Zx + dispY * compOrientation.Zy + dispZ * compOrientation.Zz, 6);
            return localCoord;
        }
    }

    class findGeometry
    {
        public bool vertexMatching(TransCAD.Vertex vtc, List<Point3d> NXrefP1, List<Point3d> NXrefP2)
        {
            for (int i = 0; i < NXrefP1.Count; i++)
            {
                if ((NXrefP1[i].X == vtc.X) && (NXrefP1[i].Y == vtc.Y) && (NXrefP1[i].Z == vtc.z))
                    return true;
            }
            for (int i = 0; i < NXrefP2.Count; i++)
            {
                if ((NXrefP2[i].X == vtc.X) && (NXrefP2[i].Y == vtc.Y) && (NXrefP2[i].Z == vtc.z))
                    return true;
            }
            return false;
        }
    }

}
