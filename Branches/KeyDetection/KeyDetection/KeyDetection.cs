/* HORRIBLE FORMATTING/FILE STRUCTURE IS DUE TO RESTRICTIONS IN PLACE BY COLLABEDIT -- will separate/properly format at final release */

/*
 * Authors: Tony Peng & Raphael Chang
 * As of February 5th, 2:45 AM, this code properly detects the edge of the key
 *
 * TO-DO:
 * - Get distance from edge
 * 
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

        static void Main(string[] args)
        {
            Stopwatch sw = new Stopwatch();

            sw.Start();

            Bitmap frameImage = new Bitmap("framein.jpg");

            UnsafeBitmap unsafeFrame = new UnsafeBitmap(frameImage);
            unsafeFrame.LockBitmap();

            unsafeFrame = BlackWhite(unsafeFrame);
            //int rowY = GetNumberPixels(unsafeFrame);
            Bitmap sobel = Sobel.DoSobel(unsafeFrame);

            unsafeFrame.UnlockBitmap();

            //Graphics gBmp = Graphics.FromImage(unsafeFrame.Bitmap);
            //Brush redBrush = new SolidBrush(Color.Red);

            //gBmp.FillRectangle(redBrush, 0, rowY, unsafeFrame.Bitmap.Width, 2);

            sw.Stop();

            sobel.Save("frameout.jpg", ImageFormat.Jpeg);

            // program end
            sobel.Dispose();

            Console.WriteLine("Execution finished in {0}ms.", sw.ElapsedMilliseconds);

            Console.ReadKey();
        }

        /* http://stackoverflow.com/questions/4669317/how-to-convert-a-bitmap-image-to-black-and-white-in-c */
        public static UnsafeBitmap BlackWhite(UnsafeBitmap uBmp)
        {
            int rgb;
            PixelData pData;

            for (int y = 0; y < uBmp.Bitmap.Height; y++)
            {
                for (int x = 0; x < uBmp.Bitmap.Width; x++)
                {
                    pData = uBmp.GetPixel(x, y);

                    //rgb = (pData.red >= redLow.red && pData.red <= redHigh.red) && (pData.green >= redLow.green && pData.green <= redHigh.green) && (pData.blue >= redLow.blue && pData.blue <= redHigh.blue) ? 255 : 0;
                    //rgb = (pData.Color >= redLow.Color && pData.Color <= redHigh.Color) ? 255 : 0;
                    //rgb = ((int)pData.red + (int)pData.green + (int)pData.blue) / 3;
                    if (Color.FromArgb((int)(pData.Color & 0x000000FF),
                    (int)(pData.Color & 0x0000FF00) >> 8,
                    (int)(pData.Color & 0x00FF0000) >> 16).GetSaturation() > 0.175)
                    {
                        rgb = 255;
                    }
                    else
                    {
                        rgb = 0;
                    }

                    uBmp.SetPixel(x, y, new PixelData((byte)rgb, (byte)rgb, (byte)rgb));
                    //uBmp.SetPixel(x, y, new PixelData((byte)rgb, (byte)rgb, (byte)rgb));
                }
            }

            return uBmp;
        }
        public static int GetNumberPixels(UnsafeBitmap uBmp)
        {
            int row = -1;
            int maxNumPixels = 0;
            for (int y = 0; y < uBmp.Bitmap.Height; y++)
            {
                int numPixels = 0;
                PixelData pData;
                for (int x = 0; x < uBmp.Bitmap.Width; x++)
                {
                    pData = uBmp.GetPixel(x, y);
                    if (pData.red == 255 && pData.green == 255 && pData.blue == 255)
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

            return row;
        }
    }

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
        public static Bitmap DoSobel(UnsafeBitmap uBmp)
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

            return eBmp.Bitmap;
        }
        /*
            public static Bitmap DoSobel(UnsafeBitmap uBmp)
            {
                UnsafeBitmap eBmp = new UnsafeBitmap(new Bitmap(uBmp.Bitmap.Width, uBmp.Bitmap.Height));

                eBmp.LockBitmap();

                for (int i = 1; i < eBmp.Bitmap.Width - 1; i++)
                {
                    for (int j = 1; j < eBmp.Bitmap.Height - 1; j++)
                    {
                        int mag = 0;

                        mag += Math.Abs(0
                            - uBmp.GetPixel(i - 1, j - 1).red
                            - uBmp.GetPixel(i - 1, j).red * 2
                            - uBmp.GetPixel(i - 1, j + 1).red
                            + uBmp.GetPixel(i + 1, j - 1).red
                            + uBmp.GetPixel(i + 1, j).red * 2
                            + uBmp.GetPixel(i + 1, j).red);
                        mag += Math.Abs(0
                            - uBmp.GetPixel(i - 1, j - 1).red
                            - uBmp.GetPixel(i, j - 1).red * 2
                            - uBmp.GetPixel(i + 1, j - 1).red
                            + uBmp.GetPixel(i - 1, j + 1).red
                            + uBmp.GetPixel(i, j + 1).red * 2
                            + uBmp.GetPixel(i + 1, j + 1).red);

                        mag /= 8;

                        eBmp.SetPixel(i, j, new PixelData((byte)0, (byte)0, (byte)0));
                    }
                }

                eBmp.UnlockBitmap();

                return eBmp.Bitmap;
            }*/
    }

    /*
 * TAKEN FROM:
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
