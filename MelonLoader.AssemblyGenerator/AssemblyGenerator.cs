using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Linq;
using System.Reflection;
using System.Security.Cryptography;
using System.Text.RegularExpressions;
using MelonLoader.LightJson;
using MelonLoader.TinyJSON;
using MelonLoader.Tomlyn;
using MelonLoader.Tomlyn.Model;
using MelonLoader.Tomlyn.Syntax;

namespace MelonLoader.AssemblyGenerator
{
    internal static class Main
    {
        private static string GameAssembly_Path = null;
        private static string MSCORLIB_Path = null;
        internal static string BaseFolder = null;
        internal static string AssemblyFolder = null;
        private static Package UnityDependencies = new Package();
        private static Executable_Package Il2CppDumper = new Executable_Package();
        private static Executable_Package Il2CppAssemblyUnhollower = new Executable_Package();
        private static string localConfigPath = null;
        private static LocalConfig localConfig = null;
        private static Il2CppConfig il2cppConfig = new Il2CppConfig();
        private static bool DownloadedSuccessfully = true;

        internal static bool Initialize(string unityVersion, string gameRoot, string gameDataDir)
        {
            PreSetup(gameRoot);
            if (AssemblyGenerateCheck(unityVersion))
            {
                Logger.Log("Assembly Generation Needed!");
                if (!AssemblyGenerate(gameRoot, unityVersion, gameDataDir))
                    return false;
                Cleanup();
                Logger.Log("Assembly Generation was Successful!");
            }
            return true;
        }

        private static void PreSetup(string gameRoot)
        {
            GameAssembly_Path = Path.Combine(gameRoot, "GameAssembly.dll");

            AssemblyFolder = Path.Combine(gameRoot, "MelonLoader", "Managed");

            MSCORLIB_Path = Path.Combine(AssemblyFolder, "mscorlib.dll");
            
            BaseFolder = SetupDirectory(Path.Combine(Path.Combine(Path.Combine(gameRoot, "MelonLoader"), "Dependencies"), "AssemblyGenerator"));
            
            Il2CppDumper.BaseFolder = SetupDirectory(Path.Combine(BaseFolder, "Il2CppDumper"));
            Il2CppDumper.OutputDirectory = SetupDirectory(Path.Combine(Il2CppDumper.BaseFolder, "DummyDll"));
            Il2CppDumper.FileName = "Il2CppDumper.exe";

            Il2CppAssemblyUnhollower.BaseFolder = SetupDirectory(Path.Combine(BaseFolder, "Il2CppAssemblyUnhollower"));
            Il2CppAssemblyUnhollower.OutputDirectory = SetupDirectory(Path.Combine(Il2CppAssemblyUnhollower.BaseFolder, "Output"));
            Il2CppAssemblyUnhollower.FileName = "AssemblyUnhollower.exe";

            UnityDependencies.BaseFolder = SetupDirectory(Path.Combine(BaseFolder, "UnityDependencies"));

            localConfigPath = Path.Combine(BaseFolder, "config.cfg");
            localConfig = LocalConfig.Load(localConfigPath);
        }

        private static bool AssemblyGenerateCheck(string unityVersion)
        {
            if (Program.Force_Regenerate || (localConfig.UnityVersion != unityVersion) || (localConfig.DumperVersion != ExternalToolVersions.Il2CppDumperVersion) || (localConfig.UnhollowerVersion != ExternalToolVersions.Il2CppAssemblyUnhollowerVersion))
                return true;
            string game_assembly_hash = null;
            using (var md5 = MD5.Create())
            {
                using (var stream = File.OpenRead(GameAssembly_Path))
                {
                    var hash = md5.ComputeHash(stream);
                    game_assembly_hash = BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
                }
            }
            if (string.IsNullOrEmpty(localConfig.GameAssemblyHash) || (game_assembly_hash != localConfig.GameAssemblyHash))
                return true;
            return false;
        }

