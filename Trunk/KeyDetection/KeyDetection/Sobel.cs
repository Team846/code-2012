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
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;

namespace KeyDetection
{
    public class Sobel
    {
        public static UnsafeBitmap DoSobel(UnsafeBitmap uBmp)
        {
            int[,] gx = new int[,] { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };   //  The matrix Gx
            int[,] gy = new int[,] { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };  //  The matrix Gy

            UnsafeBitmap eBmp = new UnsafeBitmap(new Bitmap(uBmp.Bitmap.Width, uBmp.Bitmap.Height));

            eBmp.LockBitmap();

            for (int i = 1; i < uBmp.Bitmap.Height - 1; i++)   // loop for the image pixels height
            {
                for (int j = 1; j < uBmp.Bitmap.Width - 1; j++) // loop for image pixels width    
                {
                    float new_x = 0, new_y = 0;
                    float c;
                    for (int hw = -1; hw < 2; hw++)  //loop for cov matrix
                    {
                        for (int wi = -1; wi < 2; wi++)
                        {
                            c = (uBmp.GetPixel(j + wi, i + hw).blue + uBmp.GetPixel(j + wi, i + hw).red + uBmp.GetPixel(j + wi, i + hw).green) / 3;
                            new_x += gx[hw + 1, wi + 1] * c;
                            new_y += gy[hw + 1, wi + 1] * c;
                        }
                    }
                    if (new_x * new_x + new_y * new_y > 512 * 512)
                        eBmp.SetPixel(j, i, new PixelData(0, 0, 0));
                    else
                        eBmp.SetPixel(j, i, new PixelData(255, 255, 255));
                }
            }

            eBmp.UnlockBitmap();

            return eBmp;
        }
    }
}
