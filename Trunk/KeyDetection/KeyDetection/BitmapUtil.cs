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
    public class BitmapUtil
    {
        /// <summary>
        /// Gets the distance between the center of the image and the key line.
        /// </summary>
        /// <param name="uBmp">The input image, with Sobel's edge detection.</param>
        /// <returns>The distance, in inches, between the center of the camera and the key line.</returns>
        public static float GetDistance(UnsafeBitmap uBmp, float PPI_RATIO=Globals.PPI_RATIO)
        {
            int rowY = GetNumberPixels(uBmp);

            if (rowY <= 0)
                return Int32.MinValue; // unable to locate

            /* TO-DO: Account for angles */
            return (float)(Globals.IMG_CENTER.Y - rowY) / PPI_RATIO; // TO-DO: Find center coordinates to make IMG_CENTER constant so we can use an optional parameter
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

        public static UnsafeBitmap BlackWhite(UnsafeBitmap uBmp, double satThreshold=Globals.satThreshold)
        {
            int rgb;
            PixelData pData;

            for (int y = 0; y < uBmp.Bitmap.Height; y++)
            {
                for (int x = 0; x < uBmp.Bitmap.Width; x++)
                {
                    pData = uBmp.GetPixel(x, y);

                    if (Color.FromArgb(pData.red, pData.blue, pData.green).GetSaturation() > satThreshold)
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
