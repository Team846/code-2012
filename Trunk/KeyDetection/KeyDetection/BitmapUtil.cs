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
    public class BitmapUtil
    {
        public struct Line
        {
            Point start;
            Point end;

            public Line(int x1, int y1, int x2, int y2)
                : this(new Point(x1, y2), new Point(x2, y2))
            { }

            public Line(Point p1, Point p2)
            {
                this.start = p1;
                this.end = p2;
            }

            public Point Start { get { return start; } }
            public Point End { get { return end; } }
        }

        /// <summary>
        /// Gets the distance between the center of the image and the key line.
        /// </summary>
        /// <param name="uBmp">The input image, with Sobel's edge detection.</param>
        /// <returns>The distance, in inches, between the center of the camera and the key line.</returns>
        public static float[] GetDistanceAndAngle(UnsafeBitmap uBmp, Line edge, float PPI_RATIO=0)
        {
            float[] toreturn = new float[2];

            toreturn[0] = Single.MinValue;
            toreturn[1] = Single.MinValue;

            if (edge.Start.X == -1 && edge.Start.Y == -1 && edge.End.X == -1 && edge.End.Y == -1)
                return toreturn; // INVALID_LINE


            if (PPI_RATIO == 0)
                PPI_RATIO = Globals.PPI_RATIO;

            // y = mx + b
            // b = y - mx

            float slope = (float)(edge.End.Y - edge.Start.Y) / (float)(edge.End.X - edge.Start.X);
            float intercept = (float)edge.End.Y - slope * (float)edge.End.X;
            float y = slope * (float)Globals.IMG_CENTER.X + intercept;
            float distance = y - (float)Globals.IMG_CENTER.Y;
            
            toreturn[0] = distance;
            toreturn[1] = GetAngle(slope);

            return toreturn;
        }

        public static float GetAngle(float rise, float run)
        {
            return GetAngle(rise / run);
        }

        public static float GetAngle(float slope)
        {
            return (float)Math.Atan(slope) * (180f / (float)Math.PI);
        }

        /// <summary>
        /// Creates a binary (black and white) bitmap.
        /// </summary>
        /// <param name="uBmp">The original bitmap.</param>
        /// <returns>A binary bitmap.</returns>
        /// 

        public static UnsafeBitmap Grayscale(UnsafeBitmap uBmp)
        {
            int rgb;
            PixelData pData;

            for (int y = 0; y < uBmp.Bitmap.Height; y++)
            {
                for (int x = 0; x < uBmp.Bitmap.Width; x++)
                {
                    pData = uBmp.GetPixel(x, y);

                    rgb = (pData.red + pData.green + pData.blue) / 3;

                    uBmp.SetPixel(x, y, new PixelData((byte)rgb, (byte)rgb, (byte)rgb));
                }
            }

            return uBmp;
        }

        public static UnsafeBitmap BlackWhite(UnsafeBitmap uBmp)
        {
            int rgb;
            PixelData pData;

            for (int y = 0; y < uBmp.Bitmap.Height; y++)
            {
                for (int x = 0; x < uBmp.Bitmap.Width; x++)
                {
                    pData = uBmp.GetPixel(x, y);

                    if (Color.FromArgb(pData.red, pData.blue, pData.green).GetSaturation() > Globals.satThreshold)
                    {
                        rgb = 255;
                    }
                    else
                    {
                        rgb = 0;
                    }

                    uBmp.SetPixel(x, y, new PixelData((byte)rgb, (byte)rgb, (byte)rgb));
                }
            }

            return uBmp;
        }

        /// <summary>
        /// Gets the row with the highest amount of black pixels
        /// </summary>
        /// <param name="uBmp">The unsafe bitmap to work with.</param>
        /// <param name="minAmount">(OPTIONAL) The minimum amount of required pixels for a row to pass.</param>
        /// <returns>The Y coordinate of the row with the highest amount of black pixels</returns>
        public static int GetNumberPixels(UnsafeBitmap uBmp, int minAmount = Int32.MinValue, int rowsGrouped = 3)
        {
            int row = -1;
            int maxNumPixels = 0;
            for (int y = 1; y < uBmp.Bitmap.Height - 1; y += rowsGrouped)
            {
                int numPixels = 0;
                for (int i = 0; i < rowsGrouped; i++)
                {
                    PixelData pData;
                    for (int x = 1; x < uBmp.Bitmap.Width - 1; x++)
                    {
                        if (y + i < uBmp.Bitmap.Height - 1)
                        {
                            pData = uBmp.GetPixel(x, y + i);
                            if (pData.red == 0 && pData.green == 0 && pData.blue == 0)
                            {
                                numPixels++;
                            }
                        }
                    }
                }
                if (numPixels > maxNumPixels)
                {
                    row = y;
                    maxNumPixels = numPixels;
                }
            }

            if (maxNumPixels >= minAmount)
                return row;

            return -1;
        }
    }
}
