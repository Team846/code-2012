using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace KeyDetection
{
    public partial class Viewer : Form
    {
        public Viewer()
        {
            InitializeComponent();
        }

        private void menu_File_Open_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "Select a file...";

            ofd.Multiselect = false;

            ofd.Filter = "Image Files (*.png, *.jpg, *.jpeg, *.bmp)|*.png;*.jpg;*.jpeg;*.bmp";

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                Thread thread = new Thread(processImage);
                thread.Start(ofd.FileName);
            }
        }

        private void processImage(object bLocation)
        {
            Console.WriteLine("[processImage()] Thread started");

            try
            {
                System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();

                sw.Start();

                string bitmapLocation = (string)bLocation;
                Bitmap frameImage = new Bitmap(bitmapLocation);

                panel1.BeginInvoke(
                    new Action(() =>
                        pic_Original.Image = (Image)frameImage.Clone()));

                label1.BeginInvoke(
                    new Action(() =>
                        label1.Text = "Processing...")

                    );

                label1.BeginInvoke(
                    new Action(() =>
                        label1.Left = this.Width - label1.Width - 16)
                    );

                Globals.IMG_CENTER = new Point((frameImage.Width - 1) / 2, (frameImage.Height - 1) / 2);

                UnsafeBitmap unsafeFrame = new UnsafeBitmap(frameImage);

                unsafeFrame.LockBitmap();
                unsafeFrame = BitmapUtil.BlackWhite(unsafeFrame);
                status_Progress.BeginInvoke(
                    new Action(() =>
                        status_Progress.Value = 25));
                //unsafeFrame = Grayscale(unsafeFrame);
                UnsafeBitmap sobel = Sobel.DoSobel(unsafeFrame);
                status_Progress.BeginInvoke(
                    new Action(() =>
                        status_Progress.Value = 50));
                unsafeFrame.UnlockBitmap();
                unsafeFrame.Dispose();
                sobel.LockBitmap();
                BitmapUtil.Line edge = HoughTransform.DoTransform(sobel);
                status_Progress.BeginInvoke(
                    new Action(() =>
                        status_Progress.Value = 75));
                float[] distanceangle = BitmapUtil.GetDistanceAndAngle(sobel, edge);
                status_Progress.BeginInvoke(
                    new Action(() =>
                        status_Progress.Value = 100));
                sobel.UnlockBitmap();

                Bitmap processed = new Bitmap(sobel.Bitmap);

                sobel.Dispose();

                Graphics g = Graphics.FromImage(processed);
                Pen red = new Pen(Color.Red, 3);
                Pen blue = new Pen(Color.Blue, 5);
                Pen green = new Pen(Color.Green, 5);
                Pen gold = new Pen(Color.Gold, 3);
                Pen black = new Pen(Color.Black, 1);
                
                SolidBrush b_green = new SolidBrush(Color.Green);

                g.DrawLine(black, 0, Globals.IMG_CENTER.Y, processed.Width - 1, Globals.IMG_CENTER.Y);

                g.DrawLine(red, edge.Start, edge.End);

                g.DrawLine(green, Globals.IMG_CENTER.X - 10, Globals.IMG_CENTER.Y, Globals.IMG_CENTER.X + 10, Globals.IMG_CENTER.Y);
                g.DrawLine(blue, Globals.IMG_CENTER.X - 10, distanceangle[0] + Globals.IMG_CENTER.Y, Globals.IMG_CENTER.X + 10, distanceangle[0] + Globals.IMG_CENTER.Y);
                g.DrawLine(gold, Globals.IMG_CENTER.X, distanceangle[0] + Globals.IMG_CENTER.Y, Globals.IMG_CENTER.X, Globals.IMG_CENTER.Y);

                string dist_str = Globals.PPI_RATIO <= 0 ? distanceangle[0] + "px (no PPI), " + distanceangle[1].ToString() + "" : distanceangle[0] / Globals.PPI_RATIO + "inches, " + distanceangle[1].ToString() + "";

                Font arial = new Font("Arial", 12);
                g.DrawString(dist_str, new Font("Arial", 12), b_green, Globals.IMG_CENTER.X + 10, Globals.IMG_CENTER.Y + (distanceangle[0] / 2));

                Console.WriteLine(dist_str);

                panel2.BeginInvoke(
                    new Action(() =>
                        pic_Processed.Image = (Image)processed.Clone()));

                status_Progress.BeginInvoke(
                    new Action(() =>
                        status_Progress.Value = 0));

                sw.Stop();

                label1.BeginInvoke(
                    new Action(() =>
                        label1.Text = "Last Operation completed in " + sw.ElapsedMilliseconds + "ms.")

                    );

                label1.BeginInvoke(
                    new Action(() =>
                        label1.Left = this.Width - label1.Width - 16)
                    );
            }
            catch
            {
                Console.WriteLine("[processImage()] Exception encountered...aborting...");
                Thread.CurrentThread.Abort();
            }

            Console.WriteLine("[processImage()] Finished.  Aborting...");
            Thread.CurrentThread.Abort();
        }

        private void pic_Processed_Click(object sender, EventArgs e)
        {

        }

        private void Viewer_Load(object sender, EventArgs e)
        {

        }

        private void menu_Options_Click(object sender, EventArgs e)
        {
            new Options().Show();
        }
    }
}
