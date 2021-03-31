using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace MelonLoader
{
    public abstract class MelonBase
    {
        /// <summary>
        /// Gets the Assembly of the Mod or Plugin.
        /// </summary>
        public Assembly Assembly { get; internal set; }

        /// <summary>
        /// Gets the File Location of the Mod or Plugin.
        /// </summary>
        public string Location { get; internal set; }

        /// <summary>
        /// Enum for Melon Compatibility
        /// </summary>
        public enum MelonCompatibility
        {
            UNIVERSAL = 0,
            COMPATIBLE = 1,
            NOATTRIBUTE = 2,
            INCOMPATIBLE = 3,
        }

        /// <summary>
        /// Gets the Compatibility of the Mod or Plugin.
        /// </summary>
        public MelonCompatibility Compatibility { get; internal set; }

        /// <summary>
        /// Gets the Info Attribute of the Mod or Plugin.
        /// </summary>
        public MelonInfoAttribute Info { get; internal set; }

        [Obsolete()]
        internal MelonModInfoAttribute LegacyModInfo { get; set; }
        [Obsolete()]
        internal MelonPluginInfoAttribute LegacyPluginInfo { get; set; }

        /// <summary>
        /// Gets the Game Attributes of the Mod or Plugin.
        /// </summary>
        public MelonOptionalDependenciesAttribute OptionalDependenciesAttribute { get; internal set; }

        /// <summary>
        /// Gets the Game Attributes of the Mod or Plugin.
        /// </summary>
        public MelonGameAttribute[] Games { get; internal set; }

        /// <summary>
        /// Gets the Auto-Created Harmony Instance of the Mod or Plugin.
        /// </summary>
        public Harmony.HarmonyInstance harmonyInstance { get; internal set; }

        [Obsolete()]
        internal MelonModGameAttribute[] LegacyModGames { get; set; }
        [Obsolete()]
        internal MelonPluginGameAttribute[] LegacyPluginGames { get; set; }

        public virtual void OnApplicationStart() { }
        public virtual void OnUpdate() { }
        public virtual void OnLateUpdate() { }
        public virtual void OnGUI() { }
        public virtual void OnApplicationQuit() { }
        public virtual void OnModSettingsApplied() { }
        public virtual void VRChat_OnUiManagerInit() { }
    }

    [AttributeUsage(AttributeTargets.Assembly, AllowMultiple = false)]
    public class MelonOptionalDependenciesAttribute : Attribute
    {
        /// <summary>
        /// The (simple) assembly names of the dependencies that should be regarded as optional.
        /// </summary>
        public string[] AssemblyNames { get; internal set; }

        public MelonOptionalDependenciesAttribute(params string[] assemblyNames)
        {
            AssemblyNames = assemblyNames;
        }
    }

    [AttributeUsage(AttributeTargets.Assembly, AllowMultiple = false)]
    public class MelonInfoAttribute : Attribute
    {
        /// <summary>
        /// Gets the System.Type of the Mod.
        /// </summary>
        public Type SystemType { get; internal set; }

        /// <summary>
        /// Gets the Name of the Mod.
        /// </summary>
        public string Name { get; internal set; }

        /// <summary>
        /// Gets the Version of the Mod.
        /// </summary>
        public string Version { get; internal set; }

        /// <summary>
        /// Gets the Author of the Mod.
        /// </summary>
        public string Author { get; internal set; }

        /// <summary>
        /// Gets the Download Link of the Mod.
        /// </summary>
        public string DownloadLink { get; internal set; }

        public MelonInfoAttribute(Type type, string name, string version, string author, string downloadLink = null)
        {
            SystemType = type;
            Name = name;
            Version = version;
            Author = author;
            DownloadLink = downloadLink;
        }

        [Obsolete()]
        internal MelonModInfoAttribute ConvertLegacy_Mod() => new MelonModInfoAttribute(SystemType, Name, Version, Author, DownloadLink);
        [Obsolete()]
        internal MelonPluginInfoAttribute ConvertLegacy_Plugin() => new MelonPluginInfoAttribute(SystemType, Name, Version, Author, DownloadLink);
    }

    [AttributeUsage(AttributeTargets.Assembly, AllowMultiple = true)]
    public class MelonGameAttribute : Attribute
    {
        /// <summary>
        /// Gets the target Developer
        /// </summary>
        public string Developer { get; internal set; }

        /// <summary>
        /// Gets target Game Name
        /// </summary>
        public string GameName { get; internal set; }

        /// <summary>
        /// Gets whether this Mod can target any Game.
        /// </summary>
        public bool Universal { get => string.IsNullOrEmpty(Developer) || string.IsNullOrEmpty(GameName); }

        /// <summary>
        /// Mark this Mod as Universal or Compatible with specific Games.
        /// </summary>
        public MelonGameAttribute(string developer = null, string gameName = null)
        {
            Developer = developer;
            GameName = gameName;
        }

        [Obsolete()]
        internal MelonModGameAttribute ConvertLegacy_Mod() => new MelonModGameAttribute(Developer, GameName);
        [Obsolete()]
        internal MelonPluginGameAttribute ConvertLegacy_Plugin() => new MelonPluginGameAttribute(Developer, GameName);

        public bool IsGame(string developer, string gameName) => (Universal || ((developer != null) && (gameName != null) && Developer.Equals(developer) && GameName.Equals(gameName)));
        public bool IsCompatible(MelonGameAttribute att) => ((att == null) || IsCompatibleBecauseUniversal(att) || (att.Developer.Equals(Developer) && att.GameName.Equals(GameName)));
        public bool IsCompatibleBecauseUniversal(MelonGameAttribute att) => ((att == null) || Universal || att.Universal);
    }

    [Obsolete()]
    internal class MelonLegacyAttributeSupport
    {
        internal class Response_Info
        {
            internal MelonInfoAttribute Default;
            [Obsolete()]
            internal MelonModInfoAttribute Legacy_Mod;
            [Obsolete()]
            internal MelonPluginInfoAttribute Legacy_Plugin;
            internal Response_Info(MelonInfoAttribute def, MelonModInfoAttribute legacy_mod, MelonPluginInfoAttribute legacy_plugin)
            {
                Default = def;
                Legacy_Mod = legacy_mod;
                Legacy_Plugin = legacy_plugin;
            }
            internal void SetupMelon(MelonBase baseInstance)
            {
                if (Default != null)
                    baseInstance.Info = Default;
                if (Legacy_Mod != null)
                    baseInstance.LegacyModInfo = Legacy_Mod;
                if (Legacy_Plugin != null)
                    baseInstance.LegacyPluginInfo = Legacy_Plugin;
            }
        }

        internal class Response_Game
        {
            internal MelonGameAttribute[] Default;
            internal MelonModGameAttribute[] Legacy_Mod;
            internal MelonPluginGameAttribute[] Legacy_Plugin;
            internal Response_Game(MelonGameAttribute[] def, MelonModGameAttribute[] legacy_mod, MelonPluginGameAttribute[] legacy_plugin)
            {
                Default = def;
                Legacy_Mod = legacy_mod;
                Legacy_Plugin = legacy_plugin;
            }
            internal void SetupMelon(MelonBase baseInstance)
            {
                if (Default.Length > 0)
                    baseInstance.Games = Default;
                if (Legacy_Mod.Length > 0)
                    baseInstance.LegacyModGames = Legacy_Mod;
                if (Legacy_Plugin.Length > 0)
                    baseInstance.LegacyPluginGames = Legacy_Plugin;
            }
        }

        internal static Response_Info GetMelonInfoAttribute(Assembly asm, bool isPlugin = false)
        {
            MelonInfoAttribute def = asm.GetCustomAttributes(false).FirstOrDefault(x => (x.GetType() == typeof(MelonInfoAttribute))) as MelonInfoAttribute;
            MelonModInfoAttribute legacy_mod = null;
            MelonPluginInfoAttribute legacy_plugin = null;
            if (def == null)
            {
                if (isPlugin)
                {
                    legacy_plugin = asm.GetCustomAttributes(false).FirstOrDefault(x => (x.GetType() == typeof(MelonPluginInfoAttribute))) as MelonPluginInfoAttribute;
                    if ((legacy_plugin != null) && (def == null))
                        def = legacy_plugin.Convert();
                }
                else
                {
                    legacy_mod = asm.GetCustomAttributes(false).FirstOrDefault(x => (x.GetType() == typeof(MelonModInfoAttribute))) as MelonModInfoAttribute;
                    if ((legacy_mod != null) && (def == null))
                        def = legacy_mod.Convert();
                }
            }
            else
            {
                if (isPlugin)
                    legacy_plugin = def.ConvertLegacy_Plugin();
                else
                    legacy_mod = def.ConvertLegacy_Mod();
            }
            return new Response_Info(def, legacy_mod, legacy_plugin);
        }

        internal static Response_Game GetMelonGameAttributes(Assembly asm, bool isPlugin = false)
        {
            MelonGameAttribute[] def = asm.GetCustomAttributes(typeof(MelonGameAttribute), true) as MelonGameAttribute[];
            MelonModGameAttribute[] legacy_mod = new MelonModGameAttribute[0];
            MelonPluginGameAttribute[] legacy_plugin = new MelonPluginGameAttribute[0];
            if (def.Length <= 0)
            {
                if (isPlugin)
                {
                    legacy_plugin = asm.GetCustomAttributes(typeof(MelonPluginGameAttribute), true) as MelonPluginGameAttribute[];
                    if (legacy_plugin.Length > 0)
                    {
                        List<MelonGameAttribute> deflist = new List<MelonGameAttribute>();
                        foreach (MelonPluginGameAttribute att in legacy_plugin)
                            deflist.Add(att.Convert());
                        def = deflist.ToArray();
                    }
                }
                else
                {
                    legacy_mod = asm.GetCustomAttributes(typeof(MelonModGameAttribute), true) as MelonModGameAttribute[];
                    if (legacy_mod.Length > 0)
                    {
                        List<MelonGameAttribute> deflist = new List<MelonGameAttribute>();
                        foreach (MelonModGameAttribute att in legacy_mod)
                            deflist.Add(att.Convert());
                        def = deflist.ToArray();
                    }
                }
            }
            else
            {
                if (isPlugin)
                {
                    List<MelonPluginGameAttribute> legacy_pluginlist = new List<MelonPluginGameAttribute>();
                    foreach (MelonGameAttribute att in def)
                        legacy_pluginlist.Add(att.ConvertLegacy_Plugin());
                    legacy_plugin = legacy_pluginlist.ToArray();
                }
                else
                {
                    List<MelonModGameAttribute> legacy_modlist = new List<MelonModGameAttribute>();
                    foreach (MelonGameAttribute att in def)
                        legacy_modlist.Add(att.ConvertLegacy_Mod());
                    legacy_mod = legacy_modlist.ToArray();
                }
            }
            return new Response_Game(def, legacy_mod, legacy_plugin);
        }
    }
}
