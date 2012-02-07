/*
 * Authors: Tony Peng & Raphael Chang
 * As of February 6th, 8:11 AM, this code properly detects the edge of the key
 *
 * TO-DO:
 * - Get distance from edge
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
            sobel = HoughTransform.DoTransform(sobel);
            sobel2 = HoughTransform.DoTransform(sobel2);
            sobel.LockBitmap();
            sobel2.LockBitmap();
            float distance = BitmapUtil.GetDistance(sobel);
            sobel.UnlockBitmap();
            sobel2.UnlockBitmap();


            Console.WriteLine(distance + " inches");

            sw.Stop();

            sobel.Bitmap.Save("frameout.jpg", ImageFormat.Jpeg);
            sobel2.Bitmap.Save("frameout2.jpg", ImageFormat.Jpeg);

            sobel.Dispose();
            sobel2.Dispose();

            Console.WriteLine("Execution finished in {0}ms.", sw.ElapsedMilliseconds);

            Console.ReadKey();
        }
    }
}
