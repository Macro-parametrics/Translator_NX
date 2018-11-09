using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NXOpen;
namespace NX_Code_Macro
{
    class NewNXfile
    {
        public static string A1_PostFileLocation ;//= "D:\\Macro\\A1post\\";
        public static string rootAssemFileName = "A1";
        public static string fileExtension = ".prt";
        public static string x;
        public void initiatefl(string x)
        {
            A1_PostFileLocation = x;
        }

        public string rootDir()
        {
            return x = A1_PostFileLocation + rootAssemFileName + fileExtension;

        }

        public string subassemdir(string y)
        {
            return x = A1_PostFileLocation + y + fileExtension;

        }

        public string partdir(string z)
        {
            return x = A1_PostFileLocation + z + fileExtension;

        }
        public void NXfile(NXOpen.FileNew nxRootCompFile, string name)
        {
            nxRootCompFile.TemplateFileName = "assembly-mm-template.prt";
            nxRootCompFile.UseBlankTemplate = false;
            nxRootCompFile.ApplicationName = "AssemblyTemplate";
            nxRootCompFile.Units = Part.Units.Millimeters;
            nxRootCompFile.RelationType = "";
            nxRootCompFile.UsesMasterModel = "No";
            nxRootCompFile.TemplateType = NXOpen.FileNewTemplateType.Item;
            nxRootCompFile.TemplatePresentationName = "Assembly";
            nxRootCompFile.ItemType = "";
            nxRootCompFile.Specialization = "";
            nxRootCompFile.SetCanCreateAltrep(false);
            nxRootCompFile.NewFileName = name;
            nxRootCompFile.MasterFileName = "";
            nxRootCompFile.MakeDisplayedPart = true;
         

        }

    }
}
