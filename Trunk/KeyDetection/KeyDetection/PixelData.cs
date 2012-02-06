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
using System.Linq;
using System.Text;

namespace KeyDetection
{
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
}
