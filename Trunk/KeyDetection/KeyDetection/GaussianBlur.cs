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
    public class GaussianBlur
    {
        public static UnsafeBitmap DoBlur(UnsafeBitmap uBmp, int radius)
        {
            Bitmap blurred = new Bitmap(uBmp.Bitmap.Width, uBmp.Bitmap.Height);
            int[] kernel;
            int kernelSum = 0;
            int[,] multable;
            int sz = radius * 2 + 1;
            kernel = new int[sz];
            multable = new int[sz, 256];
            for (int i = 1; i <= radius; i++)
            {
                int szi = radius - i;
                int szj = radius + i;
                kernel[szj] = kernel[szi] = (szi + 1) * (szi + 1);
                kernelSum += (kernel[szj] + kernel[szi]);
                for (int j = 0; j < 256; j++)
                {
                    multable[szj, j] = multable[szi, j] = kernel[szj] * j;
                }
            }
            kernel[radius] = (radius + 1) * (radius + 1);
            kernelSum += kernel[radius];
            for (int j = 0; j < 256; j++)
            {
                multable[radius, j] = kernel[radius] * j;
            }
            using (RawBitmap src = new RawBitmap(uBmp.Bitmap))
            {
                using (RawBitmap dest = new RawBitmap(blurred))
                {
                    int pixelCount = src.Width * src.Height;
                    int[] b = new int[pixelCount];
                    int[] g = new int[pixelCount];
                    int[] r = new int[pixelCount];

                    int[] b2 = new int[pixelCount];
                    int[] g2 = new int[pixelCount];
                    int[] r2 = new int[pixelCount];

                    int offset = src.GetOffset();
                    int index = 0;
                    unsafe
                    {
                        byte* ptr = src.Begin;
                        for (int i = 0; i < src.Height; i++)
                        {
                            for (int j = 0; j < src.Width; j++)
                            {
                                b[index] = *ptr;
                                ptr++;
                                g[index] = *ptr;
                                ptr++;
                                r[index] = *ptr;
                                ptr++;

                                ++index;
                            }
                            ptr += offset;
                        }

                        int bsum;
                        int gsum;
                        int rsum;
                        int sum;
                        int read;
                        int start = 0;
                        index = 0;
                        for (int i = 0; i < uBmp.Bitmap.Height; i++)
                        {
                            for (int j = 0; j < uBmp.Bitmap.Width; j++)
                            {
                                bsum = gsum = rsum = sum = 0;
                                read = index - radius;

                                for (int z = 0; z < kernel.Length; z++)
                                {
                                    if (read >= start && read < start + uBmp.Bitmap.Width)
                                    {
                                        bsum += multable[z, b[read]];
                                        gsum += multable[z, g[read]];
                                        rsum += multable[z, r[read]];
                                        sum += kernel[z];
                                    }
                                    ++read;
                                }

                                b2[index] = (bsum / sum);
                                g2[index] = (gsum / sum);
                                r2[index] = (rsum / sum);

                                ++index;
                            }
                            start += uBmp.Bitmap.Width;
                        }
                        int tempy;
                        for (int i = 0; i < uBmp.Bitmap.Height; i++)
                        {
                            int y = i - radius;
                            start = y * uBmp.Bitmap.Width;
                            for (int j = 0; j < uBmp.Bitmap.Width; j++)
                            {
                                bsum = gsum = rsum = sum = 0;
                                read = start + j;
                                tempy = y;
                                for (int z = 0; z < kernel.Length; z++)
                                {
                                    if (tempy >= 0 && tempy < uBmp.Bitmap.Height)
                                    {
                                        bsum += multable[z, b2[read]];
                                        gsum += multable[z, g2[read]];
                                        rsum += multable[z, r2[read]];
                                        sum += kernel[z];
                                    }
                                    read += uBmp.Bitmap.Width;
                                    ++tempy;
                                }

                                byte* pcell = dest[j, i];
                                pcell[0] = (byte)(bsum / sum);
                                pcell[1] = (byte)(gsum / sum);
                                pcell[2] = (byte)(rsum / sum);
                            }
                        }
                    }
                }
            }
            uBmp = new UnsafeBitmap(blurred);
            return uBmp;
        }
    }
}