        private static void DownloadDependencies(string unityVersion)
        {
            Logger.Log("Downloading Il2CppDumper");
            try
            {
                DownloaderAndUnpacker.Run(ExternalToolVersions.Il2CppDumperUrl, ExternalToolVersions.Il2CppDumperVersion, localConfig.DumperVersion, Il2CppDumper.BaseFolder, TempFileCache.CreateFile());
                localConfig.DumperVersion = ExternalToolVersions.Il2CppDumperVersion;
                localConfig.Save(localConfigPath);
            }
            catch (Exception ex)
            {
                DownloadedSuccessfully = false;
                Logger.LogError(ex.ToString());
                Logger.Log("Can't download Il2CppDumper!");
            }
            if (!DownloadedSuccessfully)
                return;

            Logger.Log("Downloading Il2CppAssemblyUnhollower");
            try
            {
                DownloaderAndUnpacker.Run(ExternalToolVersions.Il2CppAssemblyUnhollowerUrl, ExternalToolVersions.Il2CppAssemblyUnhollowerVersion, localConfig.UnhollowerVersion, Il2CppAssemblyUnhollower.BaseFolder, TempFileCache.CreateFile());
                localConfig.UnhollowerVersion = ExternalToolVersions.Il2CppAssemblyUnhollowerVersion;
                localConfig.Save(localConfigPath);
            }
            catch (Exception ex)
            {
                DownloadedSuccessfully = false;
                Logger.LogError(ex.ToString());
                Logger.Log("Can't download Il2CppAssemblyUnhollower!");
            }
            if (!DownloadedSuccessfully)
                return;

            Logger.Log("Downloading Unity Dependencies");
            string tempfile = TempFileCache.CreateFile();
            bool run_fallback = false;
            try
            {
                DownloaderAndUnpacker.Run($"{ExternalToolVersions.UnityDependenciesBaseUrl}{unityVersion}.zip", unityVersion, localConfig.UnityVersion, UnityDependencies.BaseFolder, tempfile);
                localConfig.UnityVersion = unityVersion;
                localConfig.Save(localConfigPath);
            }
            catch (Exception ex)
            {
                run_fallback = true;
                Logger.LogError(ex.ToString());
                Logger.Log("Can't download Unity Dependencies for " + unityVersion + ", downloading Fallback...");
            }
            if (run_fallback)
            {
                string subver = unityVersion.Substring(0, unityVersion.LastIndexOf("."));
                try
                {
                    JsonArray data = (JsonArray)JsonValue.Parse(Program.webClient.DownloadString("https://api.github.com/repos/HerpDerpinstine/MelonLoader/contents/BaseLibs/UnityDependencies")).AsJsonArray;
                    if (data.Count > 0)
                    {
                        List<string> versionlist = new List<string>();
                        foreach (var x in data)
                        {
                            string version = Path.GetFileNameWithoutExtension(x["name"].AsString);
                            if (version.StartsWith(subver))
                            {
                                versionlist.Add(version);
                                string[] semvertbl = version.Split(new char[] { '.' });
                            }
                        }
                        if (versionlist.Count > 0)
                        {
                            versionlist = versionlist.OrderBy(x => int.Parse(x.Split(new char[] { '.' })[2])).ToList();
                            string latest_version = versionlist.Last();
                            Logger.Log("Fallback Unity Version: " + latest_version);

                            DownloaderAndUnpacker.Run($"{ExternalToolVersions.UnityDependenciesBaseUrl}{latest_version}.zip", latest_version, localConfig.UnityVersion, UnityDependencies.BaseFolder, tempfile);
                            localConfig.UnityVersion = unityVersion;
                            localConfig.Save(localConfigPath);
                        }
                    }
                }
                catch (Exception ex)
                {
                    Logger.LogError(ex.ToString());
                    Logger.LogError("Can't download Unity Dependencies, Unstripping will NOT be done!");
                }
            }
        }

