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
    public unsafe class RawBitmap : IDisposable
    {
        private Bitmap _originBitmap;
        private BitmapData _bitmapData;
        private byte* _begin;

        public RawBitmap(Bitmap originBitmap)
        {
            _originBitmap = originBitmap;

            //if(HasAlpha)
            _bitmapData = _originBitmap.LockBits(new Rectangle(0, 0, _originBitmap.Width, _originBitmap.Height), ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);
            //else
            //    _bitmapData = _originBitmap.LockBits(new Rectangle(0, 0, _originBitmap.Width, _originBitmap.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

            _begin = (byte*)(void*)_bitmapData.Scan0;
        }

        public bool HasAlpha
        {
            get
            {
                return (_originBitmap.PixelFormat | System.Drawing.Imaging.PixelFormat.Alpha) == _originBitmap.PixelFormat ? true : false;
            }
        }

        #region IDisposable Members

        public void Dispose()
        {
            _originBitmap.UnlockBits(_bitmapData);
        }

        #endregion

        public unsafe byte* Begin
        {
            get { return _begin; }
        }

        public unsafe byte* this[int x, int y]
        {
            get
            {
                return _begin + y * (_bitmapData.Stride) + x * 4;
            }
        }

        public unsafe byte* this[int x, int y, int offset]
        {
            get
            {
                return _begin + y * (_bitmapData.Stride) + x * 4 + offset;
            }
        }

        //public unsafe void SetColor(int x, int y, int color)
        //{
        //    *(int*)(_begin + y * (_bitmapData.Stride) + x * 3) = color;
        //}

        public int Stride
        {
            get { return _bitmapData.Stride; }
        }

        public int Width
        {
            get { return _bitmapData.Width; }
        }

        public int Height
        {
            get { return _bitmapData.Height; }
        }

        public int GetOffset()
        {
            return _bitmapData.Stride - _bitmapData.Width * 4;
        }

        public Bitmap OriginBitmap
        {
            get { return _originBitmap; }
        }
    }
}
