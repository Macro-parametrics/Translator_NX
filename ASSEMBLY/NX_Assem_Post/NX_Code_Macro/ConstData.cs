using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NXOpen;

namespace NX_Code_Macro
{
    class ConstData
    {
        public string ConstType { get; set; }
        public string[] ConstRef1 { get; set; }
        public string[] ConstRef2 { get; set; }
        public int expressvalue { get; set; }
        public string fixbody { get; set; }
    }
    class EdgeBasedData
    {
        public string PrtName { get; set; }
        public string GeoType { get; set; }
        public string EdgeType { get; set; }
        public string EdgeName { get; set; }
        public Point3d EVectors { get; set; }
        public Double EMagniture { get; set; }
        public string[] OwFaceName { get; set; }
        public string[] OwFaceType { get; set; }
    }
    class FaceBasedDate
    {
        public string PrtName { get; set; }
        public string GeoType { get; set; }
        public string FaceType { get; set; }
        public string FaceName { get; set; }
        public List<EdgeBasedData> EdgeList { get; set; }
    }
    class ConstGeoData
    {
        public List<EdgeBasedData> PEdgeData { get; set; }
        public List<FaceBasedDate> PFaceData { get; set; }
    }
}