        private static bool AssemblyGenerate(string gameRoot, string unityVersion, string gameDataDir)
        {
            DownloadDependencies(unityVersion);
            if (!DownloadedSuccessfully)
                return false;

            FixIl2CppDumperConfig();

            Logger.Log("Executing Il2CppDumper...");
            if (!Il2CppDumper.Execute(new string[] {
                GameAssembly_Path,
                Path.Combine(gameDataDir, "il2cpp_data", "Metadata", "global-metadata.dat")
            }))
            {
                Logger.LogError("Failed to Execute Il2CppDumper!");
                return false;
            }

            Logger.Log("Executing Il2CppAssemblyUnhollower...");
            if (!Il2CppAssemblyUnhollower.Execute(new string[] {
                ("--input=" + Il2CppDumper.OutputDirectory),
                ("--output=" + Il2CppAssemblyUnhollower.OutputDirectory),
                ("--mscorlib=" + MSCORLIB_Path),
                ("--unity=" + UnityDependencies.BaseFolder),
                "--gameassembly=" + GameAssembly_Path,
                "--blacklist-assembly=Mono.Security",
                "--blacklist-assembly=Newtonsoft.Json",
                "--blacklist-assembly=Valve.Newtonsoft.Json"
            }))
            {
                Logger.LogError("Failed to Execute Il2CppAssemblyUnhollower!");
                return false;
            }

            using (var md5 = MD5.Create())
            {
                using (var stream = File.OpenRead(GameAssembly_Path))
                {
                    var hash = md5.ComputeHash(stream);
                    localConfig.GameAssemblyHash = BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
                }
            }
            localConfig.Save(localConfigPath);

            return true;
        }

        private static void Cleanup()
        {
            if (localConfig.OldFiles.Count > 0)
            {
                for (int i = 0; i < localConfig.OldFiles.Count; i++)
                {
                    string oldFile = localConfig.OldFiles[i];
                    if (!string.IsNullOrEmpty(oldFile))
                    {
                        string oldFilePath = Path.Combine(AssemblyFolder, oldFile);
                        if (File.Exists(oldFilePath))
                            File.Delete(oldFilePath);
                    }
                }
                localConfig.OldFiles.Clear();
            }
            string[] files = Directory.GetFiles(Il2CppAssemblyUnhollower.OutputDirectory, "*", SearchOption.TopDirectoryOnly);
            if (files.Length > 0)
            {
                for (int i = 0; i < files.Length; i++)
                {
                    string file = files[i];
                    if (!string.IsNullOrEmpty(file))
                    {
                        string filename = Path.GetFileName(file);
                        localConfig.OldFiles.Add(filename);
                        File.Copy(file, Path.Combine(AssemblyFolder, filename), true);
                    }
                }
            }
            Directory.Delete(Il2CppAssemblyUnhollower.OutputDirectory, true);
            localConfig.Save(localConfigPath);
        }

        private static string SetupDirectory(string path) { if (!Directory.Exists(path)) Directory.CreateDirectory(path); return path; }
        private static void FixIl2CppDumperConfig() => File.WriteAllText(Path.Combine(Il2CppDumper.BaseFolder, "config.json"), Encoder.Encode(il2cppConfig, EncodeOptions.NoTypeHints | EncodeOptions.PrettyPrint));
    }

    internal class Package
    {
        internal string Version = null;
        internal string BaseFolder = null;
    }

    internal class Executable_Package : Package
    {
        internal string FileName = null;
        internal string OutputDirectory = null;

        private static void OverrideAppDomainBase(string @base)
        {
            var appDomainBase = ((AppDomainSetup)typeof(AppDomain).GetProperty("FusionStore", BindingFlags.NonPublic | BindingFlags.Instance).GetValue(AppDomain.CurrentDomain, new object[0]));
            appDomainBase.ApplicationBase = @base;
            Directory.SetCurrentDirectory(@base);
        }

