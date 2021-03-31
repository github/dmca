using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Text;
using System.Windows.Forms;
using MelonLoader.LightJson;
#pragma warning disable 0168

namespace MelonLoader.Installer
{
    static class Program
    {
        internal static bool ShouldCheckForUpdates = false;
        internal static string Title = "MelonLoader Installer";
        private static string ExeName = null;
        internal static string ManualZipPath = null;
        internal static MainForm mainForm = null;
        internal static WebClient webClient = new WebClient();

        private static readonly string[] filesToCleanUp = new string[] { "Mono.Cecil.dll", "version.dll", "winmm.dll" };
        private static readonly string[] foldersToCleanUp = new string[] { "Logs", "MelonLoader" };

        public static bool silent;
        public static string silentPath;

        [STAThread]
        static void Main()
        {
            ServicePointManager.Expect100Continue = true;
            ServicePointManager.SecurityProtocol = SecurityProtocolType.Ssl3 | SecurityProtocolType.Tls | SecurityProtocolType.Tls11 | SecurityProtocolType.Tls12 | (SecurityProtocolType)3072;
            webClient.Headers.Add("User-Agent", "Unity web player");
            webClient.DownloadProgressChanged += (object sender, DownloadProgressChangedEventArgs info) => SetPercentage(info.ProgressPercentage);
            Application.SetCompatibleTextRenderingDefault(false);
            Application.EnableVisualStyles();

            var args = Environment.GetCommandLineArgs();
            foreach (string arg in args)
            {
                bool isSelf = Path.GetFullPath(arg) == Assembly.GetExecutingAssembly().Location;
                if (File.Exists(arg) && arg.EndsWith(".exe") && !isSelf)
                {
                    silent = true;
                    silentPath = Path.GetDirectoryName(arg);
                }
            }

            ExeName = Path.GetFileName(Process.GetCurrentProcess().MainModule.FileName);
            if (ShouldCheckForUpdates && ExeName.EndsWith(".tmp.exe"))
            {
                string original_exe_path = (Directory.GetCurrentDirectory() + "\\" + ExeName.Substring(0, (ExeName.Length - 8)));
                File.Delete(original_exe_path);
                File.Copy((Directory.GetCurrentDirectory() + "\\" + ExeName), original_exe_path);
                Process.Start(original_exe_path);
                Application.Exit();
            }
            else
            {
                if (ShouldCheckForUpdates)
                {
                    string tempfilepath = (Directory.GetCurrentDirectory() + "\\" + ExeName + ".tmp.exe");
                    if (File.Exists(tempfilepath))
                        File.Delete(tempfilepath);
                }
                ManualZipPath = (Directory.GetCurrentDirectory() + "\\MelonLoader.zip");

                if (silent)
                    Install_NoGUI();
                else
                    Install_GUI();
            }
        }

        static void Install_NoGUI()
        {
            try
            {
                string version = null;
                JsonArray data = (JsonArray)JsonValue.Parse(webClient.DownloadString("https://api.github.com/repos/HerpDerpinstine/MelonLoader/releases")).AsJsonArray;
                if (data.Count > 0)
                {
                    foreach (var x in data)
                    {
                        string versionstr = x["tag_name"].AsString;
                        if (!versionstr.StartsWith("v0.2") && !versionstr.StartsWith("v0.1"))
                            continue;
                        version = versionstr;
                        break;
                    }
                }
                if (version == null)
                    return;
                try
                {
                    Install(silentPath, version, false, true);
                    Environment.Exit(0);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Installation failed; upload the created log to #melonloader-support on discord", Title);
                    File.WriteAllText(Directory.GetCurrentDirectory() + $@"\MLInstaller_{DateTime.Now:yy-M-dd_HH-mm-ss.fff}.log", ex.ToString());
                    Environment.Exit(-1);
                }
                
            }
            catch (Exception ex)
            {
                MessageBox.Show("Failed to Get Version List; upload the created log to #melonloader-support on discord", Title);
                File.WriteAllText(Directory.GetCurrentDirectory() + $@"\MLInstaller_{DateTime.Now:yy-M-dd_HH-mm-ss.fff}.log", ex.ToString());
                Environment.Exit(-1);
            }
        }

