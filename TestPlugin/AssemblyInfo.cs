using System.Reflection;
using MelonLoader;

[assembly: AssemblyTitle(TestPlugin.BuildInfo.Description)]
[assembly: AssemblyDescription(TestPlugin.BuildInfo.Description)]
[assembly: AssemblyCompany(TestPlugin.BuildInfo.Company)]
[assembly: AssemblyProduct(TestPlugin.BuildInfo.Name)]
[assembly: AssemblyCopyright("Created by " + TestPlugin.BuildInfo.Author)]
[assembly: AssemblyTrademark(TestPlugin.BuildInfo.Company)]
[assembly: AssemblyVersion(TestPlugin.BuildInfo.Version)]
[assembly: AssemblyFileVersion(TestPlugin.BuildInfo.Version)]
[assembly: MelonInfo(typeof(TestPlugin.TestPlugin), TestPlugin.BuildInfo.Name, TestPlugin.BuildInfo.Version, TestPlugin.BuildInfo.Author, TestPlugin.BuildInfo.DownloadLink)]


// Create and Setup a MelonPluginGame to mark a Plugin as Universal or Compatible with specific Games.
// If no MelonPluginGameAttribute is found or any of the Values for any MelonPluginGame on the Mod is null or empty it will be assumed the Plugin is Universal.
// Values for MelonPluginGame can be found in the Game's app.info file or printed at the top of every log directly beneath the Unity version.
[assembly: MelonGame(null, null)]