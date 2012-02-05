/* HORRIBLE FORMATTING/FILE STRUCTURE IS DUE TO RESTRICTIONS IN PLACE BY COLLABEDIT -- will separate/properly format at final release */

/*
 * Authors: Tony Peng & Raphael Chang
 * As of February 5th, 2:45 AM, this code properly detects the edge of the key
 *
 * TO-DO:
 * - Get distance from edge
 * 
 * CURRENT STATUS:
 * - Sobel is working!
 * - Brian is not happy with binary thresholding. :(
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
        /*static PixelData redHigh = new PixelData(69, 30, 48); //if we have a red key
        static PixelData redLow = new PixelData(114, 70, 103);
        static PixelData blueHigh; //If we have a blue key
        static PixelData blueLow;*/

        const float PPI_RATIO = 35; // assuming the image is the entire static test image, bitmap.height / 12 inches
        static Point IMG_CENTER;

        static void Main(string[] args)
        {
            Stopwatch sw = new Stopwatch();

            sw.Start();

            Bitmap frameImage = new Bitmap("framein.jpg");

            IMG_CENTER = new Point((frameImage.Width-1) / 2, (frameImage.Height-1) / 2);

            UnsafeBitmap unsafeFrame = new UnsafeBitmap(frameImage);

            unsafeFrame.LockBitmap();
            unsafeFrame = BlackWhite(unsafeFrame);
            UnsafeBitmap sobel = Sobel.DoSobel(unsafeFrame);
            unsafeFrame.UnlockBitmap();
            unsafeFrame.Dispose();

            sobel.LockBitmap();
            float distance = GetDistance(sobel);
            sobel.UnlockBitmap();

            Console.WriteLine(distance + " inches");

            sw.Stop();

            sobel.Bitmap.Save("frameout.jpg", ImageFormat.Jpeg);

            sobel.Dispose();

            Console.WriteLine("Execution finished in {0}ms.", sw.ElapsedMilliseconds);

            Console.ReadKey();
        }

        /// <summary>
        /// Gets the distance between the center of the image and the key line.
        /// </summary>
        /// <param name="uBmp">The input image, with Sobel's edge detection.</param>
        /// <returns>The distance, in inches, between the center of the camera and the key line.</returns>
        public static float GetDistance(UnsafeBitmap uBmp)
        {
            int rowY = GetNumberPixels(uBmp);

            if (rowY <= 0)
                return Int32.MinValue; // unable to locate

            /* TO-DO: Account for angles */
            return (float)(IMG_CENTER.Y - rowY) / PPI_RATIO;
        }

        /// <summary>
        /// Creates a binary (black and white) bitmap.
        /// </summary>
        /// <param name="uBmp">The original bitmap.</param>
        /// <returns>A binary bitmap.</returns>
        public static UnsafeBitmap BlackWhite(UnsafeBitmap uBmp)
        {
            int rgb;
            PixelData pData;

            for (int y = 0; y < uBmp.Bitmap.Height; y++)
            {
                for (int x = 0; x < uBmp.Bitmap.Width; x++)
                {
                    pData = uBmp.GetPixel(x, y);

                    if (Color.FromArgb(pData.red, pData.blue, pData.green).GetSaturation() > 0.175)
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
        public static int GetNumberPixels(UnsafeBitmap uBmp, int minAmount=Int32.MinValue)
        {
            int row = -1;
            int maxNumPixels = 0;
            for (int y = 1; y < uBmp.Bitmap.Height - 1; y++)
            {
                int numPixels = 0;
                PixelData pData;
                for (int x = 1; x < uBmp.Bitmap.Width - 1; x++)
                {
                    pData = uBmp.GetPixel(x, y);
                    if (pData.red == 0 && pData.green == 0 && pData.blue == 0)
                    {
                        numPixels++;
                    }
                }

                if (numPixels > maxNumPixels)
                {
                    row = y;
                    maxNumPixels = numPixels;
                }
            }

            if(maxNumPixels >= minAmount)
                return row;

            return -1;
        }
    }

    /// <summary>
    /// Struct that holds the individual red, blue, and green channels for a pixel.
    /// </summary>
    public struct PixelData
    {
        public PixelData(byte red, byte blue, byte green)
        {
            this.red = red;
            this.blue = blue;
            this.green = green;
        }

        public byte blue;
        public byte green;
        public byte red;

        public int Color
        {
            get
            {
                int rgb = red;

                rgb = (rgb << 8) + green;
                rgb = (rgb << 8) + blue;

                return rgb;
            }
        }
    }

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
                    if (new_x * new_x + new_y * new_y > 32 * 32)
                        eBmp.SetPixel(j, i, new PixelData(0, 0, 0));
                    else
                        eBmp.SetPixel(j, i, new PixelData(255, 255, 255));
                }
            }

            eBmp.UnlockBitmap();

            return eBmp;
        }
    }

    /*
    * Source:
    * http://www.dreamincode.net/forums/topic/14788-c%23-fast-acces-to-bitmap-pixels/ 
    */

    public unsafe class UnsafeBitmap
    {
        Bitmap bitmap;

        int width;
        BitmapData bitmapData = null;
        Byte* pBase = null;

        public UnsafeBitmap(Bitmap bitmap)
        {
            this.bitmap = new Bitmap(bitmap);
        }

        public UnsafeBitmap(int width, int height)
        {
            this.bitmap = new Bitmap(width, height, PixelFormat.Format24bppRgb);
        }

        public void Dispose()
        {
            bitmap.Dispose();
        }

        public Bitmap Bitmap
        {
            get
            {
                return bitmap;
            }
        }

        private Point PixelSize
        {
            get
            {
                GraphicsUnit unit = GraphicsUnit.Pixel;
                RectangleF bounds = bitmap.GetBounds(ref unit);

                return new Point((int)bounds.Width, (int)bounds.Height);
            }
        }

        public void LockBitmap()
        {
            GraphicsUnit unit = GraphicsUnit.Pixel;
            RectangleF boundsF = bitmap.GetBounds(ref unit);
            Rectangle bounds = new Rectangle((int)boundsF.X,
                (int)boundsF.Y,
                (int)boundsF.Width,
                (int)boundsF.Height);

            width = (int)boundsF.Width * sizeof(PixelData);

            if (width % 4 != 0)
                width = 4 * (width / 4 + 1);

            bitmapData = bitmap.LockBits(bounds, ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

            pBase = (Byte*)bitmapData.Scan0.ToPointer();
        }

        public PixelData* PixelAt(int x, int y)
        {
            return (PixelData*)(pBase + y * width + x * sizeof(PixelData));
        }

        public PixelData GetPixel(int x, int y)
        {
            PixelData returnValue = *PixelAt(x, y);

            return returnValue;
        }

        public void SetPixel(int x, int y, PixelData color)
        {
            PixelData* pixel = PixelAt(x, y);
            *pixel = color;
        }

        public void UnlockBitmap()
        {
            bitmap.UnlockBits(bitmapData);
            bitmapData = null;
            pBase = null;
        }
    }
}
