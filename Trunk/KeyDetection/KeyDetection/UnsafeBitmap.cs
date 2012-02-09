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