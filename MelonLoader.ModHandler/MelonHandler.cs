using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using Harmony;
using MelonLoader.ICSharpCode.SharpZipLib.Zip;
#pragma warning disable 0612
#pragma warning disable 0618

namespace MelonLoader
{
    public static class MelonHandler
    {
        internal static bool HasMelons = false;
        internal static Assembly Assembly_CSharp = null;
        private static List<MelonPlugin> _TempPlugins = null;
        internal static List<MelonPlugin> _Plugins = new List<MelonPlugin>();
        public static List<MelonPlugin> Plugins { get => _Plugins; }
        internal static List<MelonMod> _Mods = new List<MelonMod>();
        public static List<MelonMod> Mods { get => _Mods; }

        internal static void LoadAll(bool plugins = false)
        {
            string searchdir = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, (plugins ? "Plugins" : "Mods"));
            if (!Directory.Exists(searchdir))
            {
                Directory.CreateDirectory(searchdir);
                return;
            }
            LoadMode loadmode = (plugins ? GetLoadMode_Plugins() : GetLoadMode_Mods());

            // DLL
            string[] files = Directory.GetFiles(searchdir, "*.dll");
            if (files.Length > 0)
            {
                for (int i = 0; i < files.Length; i++)
                {
                    string file = files[i];
                    if (string.IsNullOrEmpty(file))
                        continue;

                    bool file_extension_check = Path.GetFileNameWithoutExtension(file).EndsWith("-dev");
                    if ((loadmode != LoadMode.BOTH) && ((loadmode == LoadMode.DEV) ? !file_extension_check : file_extension_check))
                        continue;

                    try
                    {
                        LoadFromFile(file, plugins);
                    }
                    catch (Exception e)
                    {
                        MelonLogger.LogError("Unable to load " + file + ":\n" + e.ToString());
                        MelonLogger.Log("------------------------------");
                    }
                }
            }

            // ZIP
            string[] zippedFiles = Directory.GetFiles(searchdir, "*.zip");
            if (zippedFiles.Length > 0)
            {
                for (int i = 0; i < zippedFiles.Length; i++)
                {
                    string file = zippedFiles[i];
                    if (string.IsNullOrEmpty(file))
                        continue;
                    try
                    {
                        using (var fileStream = File.OpenRead(file))
                        {
                            using (var zipInputStream = new ZipInputStream(fileStream))
                            {
                                ZipEntry entry;
                                while ((entry = zipInputStream.GetNextEntry()) != null)
                                {
                                    string filename = Path.GetFileName(entry.Name);
                                    if (string.IsNullOrEmpty(filename) || !filename.EndsWith(".dll"))
                                        continue;

                                    bool file_extension_check = Path.GetFileNameWithoutExtension(file).EndsWith("-dev");
                                    if ((loadmode != LoadMode.BOTH) && ((loadmode == LoadMode.DEV) ? !file_extension_check : file_extension_check))
                                        continue;

                                    using (var unzippedFileStream = new MemoryStream())
                                    {
                                        int size = 0;
                                        byte[] buffer = new byte[4096];
                                        while (true)
                                        {
                                            size = zipInputStream.Read(buffer, 0, buffer.Length);
                                            if (size > 0)
                                                unzippedFileStream.Write(buffer, 0, size);
                                            else
                                                break;
                                        }
                                        LoadFromAssembly(Assembly.Load(unzippedFileStream.ToArray()), plugins, (file + "/" + filename));
                                    }
                                }
                            }
                        }
                    }
                    catch (Exception e)
                    {
                        MelonLogger.LogError("Unable to load " + file + ":\n" + e.ToString());
                        MelonLogger.Log("------------------------------");
                    }
                }
            }
            Main.LegacySupport(_Mods, _Plugins, MelonLoaderBase._IsVRChat, MelonLoaderBase._IsBoneworks);
        }

