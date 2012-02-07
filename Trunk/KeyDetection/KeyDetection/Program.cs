/*
 * Authors: Tony Peng & Raphael Chang
 * As of February 6th, 8:11 AM, this code properly detects the edge of the key
 *
 * TO-DO:
 * - Improve Hough Transform
 * 
 * CURRENT STATUS:
 * - Sobel is working!
 * - Hough Transform is working, but accuracy could be improved a bit. :|
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Text;

namespace KeyDetection
{
    class Program
    {
        static void Main(string[] args)
        {
            Stopwatch sw = new Stopwatch();

            sw.Start();

            Bitmap frameImage = new Bitmap("framein.jpg");
            Bitmap frameImage2 = new Bitmap("framein2.jpg");

            Globals.IMG_CENTER = new Point((frameImage.Width - 1) / 2, (frameImage.Height - 1) / 2);

            UnsafeBitmap unsafeFrame = new UnsafeBitmap(frameImage);
            UnsafeBitmap unsafeFrame2 = new UnsafeBitmap(frameImage2);

            unsafeFrame.LockBitmap();
            unsafeFrame = BitmapUtil.BlackWhite(unsafeFrame);
            unsafeFrame2.LockBitmap();
            unsafeFrame2 = BitmapUtil.BlackWhite(unsafeFrame2);
            //unsafeFrame = Grayscale(unsafeFrame);
            UnsafeBitmap sobel = Sobel.DoSobel(unsafeFrame);
            UnsafeBitmap sobel2 = Sobel.DoSobel(unsafeFrame2);
            unsafeFrame.UnlockBitmap();
            unsafeFrame2.UnlockBitmap();
            unsafeFrame.Dispose();
            unsafeFrame2.Dispose();
            sobel.LockBitmap();
            sobel2.LockBitmap();
            BitmapUtil.Line edge = HoughTransform.DoTransform(sobel);
            BitmapUtil.Line edge2 = HoughTransform.DoTransform(sobel2);
            float distance = BitmapUtil.GetDistance(sobel, edge);
            float distance2 = BitmapUtil.GetDistance(sobel2, edge2);
            sobel.UnlockBitmap();
            sobel2.UnlockBitmap();


            Console.WriteLine(distance + " inches");
            Console.WriteLine(distance2 + "inches");

            sw.Stop();

            Graphics g = Graphics.FromImage(sobel.Bitmap);
            Pen red = new Pen(Color.Red, 3);

            g.DrawLine(red, edge.Start, edge.End);

            g = Graphics.FromImage(sobel2.Bitmap);
            g.DrawLine(red, edge2.Start, edge2.End);

            sobel.Bitmap.Save("frameout.jpg", ImageFormat.Jpeg);
            sobel2.Bitmap.Save("frameout2.jpg", ImageFormat.Jpeg);

            sobel.Dispose();
            sobel2.Dispose();

            Console.WriteLine("Execution finished in {0}ms.", sw.ElapsedMilliseconds);

            Console.ReadKey();
        }
    }
}