        static void Install_GUI()
        {
            int selected_index = 0;

            try
            {
                mainForm = new MainForm();
                mainForm.cbVersions.Items.Clear();

                bool should_add_manual = File.Exists(ManualZipPath);
                if (should_add_manual)
                    mainForm.cbVersions.Items.Add("Manual Zip");
                JsonArray data = (JsonArray)JsonValue.Parse(webClient.DownloadString("https://api.github.com/repos/HerpDerpinstine/MelonLoader/releases")).AsJsonArray;
                if (data.Count > 0)
                {
                    bool has_added_latest = false;
                    foreach (var x in data)
                    {
                        string version = x["tag_name"].AsString;
                        if (!version.StartsWith("v0.2") && !version.StartsWith("v0.1"))
                            continue;
                        if (!has_added_latest)
                        {
                            has_added_latest = true;
                            mainForm.cbVersions.Items.Add("Latest (" + version + ")");
                            selected_index = (should_add_manual ? 1 : 0);
                        }
                        mainForm.cbVersions.Items.Add(version);
                    }
                }
                if (mainForm.cbVersions.Items.Count <= 0)
                    throw new Exception("Version List is Empty!");
            }
            catch (Exception ex)
            {
                MessageBox.Show("Failed to Get Version List; upload the created log to #melonloader-support on discord", Title);
                File.WriteAllText(Directory.GetCurrentDirectory() + $@"\MLInstaller_{DateTime.Now:yy-M-dd_HH-mm-ss.fff}.log", ex.ToString());
                Application.Exit();
            }

            if ((selected_index < 0) || (selected_index > (mainForm.cbVersions.Items.Count - 1)))
                selected_index = 0;
            mainForm.cbVersions.SelectedIndex = selected_index;
            mainForm.cbVersions.SelectedItem = mainForm.cbVersions.Items[selected_index];
            mainForm.Show();
            Application.Run(mainForm);
        }

        internal static void Install(string dirpath, string selectedVersion, bool legacy_install, bool silent_install)
        {
            if (File.Exists(ManualZipPath) && selectedVersion.Equals("Manual Zip"))
                Install_ManualZip(dirpath);
            else if (legacy_install)
                Install_Legacy(dirpath, selectedVersion);
            else if (silent_install)
                Install_Silent(dirpath, selectedVersion);
            else
                Install_Normal(dirpath, selectedVersion);
            TempFileCache.ClearCache();
        }

        private static void Install_ManualZip(string dirpath)
        {
            SetDisplayText("Extracting MelonLoader.zip...");
            Cleanup(dirpath);
            ExtractZip(dirpath, ManualZipPath);
            CreateDirectories(dirpath, "Manual Zip", false);
        }

        private static void Install_Legacy(string dirpath, string selectedVersion)
        {
            if (selectedVersion.Equals("v0.1.0"))
                Install_Legacy_01(dirpath);
            else
                Install_Legacy_02(dirpath, selectedVersion);
        }

        private static void Install_Silent(string dirpath, string selectedVersion)
        {
            string tempfilepath = TempFileCache.CreateFile();
            webClient.DownloadFileAsync(new Uri("https://github.com/HerpDerpinstine/MelonLoader/releases/download/" + selectedVersion + "/MelonLoader.zip"), tempfilepath);
            while (webClient.IsBusy) { }
            Cleanup(dirpath);
            ExtractZip(dirpath, tempfilepath);
            CreateDirectories(dirpath, selectedVersion, false);
        }

        internal static void SetDisplayText(string text)
        {
            mainForm.Invoke(new Action(() => {
                mainForm.lblProgressInfo.Text = text; 
            }));
        }

        internal static void SetPercentage(int percent)
        {
            if (silent)
                return;

            mainForm.Invoke(new Action(() => {
                mainForm.progressBar.Value = percent;
                mainForm.lblProgressPer.Text = percent.ToString() + "%";
            }));
        }

