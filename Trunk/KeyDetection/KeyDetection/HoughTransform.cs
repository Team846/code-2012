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
    public class HoughTransform
    {
        static int gPD_R(UnsafeBitmap uBmp, int i, int j)
        {
            return uBmp.GetPixel(i, j).red / 255;
        }

        public static UnsafeBitmap DoTransform(UnsafeBitmap uBmp)
        {
            int rMax = (int)(Math.Sqrt((uBmp.Bitmap.Width * uBmp.Bitmap.Width) + (uBmp.Bitmap.Height * uBmp.Bitmap.Height)));
            int[,] bmpArray = new int[360, rMax];

            Bitmap bBmp = new Bitmap(360, rMax);
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
                            //int rho = (int)((((double)j - ((double)uBmp.Bitmap.Width / 2.0)) * Math.Cos(theta)) + (((double)i - ((double)uBmp.Bitmap.Height / 2.0)) * Math.Sin(theta)));
                            //rho = -(rho - rMax);
                            int rho = (int)((((double)j) * Math.Cos(theta)) + (((double)i) * Math.Sin(theta)));
                            if (rho >= 0 && rho <= rMax)
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
            int max = 0;
            int xPos, yPos;

            for (int m = 0; m < rMax; m++)
            {
                for (int n = 0; n < 360; n++)
                {
                    if (bmpArray[n, m] > max)
                    {
                        max = bmpArray[n, m];
                        x = n;
                        y = m;
                    }
                }
            }
            double rho2;
            double theta2;
            uBmp.UnlockBitmap();
            UnsafeBitmap oBmp = new UnsafeBitmap(uBmp.Bitmap);
            uBmp.LockBitmap();
            Graphics g = Graphics.FromImage(oBmp.Bitmap);
            Pen redPen = new Pen(Color.Red, 3);
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
                rho2 = y;
                theta2 = x * Math.PI / 180;
                Console.WriteLine(rho2);
                Console.WriteLine(theta2);
                double a = Math.Cos(theta2), b = Math.Sin(theta2);
                double x0 = a * rho2, y0 = b * rho2;
                Point pt1 = new Point((int)Math.Round(x0 + uBmp.Bitmap.Width * (-b)),
                          (int)Math.Round(y0 + uBmp.Bitmap.Height * (a)));
                Point pt2 = new Point((int)Math.Round(x0 - uBmp.Bitmap.Width * (-b)),
                          (int)Math.Round(y0 - uBmp.Bitmap.Height * (a)));
                g.DrawLine(redPen, pt1, pt2);
                Console.WriteLine(pt1);
                Console.WriteLine(pt2);
                for (int z = 0; z < 256; z++)
                {
                    /*
                    int yPixel = (int)((rho2 / Math.Sin(theta2)) - ((z - 128.0) * (Math.Cos(theta2) / Math.Sin(theta2))) + 128.0);
                    int xPixel = (int)((rho2 / Math.Cos(theta2)) - ((z - 128.0) * Math.Tan(theta2)) + 128.0);

                    if (z > 0 && z < 256 && yPixel > 0 && yPixel < 256)
                    {
                        g.DrawLine(redPen, new Point(z + 20, yPixel + 20), new Point(z + 20, yPixel + 20));
                    }

                    if (xPixel > 0 && xPixel < 256)
                    {
                        g.DrawLine(redPen, new Point(xPixel + 20, z + 20), new Point(xPixel + 20, z + 20));
                    }*/
                }
                g.Dispose();
            }
            ubBmp.UnlockBitmap();
            return oBmp;
        }
    }
}
