using System;
using System.Net;
using MelonLoader.LightJson;

namespace MelonLoader.AssemblyGenerator
{
    public static class Program
    {
        public static bool Force_Regenerate = false;
        internal static WebClient webClient = new WebClient();

        public static int Main(string[] args)
        {
            ServicePointManager.Expect100Continue = true;
            ServicePointManager.SecurityProtocol = SecurityProtocolType.Ssl3 | SecurityProtocolType.Tls | SecurityProtocolType.Tls11 | SecurityProtocolType.Tls12 | (SecurityProtocolType)3072;
            webClient.Headers.Add("User-Agent", "Unity web player");

            if (args.Length < 4)
            {
                Logger.LogError("Bad arguments for generator process; expected arguments: <unityVersion> <gameRoot> <gameData> <regenerate> <force_version_unhollower>");
                return -1;
            }

            if ((args.Length >= 4) && !string.IsNullOrEmpty(args[3]) && args[3].Equals("true"))
                Force_Regenerate = true;

            if (args.Length >= 5)
            {
                try
                {
                    string Force_Version_Unhollower = args[4];
                    if (!string.IsNullOrEmpty(Force_Version_Unhollower))
                    {
                        if (!Force_Version_Unhollower.Equals("0.2.0.0") && !Force_Version_Unhollower.Equals("0.1.1.0") && !Force_Version_Unhollower.Equals("0.1.0.0"))
                        {
                            JsonArray data = (JsonArray)JsonValue.Parse(webClient.DownloadString("https://api.github.com/repos/knah/Il2CppAssemblyUnhollower/releases")).AsJsonArray;
                            if (data.Count > 0)
                            {
                                bool found_version = false;
                                foreach (var x in data)
                                {
                                    string version = x["tag_name"].AsString;
                                    if (!string.IsNullOrEmpty(version) && version.Equals("v" + Force_Version_Unhollower))
                                    {
                                        found_version = true;
                                        ExternalToolVersions.Il2CppAssemblyUnhollowerVersion = Force_Version_Unhollower;
                                        ExternalToolVersions.Il2CppAssemblyUnhollowerUrl = "https://github.com/knah/Il2CppAssemblyUnhollower/releases/download/v" + Force_Version_Unhollower + "/Il2CppAssemblyUnhollower." + Force_Version_Unhollower + ".zip";
                                        break;
                                    }
                                }
                                if (!found_version)
                                    throw new Exception("Invalid Version Specified!");
                            }
                            else
                                throw new Exception("Unable to Verify Version!");
                        }
                        else
                            throw new Exception("Invalid Version! Only Supports v0.3.0.0 and up!");
                    }
                }
                catch (Exception e)
                {
                    Logger.LogError("Failed to Force Unhollower Version, Defaulting to " + ExternalToolVersions.Il2CppAssemblyUnhollowerVersion + "  |  " + e.Message);
                }
            }

            try
            {
                int returnval = (AssemblyGenerator.Main.Initialize(args[0], args[1], args[2]) ? 0 : -2);
                TempFileCache.ClearCache();
                return returnval;
            }
            catch (Exception ex)
            {
                TempFileCache.ClearCache();
                Logger.LogError("Failed to generate assemblies;");
                Logger.LogError(ex.ToString());
                
                return -3;
            }
        }
    }
}