        public static void LoadFromFile(string filelocation, bool isPlugin = false)
        {
            var assembly = File.ReadAllBytes(filelocation);
            byte[] symbols = { 0 };

            if (File.Exists(filelocation + ".mdb"))
            {
                symbols = File.ReadAllBytes(filelocation + ".mdb");
            }

            LoadFromAssembly((Imports.IsDebugMode() ? Assembly.LoadFrom(filelocation) : Assembly.Load(assembly, symbols)), isPlugin, filelocation);
        }

        public static void LoadFromAssembly(Assembly asm, bool isPlugin = false, string filelocation = null)
        {
            if (!asm.Equals(null))
            {
                MelonLegacyAttributeSupport.Response_Info response_Info = MelonLegacyAttributeSupport.GetMelonInfoAttribute(asm, isPlugin);
                MelonInfoAttribute InfoAttribute = response_Info.Default;
                if ((InfoAttribute != null) && (InfoAttribute.SystemType != null) && InfoAttribute.SystemType.IsSubclassOf((isPlugin ? typeof(MelonPlugin) : typeof(MelonMod))))
                {
                    bool isCompatible = false;
                    bool isUniversal = false;
                    bool hasAttribute = true;
                    MelonLegacyAttributeSupport.Response_Game response_Game = MelonLegacyAttributeSupport.GetMelonGameAttributes(asm, isPlugin);
                    MelonGameAttribute[] GameAttributes = response_Game.Default;
                    int GameAttributes_Count = GameAttributes.Length;
                    if (GameAttributes_Count > 0)
                    {
                        for (int i = 0; i < GameAttributes_Count; i++)
                        {
                            MelonGameAttribute GameAttribute = GameAttributes[i];
                            if (MelonLoaderBase.CurrentGameAttribute.IsCompatible(GameAttribute))
                            {
                                isCompatible = true;
                                isUniversal = MelonLoaderBase.CurrentGameAttribute.IsCompatibleBecauseUniversal(GameAttribute);
                                break;
                            }
                        }
                    }
                    else
                        hasAttribute = false;
                    MelonBase baseInstance = Activator.CreateInstance(InfoAttribute.SystemType) as MelonBase;
                    if (baseInstance != null)
                    {
                        response_Info.SetupMelon(baseInstance);
                        response_Game.SetupMelon(baseInstance);
                        baseInstance.OptionalDependenciesAttribute = asm.GetCustomAttributes(false).FirstOrDefault(x => (x.GetType() == typeof(MelonOptionalDependenciesAttribute))) as MelonOptionalDependenciesAttribute;
                        baseInstance.Location = filelocation;
                        baseInstance.Compatibility = (isUniversal ? MelonBase.MelonCompatibility.UNIVERSAL :
                            (isCompatible ? MelonBase.MelonCompatibility.COMPATIBLE :
                                (!hasAttribute ? MelonBase.MelonCompatibility.NOATTRIBUTE : MelonBase.MelonCompatibility.INCOMPATIBLE)
                            )
                        );
                        if (baseInstance.Compatibility < MelonBase.MelonCompatibility.INCOMPATIBLE)
                        {
                            baseInstance.Assembly = asm;
                            baseInstance.harmonyInstance = HarmonyInstance.Create(asm.FullName);
                        }
                        if (isPlugin)
                            _Plugins.Add((MelonPlugin)baseInstance);
                        else
                            _Mods.Add((MelonMod)baseInstance);
                    }
                    else
                        MelonLogger.LogError("Unable to load " + asm.GetName() + "! Failed to Create Instance!");
                }
            }
        }

