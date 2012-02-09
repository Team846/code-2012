namespace KeyDetection
{
    partial class Viewer
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
            this.pic_Original = new System.Windows.Forms.PictureBox();
            this.pic_Thresholded = new System.Windows.Forms.PictureBox();
            this.menu_Strip = new System.Windows.Forms.MenuStrip();
            this.menu_File = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_File_Open = new System.Windows.Forms.ToolStripMenuItem();
            this.saveImageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveOriginalToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveThresholdedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveEdgedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveProcessedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_Options = new System.Windows.Forms.ToolStripMenuItem();
            this.status_Progress = new System.Windows.Forms.ProgressBar();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label1 = new System.Windows.Forms.Label();
            this.pic_Edged = new System.Windows.Forms.PictureBox();
            this.panel3 = new System.Windows.Forms.Panel();
            this.button1 = new System.Windows.Forms.Button();
            this.pic_Processed = new System.Windows.Forms.PictureBox();
            this.panel4 = new System.Windows.Forms.Panel();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Original)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Thresholded)).BeginInit();
            this.menu_Strip.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Edged)).BeginInit();
            this.panel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Processed)).BeginInit();
            this.panel4.SuspendLayout();
            this.SuspendLayout();
            // 
            // pic_Original
            // 
            this.pic_Original.Location = new System.Drawing.Point(3, 4);
            this.pic_Original.Margin = new System.Windows.Forms.Padding(4);
            this.pic_Original.Name = "pic_Original";
            this.pic_Original.Size = new System.Drawing.Size(441, 400);
            this.pic_Original.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pic_Original.TabIndex = 0;
            this.pic_Original.TabStop = false;
            // 
            // pic_Thresholded
            // 
            this.pic_Thresholded.Location = new System.Drawing.Point(4, 4);
            this.pic_Thresholded.Margin = new System.Windows.Forms.Padding(4);
            this.pic_Thresholded.Name = "pic_Thresholded";
            this.pic_Thresholded.Size = new System.Drawing.Size(441, 400);
            this.pic_Thresholded.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pic_Thresholded.TabIndex = 1;
            this.pic_Thresholded.TabStop = false;
            this.pic_Thresholded.Click += new System.EventHandler(this.pic_Processed_Click);
            // 
            // menu_Strip
            // 
            this.menu_Strip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_File,
            this.menu_Options});
            this.menu_Strip.Location = new System.Drawing.Point(0, 0);
            this.menu_Strip.Name = "menu_Strip";
            this.menu_Strip.Padding = new System.Windows.Forms.Padding(8, 2, 0, 2);
            this.menu_Strip.Size = new System.Drawing.Size(1216, 28);
            this.menu_Strip.TabIndex = 2;
            this.menu_Strip.Text = "menuStrip1";
            // 
            // menu_File
            // 
            this.menu_File.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_File_Open,
            this.saveImageToolStripMenuItem});
            this.menu_File.Name = "menu_File";
            this.menu_File.Size = new System.Drawing.Size(44, 24);
            this.menu_File.Text = "File";
            // 
            // menu_File_Open
            // 
            this.menu_File_Open.Name = "menu_File_Open";
            this.menu_File_Open.Size = new System.Drawing.Size(160, 24);
            this.menu_File_Open.Text = "Open Image";
            this.menu_File_Open.Click += new System.EventHandler(this.menu_File_Open_Click);
            // 
            // saveImageToolStripMenuItem
            // 
            this.saveImageToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.saveOriginalToolStripMenuItem,
            this.saveThresholdedToolStripMenuItem,
            this.saveEdgedToolStripMenuItem,
            this.saveProcessedToolStripMenuItem});
            this.saveImageToolStripMenuItem.Enabled = false;
            this.saveImageToolStripMenuItem.Name = "saveImageToolStripMenuItem";
            this.saveImageToolStripMenuItem.Size = new System.Drawing.Size(160, 24);
            this.saveImageToolStripMenuItem.Text = "Save Image";
            // 
            // saveOriginalToolStripMenuItem
            // 
            this.saveOriginalToolStripMenuItem.Name = "saveOriginalToolStripMenuItem";
            this.saveOriginalToolStripMenuItem.Size = new System.Drawing.Size(195, 24);
            this.saveOriginalToolStripMenuItem.Text = "Save Original";
            this.saveOriginalToolStripMenuItem.Click += new System.EventHandler(this.saveOriginalToolStripMenuItem_Click);
            // 
            // saveThresholdedToolStripMenuItem
            // 
            this.saveThresholdedToolStripMenuItem.Name = "saveThresholdedToolStripMenuItem";
            this.saveThresholdedToolStripMenuItem.Size = new System.Drawing.Size(195, 24);
            this.saveThresholdedToolStripMenuItem.Text = "Save Thresholded";
            this.saveThresholdedToolStripMenuItem.Click += new System.EventHandler(this.saveThresholdedToolStripMenuItem_Click);
            // 
            // saveEdgedToolStripMenuItem
            // 
            this.saveEdgedToolStripMenuItem.Name = "saveEdgedToolStripMenuItem";
            this.saveEdgedToolStripMenuItem.Size = new System.Drawing.Size(195, 24);
            this.saveEdgedToolStripMenuItem.Text = "Save Edged";
            this.saveEdgedToolStripMenuItem.Click += new System.EventHandler(this.saveEdgedToolStripMenuItem_Click);
            // 
            // saveProcessedToolStripMenuItem
            // 
            this.saveProcessedToolStripMenuItem.Name = "saveProcessedToolStripMenuItem";
            this.saveProcessedToolStripMenuItem.Size = new System.Drawing.Size(195, 24);
            this.saveProcessedToolStripMenuItem.Text = "Save Processed";
            this.saveProcessedToolStripMenuItem.Click += new System.EventHandler(this.saveProcessedToolStripMenuItem_Click);
            // 
            // menu_Options
            // 
            this.menu_Options.Name = "menu_Options";
            this.menu_Options.Size = new System.Drawing.Size(73, 24);
            this.menu_Options.Text = "Options";
            this.menu_Options.Click += new System.EventHandler(this.menu_Options_Click);
            // 
            // status_Progress
            // 
            this.status_Progress.Location = new System.Drawing.Point(1, 1073);
            this.status_Progress.Margin = new System.Windows.Forms.Padding(4);
            this.status_Progress.Name = "status_Progress";
            this.status_Progress.Size = new System.Drawing.Size(1215, 28);
            this.status_Progress.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.status_Progress.TabIndex = 3;
            // 
            // panel1
            // 
            this.panel1.AutoScroll = true;
            this.panel1.Controls.Add(this.pic_Original);
            this.panel1.Location = new System.Drawing.Point(0, 34);
            this.panel1.Margin = new System.Windows.Forms.Padding(4);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(604, 500);
            this.panel1.TabIndex = 4;
            // 
            // panel2
            // 
            this.panel2.AutoScroll = true;
            this.panel2.Controls.Add(this.pic_Thresholded);
            this.panel2.Location = new System.Drawing.Point(612, 34);
            this.panel2.Margin = new System.Windows.Forms.Padding(4);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(604, 500);
            this.panel2.TabIndex = 5;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(1022, 1052);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.label1.Size = new System.Drawing.Size(194, 17);
            this.label1.TabIndex = 6;
            this.label1.Text = "File > Open to load an image.";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // pic_Edged
            // 
            this.pic_Edged.Location = new System.Drawing.Point(4, 4);
            this.pic_Edged.Margin = new System.Windows.Forms.Padding(4);
            this.pic_Edged.Name = "pic_Edged";
            this.pic_Edged.Size = new System.Drawing.Size(441, 400);
            this.pic_Edged.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pic_Edged.TabIndex = 1;
            this.pic_Edged.TabStop = false;
            // 
            // panel3
            // 
            this.panel3.AutoScroll = true;
            this.panel3.Controls.Add(this.pic_Edged);
            this.panel3.Location = new System.Drawing.Point(0, 542);
            this.panel3.Margin = new System.Windows.Forms.Padding(4);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(604, 500);
            this.panel3.TabIndex = 6;
            // 
            // button1
            // 
            this.button1.Enabled = false;
            this.button1.Location = new System.Drawing.Point(3, 1048);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(122, 23);
            this.button1.TabIndex = 7;
            this.button1.Text = "Redo Operation";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // pic_Processed
            // 
            this.pic_Processed.Location = new System.Drawing.Point(4, 4);
            this.pic_Processed.Margin = new System.Windows.Forms.Padding(4);
            this.pic_Processed.Name = "pic_Processed";
            this.pic_Processed.Size = new System.Drawing.Size(441, 400);
            this.pic_Processed.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pic_Processed.TabIndex = 1;
            this.pic_Processed.TabStop = false;
            // 
            // panel4
            // 
            this.panel4.AutoScroll = true;
            this.panel4.Controls.Add(this.pic_Processed);
            this.panel4.Location = new System.Drawing.Point(612, 542);
            this.panel4.Margin = new System.Windows.Forms.Padding(4);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(604, 500);
            this.panel4.TabIndex = 7;
            // 
            // Viewer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1216, 1102);
            this.Controls.Add(this.panel4);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.status_Progress);
            this.Controls.Add(this.menu_Strip);
            this.MainMenuStrip = this.menu_Strip;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Viewer";
            this.Text = "Viewer";
            this.Load += new System.EventHandler(this.Viewer_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pic_Original)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Thresholded)).EndInit();
            this.menu_Strip.ResumeLayout(false);
            this.menu_Strip.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Edged)).EndInit();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Processed)).EndInit();
            this.panel4.ResumeLayout(false);
            this.panel4.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pic_Original;
        private System.Windows.Forms.PictureBox pic_Thresholded;
        private System.Windows.Forms.MenuStrip menu_Strip;
        private System.Windows.Forms.ToolStripMenuItem menu_File;
        private System.Windows.Forms.ToolStripMenuItem menu_File_Open;
        private System.Windows.Forms.ToolStripMenuItem menu_Options;
        private System.Windows.Forms.ProgressBar status_Progress;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PictureBox pic_Edged;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ToolStripMenuItem saveImageToolStripMenuItem;
        private System.Windows.Forms.PictureBox pic_Processed;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.ToolStripMenuItem saveOriginalToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveThresholdedToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveEdgedToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveProcessedToolStripMenuItem;
    }
}