        internal static void Cleanup(string dirpath)
        {
            try
            {
                foreach (string file in filesToCleanUp)
                {
                    if (File.Exists(Path.Combine(dirpath, file)))
                        File.Delete(Path.Combine(dirpath, file));
                }

                foreach (string folder in foldersToCleanUp)
                {
                    if (Directory.Exists(Path.Combine(dirpath, folder)))
                        Directory.Delete(Path.Combine(dirpath, folder), true);
                }
            }
            catch (UnauthorizedAccessException e)
            {
                DialogResult result = MessageBox.Show($"MelonLoader could not remove old files.{Environment.NewLine}Please close the game then click retry to try again.{Environment.NewLine}If issue persists please click cancel to dump logs.", Program.Title, MessageBoxButtons.RetryCancel, MessageBoxIcon.Error);
                if (result == DialogResult.Cancel)
                    throw e;
            }
        }

        private static void CreateDirectories(string dirpath, string selectedVersion, bool legacy_install)
        {
            Directory.CreateDirectory(Path.Combine(dirpath, "Logs"));
            if (!Directory.Exists(Path.Combine(dirpath, "Mods")))
                Directory.CreateDirectory(Path.Combine(dirpath, "Mods"));
            if (!legacy_install && !selectedVersion.Equals("v0.2.2") && !Directory.Exists(Path.Combine(dirpath, "Plugins")))
                Directory.CreateDirectory(Path.Combine(dirpath, "Plugins"));
        }

        private static void ExtractZip(string dirpath, string tempFile)
        {
            SetPercentage(0);
            using var stream = new FileStream(tempFile, FileMode.Open, FileAccess.Read);
            using var zip = new ZipArchive(stream);
            int total_entry_count = zip.Entries.Count;
            string fullName = Directory.CreateDirectory(dirpath).FullName;
            for (int i = 0; i < total_entry_count; i++)
            {
                ZipArchiveEntry entry = zip.Entries[i];
                string fullPath = Path.GetFullPath(Path.Combine(fullName, entry.FullName));
                if (!fullPath.StartsWith(fullName))
                    throw new IOException("Extracting Zip entry would have resulted in a file outside the specified destination directory.");
                if (Path.GetFileName(fullPath).Length != 0)
                {
                    Directory.CreateDirectory(Path.GetDirectoryName(fullPath));
                    entry.ExtractToFile(fullPath, true);
                }
                else
                {
                    if (entry.Length != 0)
                        throw new IOException("Zip entry name ends in directory separator character but contains data.");
                    Directory.CreateDirectory(fullPath);
                }
                SetPercentage(((i + 1) / total_entry_count) * 100);
            }
        }

        private static void Install_Normal(string dirpath, string selectedVersion)
        {
            SetDisplayText("Downloading MelonLoader...");
            string tempfilepath = TempFileCache.CreateFile();
            webClient.DownloadFileAsync(new Uri("https://github.com/HerpDerpinstine/MelonLoader/releases/download/" + selectedVersion + "/MelonLoader.zip"), tempfilepath);
            while (webClient.IsBusy) { }
            SetDisplayText("Extracting MelonLoader...");
            Cleanup(dirpath);
            ExtractZip(dirpath, tempfilepath);
            CreateDirectories(dirpath, selectedVersion, false);
        }

