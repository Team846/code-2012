﻿/*
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
        /*static PixelData redHigh = new PixelData(69, 30, 48); //if we have a red key
        static PixelData redLow = new PixelData(114, 70, 103);
        static PixelData blueHigh; //If we have a blue key
        static PixelData blueLow;*/

        public static double SAT_THRESHOLD = 0.15; //0<=x<=1
        public static double EDGE_THRESHOLD = 6;
        public static bool FILTER_BLACKWHITE = true;

        public static string IMG_FILENAME = "";

        public static float PPI_RATIO = 0; // assuming the image is the entire static test image, bitmap.height / 12 inches
        public static Point IMG_CENTER = new Point();
    }
}