        internal static void LogAndPrune()
        {
            if (_Plugins.Count > 0)
            {
                for (int i = 0; i < _Plugins.Count; i++)
                    if (_Plugins[i] != null)
                        LogMelonInfo(_Plugins[i]);
                _Plugins = _TempPlugins;
            }
            if (_Plugins.Count <= 0)
            {
                MelonLogger.Log("No Plugins Loaded!");
                MelonLogger.Log("------------------------------");
            }
            else
                HasMelons = true;

            if (_Mods.Count > 0)
            {
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        LogMelonInfo(_Mods[i]);
                _Mods.RemoveAll((MelonMod mod) => ((mod == null) || (mod.Compatibility >= MelonBase.MelonCompatibility.INCOMPATIBLE)));
                DependencyGraph<MelonMod>.TopologicalSort(_Mods, mod => mod.Info.Name);
            }
            if (_Mods.Count <= 0)
            {
                MelonLogger.Log("No Mods Loaded!");
                MelonLogger.Log("------------------------------");
            }
            else
                HasMelons = true;
        }

        private static void LogMelonInfo(MelonBase melon)
        {
            MelonLogger.Log(melon.Info.Name
                            + (!string.IsNullOrEmpty(melon.Info.Version)
                            ? (" v" + melon.Info.Version) : "")
                            + (!string.IsNullOrEmpty(melon.Info.Author)
                            ? (" by " + melon.Info.Author) : "")
                            + (!string.IsNullOrEmpty(melon.Info.DownloadLink)
                            ? (" (" + melon.Info.DownloadLink + ")")
                            : "")
                            );
            MelonLogger.LogMelonCompatibility(melon.Compatibility);
            MelonLogger.Log("------------------------------");
        }

        internal static void OnPreInitialization()
        {
            if (_Plugins.Count > 0)
            {
                HashSet<MelonPlugin> failedPlugins = new HashSet<MelonPlugin>();
                _TempPlugins = _Plugins.Where(plugin => (plugin.Compatibility < MelonBase.MelonCompatibility.INCOMPATIBLE)).ToList();
                DependencyGraph<MelonPlugin>.TopologicalSort(_TempPlugins, plugin => plugin.Info.Name);
                for (int i = 0; i < _TempPlugins.Count; i++)
                    if (_TempPlugins[i] != null)
                        try { _TempPlugins[i].OnPreInitialization(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _TempPlugins[i].Info.Name); failedPlugins.Add(_TempPlugins[i]); }
                _TempPlugins.RemoveAll(plugin => ((plugin == null) || failedPlugins.Contains(plugin)));
                Main.LegacySupport(_Mods, _TempPlugins, MelonLoaderBase._IsVRChat, MelonLoaderBase._IsBoneworks);
            }
        }

