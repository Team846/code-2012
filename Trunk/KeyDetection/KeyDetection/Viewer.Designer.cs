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
            this.pic_Processed = new System.Windows.Forms.PictureBox();
            this.menu_Strip = new System.Windows.Forms.MenuStrip();
            this.menu_File = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_File_Open = new System.Windows.Forms.ToolStripMenuItem();
            this.menu_Options = new System.Windows.Forms.ToolStripMenuItem();
            this.status_Progress = new System.Windows.Forms.ProgressBar();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label1 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Original)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Processed)).BeginInit();
            this.menu_Strip.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // pic_Original
            // 
            this.pic_Original.Location = new System.Drawing.Point(12, 3);
            this.pic_Original.Name = "pic_Original";
            this.pic_Original.Size = new System.Drawing.Size(460, 400);
            this.pic_Original.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pic_Original.TabIndex = 0;
            this.pic_Original.TabStop = false;
            // 
            // pic_Processed
            // 
            this.pic_Processed.Location = new System.Drawing.Point(3, 3);
            this.pic_Processed.Name = "pic_Processed";
            this.pic_Processed.Size = new System.Drawing.Size(441, 400);
            this.pic_Processed.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pic_Processed.TabIndex = 1;
            this.pic_Processed.TabStop = false;
            this.pic_Processed.Click += new System.EventHandler(this.pic_Processed_Click);
            // 
            // menu_Strip
            // 
            this.menu_Strip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_File,
            this.menu_Options});
            this.menu_Strip.Location = new System.Drawing.Point(0, 0);
            this.menu_Strip.Name = "menu_Strip";
            this.menu_Strip.Size = new System.Drawing.Size(931, 24);
            this.menu_Strip.TabIndex = 2;
            this.menu_Strip.Text = "menuStrip1";
            // 
            // menu_File
            // 
            this.menu_File.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menu_File_Open});
            this.menu_File.Name = "menu_File";
            this.menu_File.Size = new System.Drawing.Size(37, 20);
            this.menu_File.Text = "File";
            // 
            // menu_File_Open
            // 
            this.menu_File_Open.Name = "menu_File_Open";
            this.menu_File_Open.Size = new System.Drawing.Size(139, 22);
            this.menu_File_Open.Text = "Open Image";
            this.menu_File_Open.Click += new System.EventHandler(this.menu_File_Open_Click);
            // 
            // menu_Options
            // 
            this.menu_Options.Name = "menu_Options";
            this.menu_Options.Size = new System.Drawing.Size(61, 20);
            this.menu_Options.Text = "Options";
            this.menu_Options.Click += new System.EventHandler(this.menu_Options_Click);
            // 
            // status_Progress
            // 
            this.status_Progress.Location = new System.Drawing.Point(0, 471);
            this.status_Progress.Name = "status_Progress";
            this.status_Progress.Size = new System.Drawing.Size(931, 23);
            this.status_Progress.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.status_Progress.TabIndex = 3;
            // 
            // panel1
            // 
            this.panel1.AutoScroll = true;
            this.panel1.Controls.Add(this.pic_Original);
            this.panel1.Location = new System.Drawing.Point(0, 28);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(475, 420);
            this.panel1.TabIndex = 4;
            // 
            // panel2
            // 
            this.panel2.AutoScroll = true;
            this.panel2.Controls.Add(this.pic_Processed);
            this.panel2.Location = new System.Drawing.Point(478, 28);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(453, 420);
            this.panel2.TabIndex = 5;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(777, 451);
            this.label1.Name = "label1";
            this.label1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.label1.Size = new System.Drawing.Size(145, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "File > Open to load an image.";
            // 
            // Viewer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(931, 494);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.status_Progress);
            this.Controls.Add(this.menu_Strip);
            this.MainMenuStrip = this.menu_Strip;
            this.Name = "Viewer";
            this.Text = "Viewer";
            this.Load += new System.EventHandler(this.Viewer_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pic_Original)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pic_Processed)).EndInit();
            this.menu_Strip.ResumeLayout(false);
            this.menu_Strip.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pic_Original;
        private System.Windows.Forms.PictureBox pic_Processed;
        private System.Windows.Forms.MenuStrip menu_Strip;
        private System.Windows.Forms.ToolStripMenuItem menu_File;
        private System.Windows.Forms.ToolStripMenuItem menu_File_Open;
        private System.Windows.Forms.ToolStripMenuItem menu_Options;
        private System.Windows.Forms.ProgressBar status_Progress;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label1;
    }
}