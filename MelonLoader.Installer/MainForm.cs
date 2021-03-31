using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Windows.Forms;

namespace MelonLoader.Installer
{
    public partial class MainForm : MetroFramework.Forms.MetroForm
    {
        internal string CurrentVersion = null;
        internal string UnityVersion = null;

        public MainForm()
        {
            InitializeComponent();
            Text = Program.Title;
            cbVersions.SelectedIndex = 0;
        }

        private void MainForm_Load(object sender, EventArgs e) => Program.CheckForUpdates();

        private void btnSelect_Click_1(object sender, EventArgs e)
        {
            using (OpenFileDialog openFileDialog = new OpenFileDialog())
            {
                openFileDialog.Filter = "Unity Game (*.exe)|*.exe";
                openFileDialog.RestoreDirectory = true;
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    string filePath = openFileDialog.FileName;
                    if (!string.IsNullOrEmpty(filePath))
                    {
                        // Check if Game Selected actually is a Unity Game

                        tbPath.Text = filePath;
                        btnInstall.Enabled = true;
                        UnityVersion = Program.GetUnityVersion(filePath);

                        string existingFilePath = Path.Combine(Path.Combine(Path.GetDirectoryName(filePath), "MelonLoader"), "MelonLoader.ModHandler.dll");
                        if (File.Exists(existingFilePath))
                        {
                            string file_version = FileVersionInfo.GetVersionInfo(existingFilePath).FileVersion;
                            if (file_version.IndexOf(".0") >= 0)
                                file_version = file_version.Substring(0, file_version.IndexOf(".0"));
                            CurrentVersion = "v" + file_version;
                            System.Drawing.Size btnInstallSize = btnInstall.Size;
                            btnInstallSize.Width = 215;
                            btnInstall.Size = btnInstallSize;
                            btnUninstall.Show();
                            VersionCheck();
                        }
                        else
                        {
                            System.Drawing.Size btnInstallSize = btnInstall.Size;
                            btnInstallSize.Width = 437;
                            btnInstall.Size = btnInstallSize;
                            btnUninstall.Hide();
                            CurrentVersion = null;
                            btnInstall.Text = "INSTALL";
                        }
                    }
                }
            }
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if ((e.CloseReason == CloseReason.WindowsShutDown) || (e.CloseReason == CloseReason.UserClosing) || (e.CloseReason == CloseReason.TaskManagerClosing))
            {
                TempFileCache.ClearCache();
                Process.GetCurrentProcess().Kill();
            }
        }


        private void cbVersions_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (btnInstall.Enabled && !string.IsNullOrEmpty(CurrentVersion))
                VersionCheck();
        }

        private void VersionCheck()
        {
            string selectedVersion = ((((string)cbVersions.Items[cbVersions.SelectedIndex]).StartsWith("Latest")) ? (string)cbVersions.Items[cbVersions.SelectedIndex + 1] : (string)cbVersions.Items[cbVersions.SelectedIndex]);
            if (selectedVersion.Equals("Manual Zip") || string.IsNullOrEmpty(CurrentVersion))
                btnInstall.Text = "INSTALL";
            else if (CurrentVersion.Equals(selectedVersion))
                    btnInstall.Text = "RE-INSTALL";
            else
            {
                int index = cbVersions.FindString(CurrentVersion);
                if ((index != -1) && (index < cbVersions.SelectedIndex))
                    btnInstall.Text = "INSTALL";
                else
                    btnInstall.Text = "UPDATE";
            }
        }

        private void btnInstall_Click(object sender, EventArgs e)
        {
            cbVersions.Hide();
            tbPath.Hide();
            btnSelect.Hide();
            btnInstall.Hide();
            btnUninstall.Hide();

            progressBar.Show();
            lblProgressInfo.Show();
            lblProgressPer.Show();

            new Thread(() =>
            {
                try
                {
                    string dirpath = Path.GetDirectoryName(tbPath.Text);
                    string selectedVersion = ((((string)cbVersions.Items[cbVersions.SelectedIndex]).StartsWith("Latest")) ? (string)cbVersions.Items[cbVersions.SelectedIndex + 1] : (string)cbVersions.Items[cbVersions.SelectedIndex]);
                    bool legacy_install = (selectedVersion.Equals("v0.2.1") || selectedVersion.Equals("v0.2") || selectedVersion.Equals("v0.1.0"));

                    Program.Install(dirpath, selectedVersion, legacy_install, false);

                    Program.SetDisplayText("SUCCESS!");
                    MessageBox.Show("Installation Successful!", Program.Title, MessageBoxButtons.OK, MessageBoxIcon.Asterisk, MessageBoxDefaultButton.Button1, MessageBoxOptions.DefaultDesktopOnly);
                    Close();
                    Application.Exit();
                }
                catch (Exception ex)
                {
                    TempFileCache.ClearCache();
                    Program.SetDisplayText("ERROR!");
                    MessageBox.Show("Installation failed; upload the created log to #melonloader-support on discord", Program.Title);
                    File.WriteAllText(Directory.GetCurrentDirectory() + $@"\MLInstaller_{DateTime.Now:yy-M-dd_HH-mm-ss.fff}.log", ex.ToString());
                    Close();
                    Application.Exit();
                }
            }).Start();
        }

        private void btnUninstall_Click_1(object sender, EventArgs e)
        {
            cbVersions.Hide();
            tbPath.Hide();
            btnSelect.Hide();
            btnInstall.Hide();
            btnUninstall.Hide();

            progressBar.Show();
            lblProgressInfo.Show();
            lblProgressPer.Show();

            new Thread(() =>
            {
                try
                {
                    string dirpath = Path.GetDirectoryName(tbPath.Text);

                    Program.Cleanup(dirpath);
                    
                    Program.SetDisplayText("SUCCESS!");
                    Program.SetPercentage(100);
                    MessageBox.Show("Uninstall Successful!", Program.Title, MessageBoxButtons.OK, MessageBoxIcon.Asterisk, MessageBoxDefaultButton.Button1, MessageBoxOptions.DefaultDesktopOnly);
                    Close();
                    Application.Exit();
                }
                catch (Exception ex)
                {
                    TempFileCache.ClearCache();
                    Program.SetDisplayText("ERROR!");
                    MessageBox.Show("Uninstall failed; upload the created log to #melonloader-support on discord", Program.Title);
                    File.WriteAllText(Directory.GetCurrentDirectory() + $@"\MLInstaller_{DateTime.Now:yy-M-dd_HH-mm-ss.fff}.log", ex.ToString());
                    Close();
                    Application.Exit();
                }
            }).Start();
        }
    }
}