using MelonLoader;

namespace TestPlugin
{
    public static class BuildInfo
    {
        public const string Name = "TestPlugin"; // Name of the Plugin.  (MUST BE SET)
        public const string Description = "Plugin for Testing"; // Description for the Plugin.  (Set as null if none)
        public const string Author = null; // Author of the Plugin.  (Set as null if none)
        public const string Company = null; // Company that made the Plugin.  (Set as null if none)
        public const string Version = "1.0.0"; // Version of the Plugin.  (MUST BE SET)
        public const string DownloadLink = null; // Download Link for the Plugin.  (Set as null if none)
    }

    public class TestPlugin : MelonPlugin
    {
        public override void OnPreInitialization() // Runs before Game Initialization.
        {
            MelonLogger.Log("OnPreInitialization");
        }

        public override void OnApplicationStart() // Runs after Game Initialization.
        {
            MelonLogger.Log("OnApplicationStart");
        }

        public override void OnApplicationQuit() // Runs when the Game is told to Close.
        {
            MelonLogger.Log("OnApplicationQuit");
        }

        public override void OnModSettingsApplied() // Runs when Mod Preferences get saved to UserData/modprefs.ini.
        {
            MelonLogger.Log("OnModSettingsApplied");
        }
    }
}