using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NXOpen;
using NXOpen.Positioning;

namespace NX_Code_Macro
{
    class ConstraintHandler
    {
        private static Part Stpfile, Assem;
        private static PartLoadStatus partloadstatus;
        public static ComponentPositioner[] componentPositioners;
        public void UpConstraints(Session nxSession, string AssemblyPart, string[] Constraints, List<ConstData> ConstInfo)
        {
            Assem = nxSession.Parts.OpenDisplay(AssemblyPart, out partloadstatus);
            componentPositioners = new ComponentPositioner[ConstInfo.Count];
            for (int i = 0; i < ConstInfo.Count; i++)
            {
                componentPositioners[i] = Assem.ComponentAssembly.Positioner;

                if (ConstInfo[i].ConstType == "Touch")
                {
                    var touchConst = new touchCons();
                    touchConst.ApplyTouch(Assem, componentPositioners[i], ConstInfo[i]);
                    //Call 
                }
                else if (ConstInfo[i].ConstType == "Fit")
                {
                    var fitConst = new fitCons();
                    fitConst.ApplyFit(Assem, componentPositioners[i], ConstInfo[i]);
                }
                else if (ConstInfo[i].ConstType == "Fix")
                {
                    var fixConst = new fixCons();
                    fixConst.ApplyFix(Assem, componentPositioners[i], ConstInfo[i]);
                }
                else if (ConstInfo[i].ConstType == "Angle")
                {
                    var angConst = new angCons();
                    angConst.ApplyAng(Assem, componentPositioners[i], ConstInfo[i]);
                }
                else if (ConstInfo[i].ConstType == "Distance")
                {
                    var disConst = new disCons();
                    disConst.ApplyDis(Assem, componentPositioners[i], ConstInfo[i]);
                }

            }
            NXOpen.PartSaveStatus partSaveStatus1;
            partSaveStatus1 = Assem.Save(NXOpen.BasePart.SaveComponents.True, NXOpen.BasePart.CloseAfterSave.False);
            partSaveStatus1.Dispose();
            NXOpen.PartCloseResponses partCloseResponse1;
            partCloseResponse1 = nxSession.Parts.NewPartCloseResponses();
            Assem.Close(BasePart.CloseWholeTree.False, NXOpen.BasePart.CloseModified.UseResponses, partCloseResponse1);
            partloadstatus.Dispose();
        }
    }
}
