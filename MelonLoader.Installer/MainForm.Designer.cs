namespace MelonLoader.Installer
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.picTextLogo = new System.Windows.Forms.PictureBox();
            this.picImageLogo = new System.Windows.Forms.PictureBox();
            this.progressBar = new MetroFramework.Controls.MetroProgressBar();
            this.cbVersions = new MetroFramework.Controls.MetroComboBox();
            this.tbPath = new MetroFramework.Controls.MetroTextBox();
            this.tileDivider = new MetroFramework.Controls.MetroTile();
            this.btnInstall = new MetroFramework.Controls.MetroButton();
            this.lblProgressInfo = new MetroFramework.Controls.MetroLabel();
            this.lblProgressPer = new MetroFramework.Controls.MetroLabel();
            this.btnSelect = new MetroFramework.Controls.MetroButton();
            this.btnUninstall = new MetroFramework.Controls.MetroButton();
            ((System.ComponentModel.ISupportInitialize)(this.picTextLogo)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picImageLogo)).BeginInit();
            this.SuspendLayout();
            // 
            // picTextLogo
            // 
            this.picTextLogo.Image = global::MelonLoader.Installer.Properties.Resources.MLText;
            this.picTextLogo.Location = new System.Drawing.Point(23, 134);
            this.picTextLogo.Name = "picTextLogo";
            this.picTextLogo.Size = new System.Drawing.Size(437, 63);
            this.picTextLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.picTextLogo.TabIndex = 0;
            this.picTextLogo.TabStop = false;
            // 
            // picImageLogo
            // 
            this.picImageLogo.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(17)))), ((int)(((byte)(17)))), ((int)(((byte)(17)))));
            this.picImageLogo.Image = global::MelonLoader.Installer.Properties.Resources.Logo;
            this.picImageLogo.Location = new System.Drawing.Point(184, 20);
            this.picImageLogo.Name = "picImageLogo";
            this.picImageLogo.Size = new System.Drawing.Size(120, 109);
            this.picImageLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.picImageLogo.TabIndex = 1;
            this.picImageLogo.TabStop = false;
            // 
            // progressBar
            // 
            this.progressBar.Location = new System.Drawing.Point(23, 248);
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(437, 29);
            this.progressBar.Step = 1;
            this.progressBar.Style = MetroFramework.MetroColorStyle.Green;
            this.progressBar.TabIndex = 10;
            this.progressBar.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.progressBar.Visible = false;
            // 
            // cbVersions
            // 
            this.cbVersions.FormattingEnabled = true;
            this.cbVersions.ItemHeight = 23;
            this.cbVersions.Items.AddRange(new object[] {
            "v0.2.5",
            "v0.2.4",
            "v0.2.3",
            "v0.2.2",
            "v0.2.1",
            "v0.2",
            "v0.1.0"});
            this.cbVersions.Location = new System.Drawing.Point(329, 216);
            this.cbVersions.Name = "cbVersions";
            this.cbVersions.Size = new System.Drawing.Size(131, 29);
            this.cbVersions.Style = MetroFramework.MetroColorStyle.Green;
            this.cbVersions.TabIndex = 13;
            this.cbVersions.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.cbVersions.UseSelectable = true;
            this.cbVersions.SelectedIndexChanged += new System.EventHandler(this.cbVersions_SelectedIndexChanged);
            // 
            // tbPath
            // 
            // 
            // 
            // 
            this.tbPath.CustomButton.Image = null;
            this.tbPath.CustomButton.Location = new System.Drawing.Point(197, 1);
            this.tbPath.CustomButton.Name = "";
            this.tbPath.CustomButton.Size = new System.Drawing.Size(21, 21);
            this.tbPath.CustomButton.Style = MetroFramework.MetroColorStyle.Blue;
            this.tbPath.CustomButton.TabIndex = 1;
            this.tbPath.CustomButton.Theme = MetroFramework.MetroThemeStyle.Light;
            this.tbPath.CustomButton.UseSelectable = true;
            this.tbPath.CustomButton.Visible = false;
            this.tbPath.Lines = new string[] {
        "Please Select your Unity Game\'s EXE..."};
            this.tbPath.Location = new System.Drawing.Point(104, 219);
            this.tbPath.MaxLength = 32767;
            this.tbPath.Name = "tbPath";
            this.tbPath.PasswordChar = '\0';
            this.tbPath.ReadOnly = true;
            this.tbPath.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.tbPath.SelectedText = "";
            this.tbPath.SelectionLength = 0;
            this.tbPath.SelectionStart = 0;
            this.tbPath.ShortcutsEnabled = false;
            this.tbPath.Size = new System.Drawing.Size(219, 23);
            this.tbPath.Style = MetroFramework.MetroColorStyle.Green;
            this.tbPath.TabIndex = 15;
            this.tbPath.Text = "Please Select your Unity Game\'s EXE...";
            this.tbPath.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.tbPath.UseSelectable = true;
            this.tbPath.WaterMarkColor = System.Drawing.Color.FromArgb(((int)(((byte)(109)))), ((int)(((byte)(109)))), ((int)(((byte)(109)))));
            this.tbPath.WaterMarkFont = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Pixel);
            // 
            // tileDivider
            // 
            this.tileDivider.ActiveControl = null;
            this.tileDivider.Enabled = false;
            this.tileDivider.Location = new System.Drawing.Point(23, 203);
            this.tileDivider.Name = "tileDivider";
            this.tileDivider.Size = new System.Drawing.Size(437, 4);
            this.tileDivider.Style = MetroFramework.MetroColorStyle.Black;
            this.tileDivider.TabIndex = 17;
            this.tileDivider.Text = "metroTile1";
            this.tileDivider.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.tileDivider.UseSelectable = true;
            // 
            // btnInstall
            // 
            this.btnInstall.Enabled = false;
            this.btnInstall.Location = new System.Drawing.Point(23, 251);
            this.btnInstall.Name = "btnInstall";
            this.btnInstall.Size = new System.Drawing.Size(437, 45);
            this.btnInstall.Style = MetroFramework.MetroColorStyle.Green;
            this.btnInstall.TabIndex = 18;
            this.btnInstall.Text = "INSTALL";
            this.btnInstall.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.btnInstall.UseSelectable = true;
            this.btnInstall.Click += new System.EventHandler(this.btnInstall_Click);
            // 
            // lblProgressInfo
            // 
            this.lblProgressInfo.FontSize = MetroFramework.MetroLabelSize.Tall;
            this.lblProgressInfo.Location = new System.Drawing.Point(43, 214);
            this.lblProgressInfo.Name = "lblProgressInfo";
            this.lblProgressInfo.Size = new System.Drawing.Size(403, 28);
            this.lblProgressInfo.Style = MetroFramework.MetroColorStyle.Green;
            this.lblProgressInfo.TabIndex = 19;
            this.lblProgressInfo.Text = "Downloading...";
            this.lblProgressInfo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.lblProgressInfo.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.lblProgressInfo.Visible = false;
            // 
            // lblProgressPer
            // 
            this.lblProgressPer.FontSize = MetroFramework.MetroLabelSize.Tall;
            this.lblProgressPer.Location = new System.Drawing.Point(43, 274);
            this.lblProgressPer.Name = "lblProgressPer";
            this.lblProgressPer.Size = new System.Drawing.Size(403, 28);
            this.lblProgressPer.Style = MetroFramework.MetroColorStyle.Green;
            this.lblProgressPer.TabIndex = 20;
            this.lblProgressPer.Text = "0%";
            this.lblProgressPer.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.lblProgressPer.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.lblProgressPer.Visible = false;
            // 
            // btnSelect
            // 
            this.btnSelect.Location = new System.Drawing.Point(23, 216);
            this.btnSelect.Name = "btnSelect";
            this.btnSelect.Size = new System.Drawing.Size(75, 29);
            this.btnSelect.Style = MetroFramework.MetroColorStyle.Green;
            this.btnSelect.TabIndex = 21;
            this.btnSelect.Text = "SELECT";
            this.btnSelect.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.btnSelect.UseSelectable = true;
            this.btnSelect.Click += new System.EventHandler(this.btnSelect_Click_1);
            // 
            // btnUninstall
            // 
            this.btnUninstall.Location = new System.Drawing.Point(245, 251);
            this.btnUninstall.Name = "btnUninstall";
            this.btnUninstall.Size = new System.Drawing.Size(215, 45);
            this.btnUninstall.Style = MetroFramework.MetroColorStyle.Green;
            this.btnUninstall.TabIndex = 22;
            this.btnUninstall.Text = "UN-INSTALL";
            this.btnUninstall.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.btnUninstall.UseSelectable = true;
            this.btnUninstall.Visible = false;
            this.btnUninstall.Click += new System.EventHandler(this.btnUninstall_Click_1);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(481, 314);
            this.Controls.Add(this.btnInstall);
            this.Controls.Add(this.btnUninstall);
            this.Controls.Add(this.btnSelect);
            this.Controls.Add(this.tileDivider);
            this.Controls.Add(this.tbPath);
            this.Controls.Add(this.cbVersions);
            this.Controls.Add(this.picImageLogo);
            this.Controls.Add(this.picTextLogo);
            this.Controls.Add(this.lblProgressPer);
            this.Controls.Add(this.lblProgressInfo);
            this.Controls.Add(this.progressBar);
            this.DisplayHeader = false;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.Padding = new System.Windows.Forms.Padding(20, 30, 20, 20);
            this.Resizable = false;
            this.Style = MetroFramework.MetroColorStyle.Green;
            this.Text = "MelonLoader Installer";
            this.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.MainForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.picTextLogo)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picImageLogo)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.PictureBox picTextLogo;
        private System.Windows.Forms.PictureBox picImageLogo;
        public MetroFramework.Controls.MetroProgressBar progressBar;
        public MetroFramework.Controls.MetroComboBox cbVersions;
        public MetroFramework.Controls.MetroTextBox tbPath;
        public MetroFramework.Controls.MetroTile tileDivider;
        public MetroFramework.Controls.MetroButton btnInstall;
        public MetroFramework.Controls.MetroLabel lblProgressInfo;
        public MetroFramework.Controls.MetroLabel lblProgressPer;
        public MetroFramework.Controls.MetroButton btnSelect;
        public MetroFramework.Controls.MetroButton btnUninstall;
    }
}