        private static void Install_Legacy_02(string dirpath, string selectedVersion)
        {
            SetDisplayText("Downloading MelonLoader...");
            bool is_02 = selectedVersion.Equals("v0.2");
            string tempfilepath = TempFileCache.CreateFile();
            webClient.DownloadFileAsync(new Uri("https://github.com/HerpDerpinstine/MelonLoader/releases/download/" + selectedVersion + "/MelonLoader" + (is_02 ? "_" : ".") + (File.Exists(Path.Combine(dirpath, "GameAssembly.dll")) ? "Il2Cpp" : "Mono") + ".zip"), tempfilepath);
            while (webClient.IsBusy) { }
            SetDisplayText("Extracting MelonLoader...");
            Cleanup(dirpath);
            ExtractZip(dirpath, tempfilepath);

            if (is_02)
            {
                string AssemblyGenerator_Folder = Path.Combine(Path.Combine(dirpath, "MelonLoader"), "AssemblyGenerator");
                string Il2CppDumper_Folder = Path.Combine(AssemblyGenerator_Folder, "Il2CppDumper");
                string Il2CppAssemblyUnhollower_Folder = Path.Combine(AssemblyGenerator_Folder, "Il2CppAssemblyUnhollower");
                string UnityDependencies_Folder = Path.Combine(Il2CppAssemblyUnhollower_Folder, "UnityDependencies");

                SetDisplayText("Downloading Il2CppDumper...");
                string tempfilepath2 = TempFileCache.CreateFile();
                webClient.DownloadFileAsync(new Uri("https://github.com/Perfare/Il2CppDumper/releases/download/v6.2.1/Il2CppDumper-v6.2.1.zip"), tempfilepath2);
                while (webClient.IsBusy) { }

                SetDisplayText("Downloading Il2CppUnhollower...");
                string tempfilepath3 = TempFileCache.CreateFile();
                webClient.DownloadFileAsync(new Uri("https://github.com/knah/Il2CppAssemblyUnhollower/releases/download/v0.4.3.0/Il2CppAssemblyUnhollower.0.4.3.0.zip"), tempfilepath3);
                while (webClient.IsBusy) { }

                SetDisplayText("Downloading Dependencies...");
                string tempfilepath4 = TempFileCache.CreateFile();
                bool run_fallback = false;
                try { webClient.DownloadFileAsync(new Uri("https://github.com/HerpDerpinstine/MelonLoader/raw/master/BaseLibs/UnityDependencies/" + mainForm.UnityVersion + ".zip"), tempfilepath4); while (webClient.IsBusy) { } } catch (Exception ex) { run_fallback = true; }
                if (run_fallback)
                {
                    string subver = mainForm.UnityVersion.Substring(0, mainForm.UnityVersion.LastIndexOf("."));
                    JsonArray data = (JsonArray)JsonValue.Parse(Program.webClient.DownloadString("https://api.github.com/repos/HerpDerpinstine/MelonLoader/contents/BaseLibs/UnityDependencies")).AsJsonArray;
                    if (data.Count > 0)
                    {
                        List<string> versionlist = new List<string>();
                        foreach (var x in data)
                        {
                            string version = Path.GetFileNameWithoutExtension(x["name"].AsString);
                            if (version.StartsWith(subver))
                                versionlist.Add(version);
                        }
                        if (versionlist.Count > 0)
                        {
                            versionlist = versionlist.OrderBy(x => int.Parse(x.Split(new char[] { '.' })[2])).ToList();
                            string latest_version = versionlist.Last();
                            webClient.DownloadFileAsync(new Uri("https://github.com/HerpDerpinstine/MelonLoader/raw/master/BaseLibs/UnityDependencies/" + latest_version + ".zip"), tempfilepath4);
                            while (webClient.IsBusy) { }
                        }
                    }
                }

                SetDisplayText("Extracting Il2CppDumper...");
                ExtractZip(Il2CppDumper_Folder, tempfilepath2);

                SetDisplayText("Extracting Il2CppUnhollower...");
                ExtractZip(Il2CppAssemblyUnhollower_Folder, tempfilepath3);

                SetDisplayText("Extracting Dependencies...");
                ExtractZip(UnityDependencies_Folder, tempfilepath4);
            }

            CreateDirectories(dirpath, selectedVersion, true);
        }

