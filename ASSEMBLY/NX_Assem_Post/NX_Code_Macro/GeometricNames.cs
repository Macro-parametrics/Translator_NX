using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections;
using System.IO;
using System.Runtime.InteropServices; //For Marshal

using NXOpen;
using NXOpen.Assemblies;
using NXOpen.Positioning;
using NXOpen.UF;

namespace NX_Code_Macro
{
    class GeometricNames
    {


        private static string rename = null, featureName = null, matched = null, featureName1 = null;
        public string LoadName(Session nxSession, Point3d[] fromtcad, string dir, string type)
        {
            var loadpartinfo = new NXPartFile();
            rename = null;
            int count = 0;
            loadpartinfo.FileLoad(nxSession, dir);
            loadpartinfo.BodzinPart(dir);
            string feature = "";
            Point3d[] fromNX;
            int z = 0, z1 = 0, x = 0, x2 = 0;

            //  count = 1;
            #region
            if (type == "Face")
            {
                for (; z < loadpartinfo.NumOfFaces(); z++)
                {
                    fromNX = loadpartinfo.FaceVrtx(z);
                    Console.WriteLine(fromNX.Length + "\n" + fromtcad.Length);
                    if ((fromNX.Length == fromtcad.Length) /*|| (fromNX.Length == fromtcad.Length - 1) && (fromNX.Length > 1)*/)
                    {
                        Point3d checkchange;
                        Console.WriteLine("Condition satisfies");
                        int k = 0;
                        for (int j = 0; j < fromNX.Length; j++)
                        {
                            for (k = 0; k < fromNX.Length; k++)
                            {
                                if ((fromtcad[j].X == fromNX[k].X) && (fromtcad[j].Y == fromNX[k].Y) && (fromtcad[j].Z == fromNX[k].Z))
                                {
                                    count = count + 1;
                                    break;
                                }
                            }
                        }

                    }
                    if ((count >= fromNX.Length) && (fromNX.Length > 1))
                    {
                        rename = loadpartinfo.FaceName(loadpartinfo.FaceVrtx(z));
                        break;
                    }
                    else
                    {
                        count = 0;
                    }
                }

            }
            #endregion
            #region
            if ((type == "Face") && (rename == null))
            {
                for (; z1 < loadpartinfo.NumOfFaces(); z1++)
                {
                    fromNX = loadpartinfo.FaceVrtx(z1);
                    Console.WriteLine(fromNX.Length + "\n" + fromtcad.Length);
                    if ((fromNX.Length == fromtcad.Length))// && (fromNX.Length == fromtcad.Length - 1) && (fromNX.Length > 1))
                    {
                        Point3d checkchange;
                        Console.WriteLine("Condition satisfies");
                        for (int j = 0; j < fromtcad.Length; j++)
                        {
                            for (int k = 0; k < fromNX.Length; k++)
                            {
                                if ((fromtcad[j].X == fromNX[k].X) && (fromtcad[j].Y == fromNX[k].Y) && (fromtcad[j].Z == fromNX[k].Z))
                                {
                                    count = count + 1;
                                    break;
                                }
                            }
                        }

                    }
                    if (rename == null)
                    {
                        if ((fromNX.Length == fromtcad.Length - 1))// && (fromNX.Length == fromtcad.Length - 1) && (fromNX.Length > 1))
                        {
                            Point3d checkchange;
                            Console.WriteLine("Condition satisfies");
                            for (int j = 0; j < fromtcad.Length; j++)
                            {
                                for (int k = 0; k < fromNX.Length; k++)
                                {
                                    if ((fromtcad[j].X == fromNX[k].X) && (fromtcad[j].Y == fromNX[k].Y) && (fromtcad[j].Z == fromNX[k].Z))
                                    {
                                        count = count + 1;
                                        break;
                                    }
                                }
                            }

                        }

                    }
                    if ((count >= fromNX.Length) && (fromNX.Length > 1))
                    {
                        rename = loadpartinfo.FaceName(loadpartinfo.FaceVrtx(z1));
                        break;
                    }
                    else
                    {
                        count = 0;
                    }
                }

            }
            #endregion

            #region
            else if (type == "Edge")
            {

                int n = loadpartinfo.NumOfEdges();
                fromNX = new Point3d[n];

                for (; x < n; x++)
                {
                    fromNX = loadpartinfo.EdgeMVrtx(x);
                    if (fromNX.Length == fromtcad.Length)
                    {
                        if ((fromtcad[0].X == fromNX[0].X) && (fromtcad[0].Y == fromNX[0].Y) && (fromtcad[0].Z == fromNX[0].Z))
                        {
                            //count = count + 1;
                            count = 5;
                            rename = loadpartinfo.EdgeName(x);
                            break;
                        }
                        /*
                        if ((count >= fromNX.Length) && (fromNX.Length > 1))
                        {
                            rename = loadpartinfo.EdgeName(z);
                            break;
                        }*/

                        /*
                        if ((count >= fromNX.Length) && (fromNX.Length > 1))
                        {
                            rename = loadpartinfo.EdgeName(z);
                            break;
                        }*/

                    }
                    //if ((count >= fromNX.Length) && (fromNX.Length > 1))
                    //break;
                }
                //if ((count >= fromNX.Length) && (fromNX.Length > 1))
                //break;

            }
            #endregion
            #region
            if ((count != 5) && (type == "Edge"))
            {
                int n = loadpartinfo.NumOfEdges();
                fromNX = new Point3d[n];
                for (; x2 < n; x2++)
                {
                    fromNX = loadpartinfo.EdgeMVrtx(x2);
                    if (fromNX.Length == fromtcad.Length)
                    {
                        if ((fromtcad[0].X == fromNX[0].X) && (fromtcad[0].Y == fromNX[0].Y) && (20 == fromNX[0].Z) &&
                            ((fromtcad[1].X == fromNX[1].X) && (fromtcad[1].Y == fromNX[1].Y) && (20 == fromNX[1].Z)))
                        {
                            count = 5;
                            rename = loadpartinfo.EdgeName(x2);
                            break;
                        }
                    }
                }
            }
            #endregion
            if (type == "Face")
            {
                for (int y = 0; y < loadpartinfo.BodzinPart(dir); y++)
                {
                    string fName = loadpartinfo.featurename(y);
                    string[] fFeatureN = loadpartinfo.faceinfeature(fName);
                    for (int x1 = 0; x1 < fFeatureN.Length; x1++)
                    {
                        if (fFeatureN[x1].Contains(rename))
                        {
                            featureName = loadpartinfo.featurename(y);
                            break;
                        }
                    }
                }
                matched = "PROTO#.Features|" + featureName + "|" + rename;
            }
            #region
            if (type == "Edge")
            {
                for (int y = 0; y < loadpartinfo.BodzinPart(dir); y++)
                {
                    string eName = loadpartinfo.featurename(y);
                    string[] EFeatureN = loadpartinfo.EdgeinFeature(eName);
                    for (int x1 = 0; x1 < EFeatureN.Length; x1++)
                    {
                        if (EFeatureN[x1].Contains(rename))
                        {
                            featureName1 = loadpartinfo.featurename(y);
                            break;
                        }
                    }
                }
                matched = "PROTO#.Features|" + featureName1 + "|" + rename;
            }
            #endregion
            return matched;
        }
    }

