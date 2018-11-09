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

    class Program
    {
        private static NXOpen.Session nxSession,nxSession1;
        private static NXOpen.UF.UFSession ufSession;
       // private static TransCAD.Application tApp;

        static void Main(string[] args)
        {
            

            System.Console.WriteLine("parameter count = {0}", args.Length);


            var AssemManager = new Assembly();//Instance of Assembly Class
            var fileDir = new NewNXfile(); //Contains File Directory Infromation

            fileDir.initiatefl(args[0]);
            nxSession = Session.GetSession();
            
            
            AssemManager.InitializeTransCAD();           
            AssemManager.loadAssemblyInfo();

            AssemManager.FromTransCAD(nxSession);
            //nxSession = Session.GetSession();
            AssemManager.NXRootFile(nxSession, fileDir);
            AssemManager.ToNXFile(nxSession);
            //AssemManager.Comparison();
       }
    }

}
