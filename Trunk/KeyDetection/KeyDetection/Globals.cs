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
using System.Drawing;
using System.Linq;
using System.Text;

namespace KeyDetection
{
    public class Globals
    {
        //Color thresholding
        /*static PixelData redHigh = new PixelData(69, 30, 48); //if we have a red key
        static PixelData redLow = new PixelData(114, 70, 103);
        static PixelData blueHigh; //If we have a blue key
        static PixelData blueLow;*/

        //Saturation thresholding
        public static double satThreshold = 0.15; //0<=x<=1
        public static double edgeThreshold = 6;
        public static Boolean threshold = true;

        public static float PPI_RATIO = 1; // assuming the image is the entire static test image
        public static Point IMG_CENTER = new Point();
    }
}
