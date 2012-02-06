/* HORRIBLE FORMATTING/FILE STRUCTURE IS DUE TO RESTRICTIONS IN PLACE BY COLLABEDIT -- will separate/properly format at final release */

/*
 * Authors: Tony Peng & Raphael Chang
 * As of February 5th, 2:45 AM, this code properly detects the edge of the key
 *
 * TO-DO:
 * - Get distance from edge
 * - Finish Hough Transform
 * 
 * CURRENT STATUS:
 * - Sobel is working!
 * - Hough Transform is NOT working! :(
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Text;

namespace KeyDetection
{
    public class HoughTransform
    {
        static int[,] maxPosArray = new int[10, 2];
        static int gPD_R(UnsafeBitmap uBmp, int i, int j)
        {
            return uBmp.GetPixel(i, j).red / 255;
        }

        public static UnsafeBitmap DoTransform(UnsafeBitmap uBmp)
        {
            int[,] bmpArray = new int[uBmp.Bitmap.Width, uBmp.Bitmap.Height];

            int rMax = (int)(Math.Sqrt((uBmp.Bitmap.Width / 2 * uBmp.Bitmap.Width / 2) + (uBmp.Bitmap.Height / 2 * uBmp.Bitmap.Height / 2)));
            Bitmap bBmp = new Bitmap(uBmp.Bitmap.Width, uBmp.Bitmap.Height);
            UnsafeBitmap ubBmp = new UnsafeBitmap(bBmp);

            ubBmp.LockBitmap();

            for (int i = 0; i < uBmp.Bitmap.Height; i++)
            {
                for (int j = 0; j < uBmp.Bitmap.Width; j++)
                {
                    if (gPD_R(uBmp, j, i) == 0)
                    {
                        for (int t = 0; t < 360; t++)
                        {
                            double theta = t * Math.PI / 180;
                            int rho = (int)((((double)j - ((double)uBmp.Bitmap.Width / 2.0)) * Math.Cos(theta)) + (((double)i - ((double)uBmp.Bitmap.Height / 2.0)) * Math.Sin(theta)));
                            rho = -(rho - rMax);
                            if (!(rho < 0) && !(rho > rMax))
                            {
                                ubBmp.SetPixel(t, rho, new PixelData(255, 255, 255));
                                bmpArray[t, rho] += 1;
                            }
                        }
                    }
                }
            }
            int x = 0;
            int y = 0;
            int max = bmpArray[0, 0];
            int xPos, yPos;

            for (int m = 1; m < uBmp.Bitmap.Height; m++)
            {
                for (int n = 1; n < uBmp.Bitmap.Width; n++)
                {
                    if (bmpArray[n, m] > max)
                    {
                        max = bmpArray[n, m];
                        x = n;
                        y = m;
                    }
                }
            }
            int rho2;
            double theta2;
            if (ubBmp.GetPixel(x, y).red != 0)
            {
                for (int k = x - 10; k <= x + 10; k++)
                {
                    for (int l = y - 10; l <= y + 10; l++)
                    {
                        xPos = k; yPos = l;
                        if (k < 0) xPos = xPos + uBmp.Bitmap.Width;
                        else if (k >= uBmp.Bitmap.Width) xPos = xPos % uBmp.Bitmap.Width;
                        if (l < 0) yPos = yPos + uBmp.Bitmap.Height;
                        else if (l >= uBmp.Bitmap.Height) yPos = yPos % uBmp.Bitmap.Height;
                        ubBmp.SetPixel(xPos, yPos, new PixelData(0, 0, 0));

                    }
                }
                rho2 = (y - rMax) * -1;
                theta2 = x * Math.PI / 180;
                Console.WriteLine(rho2);
                Console.WriteLine(theta2);
                Graphics g = Graphics.FromImage(uBmp.Bitmap);
                Pen redPen = new Pen(Color.Red);
                for (int z = 0; z < 256; z++)
                {

                    int yPixel = (int)(((double)rho2 / Math.Sin(theta2)) - (((double)z - 128.0) * (Math.Cos(theta2) / Math.Sin(theta2))) + 128.0);

                    // To get a Clear line....we draw in x- as well as in y direction
                    int xPixel = (int)(((double)rho2 / Math.Cos(theta2)) - (((double)z - 128.0) * Math.Tan(theta2)) + 128.0);

                    if (z > 0 && z < 256 && yPixel > 0 && yPixel < 256)
                    {
                        g.DrawLine(redPen, new Point(z + 20, yPixel + 20), new Point(z + 20, yPixel + 20));
                    }

                    if (xPixel > 0 && xPixel < 256)
                    {
                        g.DrawLine(redPen, new Point(xPixel + 20, z + 20), new Point(xPixel + 20, z + 20));
                    }
                }
            }
            uBmp.Bitmap.Save("image.jpg");

            ubBmp.UnlockBitmap();
            return uBmp;
        }
    }
}
