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

        public const double satThreshold = 0.15; //0<=x<=1

        public const float PPI_RATIO = 35; // assuming the image is the entire static test image, bitmap.height / 12 inches
        public static Point IMG_CENTER = new Point();
    }
}