    /*
    private static string rename, featureName;
    public string LoadName(Session nxSession,string dir,Point3d[] fromtcad)
    {
        var loadpartinfo = new NXPartFile();
        int count = 01;
        loadpartinfo.FileLoad(nxSession,dir);
        loadpartinfo.BodzinPart(dir);
        string feature="";
        Point3d[] fromNX;
        int i = 0;
        for (int z = 0; z < loadpartinfo.BodzinPart(dir); z++)
        {
            count = 1;
            fromNX = loadpartinfo.FaceVrtx(z);
            Console.WriteLine(fromNX.Length + "\n" + fromtcad.Length);
            if ((fromNX.Length == fromtcad.Length) || (fromNX.Length == fromtcad.Length-1) && (fromNX.Length>1))
            {
                Point3d checkchange;
                Console.WriteLine("Condition satisfies");
                for (int j = 0; j < fromNX.Length; j++)
                {
                    for (int k = 0; k < fromNX.Length; k++)
                    {
                        if ((fromtcad[j].X == fromNX[k].X ) && (fromtcad[j].Y == fromNX[k].Y ) && (fromtcad[j].Z == fromNX[k].Z ))
                        {
                            count = count + 1;
                            break;
                        }
                    }
                }
            }
            if ((count >= fromNX.Length)&&(fromNX.Length>1))
            {
                rename = loadpartinfo.FaceName(loadpartinfo.FaceVrtx(z));
                break;
            }
        }
        for (int y = 0; y < loadpartinfo.BodzinPart(dir); y++)
        {
            string fName = loadpartinfo.featurename(y);
            string[] fFeatureN = loadpartinfo.faceinfeature(fName);
            for (int x = 0; x < fFeatureN.Length; x++)
            {
                if (fFeatureN[x].Contains(rename))
                {
                    featureName = loadpartinfo.featurename(y);
                    break;
                }
            }
        }
        string macthedFace = "PROTO#.Features|" + featureName + "|" + rename;
        return macthedFace;
    }
    */

}