        internal static void OnApplicationStart()
        {
            if (_Plugins.Count > 0)
            {
                HashSet<MelonPlugin> failedPlugins = new HashSet<MelonPlugin>();
                for (int i = 0; i < _Plugins.Count; i++)
                    if (_Plugins[i] != null)
                        try { _Plugins[i].harmonyInstance.PatchAll(_Plugins[i].Assembly); _Plugins[i].OnApplicationStart(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Plugins[i].Info.Name); HarmonyInstance.UnpatchAllMelonInstances(_Plugins[i]); failedPlugins.Add(_Plugins[i]); }
                _Plugins.RemoveAll(plugin => ((plugin == null) || failedPlugins.Contains(plugin)));
                Main.LegacySupport(_Mods, _Plugins, MelonLoaderBase._IsVRChat, MelonLoaderBase._IsBoneworks);
            }
            if (_Mods.Count > 0)
            {
                HashSet<MelonMod> failedMods = new HashSet<MelonMod>();
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].harmonyInstance.PatchAll(_Mods[i].Assembly); _Mods[i].OnApplicationStart(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); HarmonyInstance.UnpatchAllMelonInstances(_Mods[i]); failedMods.Add(_Mods[i]); }
                _Mods.RemoveAll(mod => ((mod == null) || failedMods.Contains(mod)));
                Main.LegacySupport(_Mods, _Plugins, MelonLoaderBase._IsVRChat, MelonLoaderBase._IsBoneworks);
            }
        }

        public static void OnApplicationQuit()
        {
            if (_Plugins.Count > 0)
                for (int i = 0; i < _Plugins.Count; i++)
                    if (_Plugins[i] != null)
                        try { _Plugins[i].OnApplicationQuit(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Plugins[i].Info.Name); }
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].OnApplicationQuit(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
            MelonLoaderBase.Quit();
        }

        public static void OnModSettingsApplied()
        {
            if (_Plugins.Count > 0)
                for (int i = 0; i < _Plugins.Count; i++)
                    try { _Plugins[i].OnModSettingsApplied(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Plugins[i].Info.Name); }
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].OnModSettingsApplied(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
        }

        public static void OnUpdate()
        {
            SceneHandler.CheckForSceneChange();
            if (Imports.IsIl2CppGame() && MelonLoaderBase._IsVRChat)
                VRChat_CheckUiManager();
            if (_Plugins.Count > 0)
                for (int i = 0; i < _Plugins.Count; i++)
                    if (_Plugins[i] != null)
                        try { _Plugins[i].OnUpdate(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Plugins[i].Info.Name); }
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].OnUpdate(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
        }

        public static void OnFixedUpdate()
        {
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].OnFixedUpdate(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
        }

        public static void OnLateUpdate()
        {
            if (_Plugins.Count > 0)
                for (int i = 0; i < _Plugins.Count; i++)
                    if (_Plugins[i] != null)
                        try { _Plugins[i].OnLateUpdate(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Plugins[i].Info.Name); }
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].OnLateUpdate(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
        }

        public static void OnGUI()
        {
            if (_Plugins.Count > 0)
                for (int i = 0; i < _Plugins.Count; i++)
                    if (_Plugins[i] != null)
                        try { _Plugins[i].OnGUI(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Plugins[i].Info.Name); }
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].OnGUI(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
        }

        internal static void OnLevelIsLoading()
        {
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].OnLevelIsLoading(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
        }

        internal static void OnLevelWasLoaded(int level)
        {
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].OnLevelWasLoaded(level); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
        }

        internal static void OnLevelWasInitialized(int level)
        {
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].OnLevelWasInitialized(level); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
        }

        private static bool ShouldCheckForUiManager = true;
        private static Type VRCUiManager = null;
        private static MethodInfo VRCUiManager_Instance = null;
        private static void VRChat_CheckUiManager()
        {
            if (!ShouldCheckForUiManager)
                return;
            if (VRCUiManager == null)
                VRCUiManager = Assembly_CSharp.GetType("VRCUiManager");
            if (VRCUiManager == null)
            {
                ShouldCheckForUiManager = false;
                return;
            }
            if (VRCUiManager_Instance == null)
                VRCUiManager_Instance = VRCUiManager.GetMethods().First(x => (x.ReturnType == VRCUiManager));
            if (VRCUiManager_Instance == null)
            {
                ShouldCheckForUiManager = false;
                return;
            }
            object returnval = VRCUiManager_Instance.Invoke(null, new object[0]);
            if (returnval == null)
                return;
            ShouldCheckForUiManager = false;
            if (_Mods.Count > 0)
                for (int i = 0; i < _Mods.Count; i++)
                    if (_Mods[i] != null)
                        try { _Mods[i].VRChat_OnUiManagerInit(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Mods[i].Info.Name); }
            if (_Plugins.Count > 0)
                for (int i = 0; i < _Plugins.Count; i++)
                    if (_Plugins[i] != null)
                        try { _Plugins[i].VRChat_OnUiManagerInit(); } catch (Exception ex) { MelonLogger.LogMelonError(ex.ToString(), _Plugins[i].Info.Name); }
        }

        private enum LoadMode
        {
            NORMAL,
            DEV,
            BOTH
        }
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static LoadMode GetLoadMode_Plugins();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static LoadMode GetLoadMode_Mods();
    }
}