        internal bool Execute(string[] argv)
        {
            string assembly_path = Path.Combine(BaseFolder, FileName);
            if (!File.Exists(assembly_path))
            {
                Logger.LogError(FileName + " does not exist!");
                return false;
            }
            var originalCwd = AppDomain.CurrentDomain.BaseDirectory;
            OverrideAppDomainBase(BaseFolder + Path.DirectorySeparatorChar);
            var generatorProcessInfo = new ProcessStartInfo(assembly_path);
            generatorProcessInfo.Arguments = String.Join(" ", argv.Where(s => !String.IsNullOrEmpty(s)).Select(it => ("\"" + Regex.Replace(it, @"(\\+)$", @"$1$1") + "\"")));
            generatorProcessInfo.UseShellExecute = false;
            generatorProcessInfo.RedirectStandardOutput = true;
            generatorProcessInfo.CreateNoWindow = true;
            Process process = null;
            Logger.Log("\"" + assembly_path + "\" " + generatorProcessInfo.Arguments);
            try { process = Process.Start(generatorProcessInfo); } catch (Exception e) { Logger.LogError(e.ToString()); Logger.LogError("Unable to Start " + FileName + "!"); OverrideAppDomainBase(originalCwd); return false; }
            var stdout = process.StandardOutput;
            while (!stdout.EndOfStream)
                Logger.Log(stdout.ReadLine());
            while (!process.HasExited)
                Thread.Sleep(100);
            OverrideAppDomainBase(originalCwd);
            return (process.ExitCode == 0);
        }
    }

    internal class Il2CppConfig
    {
        public bool DumpMethod = true;
        public bool DumpField = true;
        public bool DumpProperty = true;
        public bool DumpAttribute = true;
        public bool DumpFieldOffset = false;
        public bool DumpMethodOffset = false;
        public bool DumpTypeDefIndex = false;
        public bool GenerateDummyDll = true;
        public bool GenerateScript = false;
        public bool RequireAnyKey = false;
        public bool ForceIl2CppVersion = false;
        public float ForceVersion = 24.3f;
    }

    internal class LocalConfig
    {
        public string UnityVersion = null;
        public string GameAssemblyHash = null;
        public string DumperVersion = null;
        public string UnhollowerVersion = null;
        public List<string> OldFiles = new List<string>();

        public static LocalConfig Load(string path)
        {
            LocalConfig returnval = new LocalConfig();
            if (File.Exists(path))
            {
                string filestr = File.ReadAllText(path);
                if (!string.IsNullOrEmpty(filestr))
                {
                    var doc = Toml.Parse(filestr);
                    if (doc != null)
                    {
                        var table = doc.ToModel();
                        TomlTable MainTbl = (TomlTable)table["AssemblyGenerator"];
                        if (MainTbl != null)
                        {
                            returnval.UnityVersion = (string)MainTbl["UnityVersion"];
                            returnval.GameAssemblyHash = (string)MainTbl["GameAssemblyHash"];
                            returnval.DumperVersion = (string)MainTbl["DumperVersion"];
                            returnval.UnhollowerVersion = (string)MainTbl["UnhollowerVersion"];
                            TomlArray oldfilesarr = (TomlArray)MainTbl["OldFiles"];
                            if (oldfilesarr.Count > 0)
                            {
                                for (int i = 0; i < oldfilesarr.Count; i++)
                                {
                                    string file = (string)oldfilesarr[i];
                                    if (!string.IsNullOrEmpty(file))
                                        returnval.OldFiles.Add(file);
                                }
                            }
                        }
                    }
                }
            }
            return returnval;
        }

        public void Save(string path)
        {
            var doc = new DocumentSyntax()
            {
                Tables =
                {
                    new TableSyntax("AssemblyGenerator")
                    {
                        Items =
                        {
                            {"UnityVersion", (UnityVersion == null) ? "" : UnityVersion},
                            {"GameAssemblyHash", (GameAssemblyHash == null) ? "" : GameAssemblyHash},
                            {"DumperVersion", (DumperVersion == null) ? "" : DumperVersion},
                            {"UnhollowerVersion", (UnhollowerVersion == null) ? "" : UnhollowerVersion},
                            {"OldFiles", OldFiles.ToArray()}
                        }
                    }
                }
            };
            File.WriteAllText(path, doc.ToString());
        }
    }
}