        private static void Install_Legacy_01(string dirpath)
        {
            SetDisplayText("Downloading MelonLoader...");
            string tempfilepath = TempFileCache.CreateFile();
            webClient.DownloadFileAsync(new Uri("https://github.com/HerpDerpinstine/MelonLoader/releases/download/v0.1.0/MelonLoader.zip"), tempfilepath);
            while (webClient.IsBusy) { }

            SetDisplayText("Downloading Dependencies...");
            string tempfilepath2 = TempFileCache.CreateFile();
            webClient.DownloadFileAsync(new Uri("https://github.com/HerpDerpinstine/MelonLoader/releases/download/v0.1.0/MonoDependencies.zip"), tempfilepath2);
            while (webClient.IsBusy) { }

            SetDisplayText("Extracting MelonLoader...");
            Cleanup(dirpath);
            ExtractZip(dirpath, tempfilepath);

            SetDisplayText("Extracting Dependencies...");
            ExtractZip(dirpath, tempfilepath2);

            CreateDirectories(dirpath, "v0.1.0", true);
        }

        internal static string GetUnityVersion(string exepath)
        {
            string ggm_path = Path.Combine(Path.GetDirectoryName(exepath), (Path.GetFileNameWithoutExtension(exepath) + "_Data"), "globalgamemanagers");
            if (!File.Exists(ggm_path))
            {
                FileVersionInfo versioninfo = FileVersionInfo.GetVersionInfo(exepath);
                if ((versioninfo == null) || string.IsNullOrEmpty(versioninfo.FileVersion))
                    return "UNKNOWN";
                return versioninfo.FileVersion.Substring(0, versioninfo.FileVersion.LastIndexOf('.'));
            }
            else
            {
                byte[] ggm_bytes = File.ReadAllBytes(ggm_path);
                if ((ggm_bytes == null) || (ggm_bytes.Length <= 0))
                    return "UNKNOWN";
                int start_position = 0;
                for (int i = 10; i < ggm_bytes.Length; i++)
                {
                    byte pos_byte = ggm_bytes[i];
                    if ((pos_byte <= 0x39) && (pos_byte >= 0x30))
                    {
                        start_position = i;
                        break;
                    }
                }
                if (start_position == 0)
                    return "UNKNOWN";
                int end_position = 0;
                for (int i = start_position; i < ggm_bytes.Length; i++)
                {
                    byte pos_byte = ggm_bytes[i];
                    if ((pos_byte != 0x2E) && ((pos_byte > 0x39) || (pos_byte < 0x30)))
                    {
                        end_position = (i - 1);
                        break;
                    }
                }
                if (end_position == 0)
                    return "UNKNOWN";
                int verstr_byte_pos = 0;
                byte[] verstr_byte = new byte[((end_position - start_position) + 1)];
                for (int i = start_position; i <= end_position; i++)
                {
                    verstr_byte[verstr_byte_pos] = ggm_bytes[i];
                    verstr_byte_pos++;
                }
                return Encoding.UTF8.GetString(verstr_byte, 0, verstr_byte.Length);
            }
        }

        internal static void CheckForUpdates()
        {
            if (!ShouldCheckForUpdates)
                return;
            try
            {
                string response = webClient.DownloadString("https://github.com/HerpDerpinstine/MelonLoader/raw/master/MelonLoader.Installer/MelonLoader.Installer.csproj");
                if (string.IsNullOrEmpty(response))
                    return;
                string response_p1 = response.Substring(response.IndexOf("<Version>") + 9);
                string response_p2 = response_p1.Substring(0, response_p1.IndexOf("</Version>"));
                if (!response_p2.Equals(FileVersionInfo.GetVersionInfo(Process.GetCurrentProcess().MainModule.FileName).ProductVersion))
                {
                    string tempfilepath = (Directory.GetCurrentDirectory() + "\\" + ExeName + ".tmp.exe");
                    if (File.Exists(tempfilepath))
                        File.Delete(tempfilepath);
                    webClient.DownloadFileAsync(new Uri("https://github.com/HerpDerpinstine/MelonLoader/releases/latest/download/MelonLoader.Installer.exe"), tempfilepath);
                    while (webClient.IsBusy) { }
                    Process.Start(tempfilepath);
                    Application.Exit();
                }
            }
            catch (Exception e) {}
        }
    }
}