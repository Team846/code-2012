//
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <signal.h>
//
using namespace cv;
using namespace std;
//
static int thresh = 100;
static int save = 0;
//
void
printSeq (const vector < vector < Point > >&squares, int n)
{
  char name[32];
  sprintf (name, "nseq%02i.txt", n);
  FILE *fp = fopen (name, "w");
  if (fp == NULL)
    {
      printf ("lazha\n");
    }

  else
    {

      // read 4 sequence elements at a time (all vertices of a square)
      for (size_t i = 0; i < squares.size (); i++)
	{
	  for (size_t j = 0; j < squares[i].size (); j++)
	    {
	      const Point *p = &squares[i][j];
	      fprintf (fp, "(%i,%i), ", p->x, p->y);
	    } fprintf (fp, "\n");
	} fclose (fp);
    }
}

//
void
getTarget (const vector < vector < Point > >&squares, int *slop, bool * top)
{
  const int width2 = 1280;
  int sum = 0;
  int delta4X = width2;
  int rowTarget = 0;
  int *weight = new int[squares.size ()];
  memset (weight, 0, sizeof (int) * squares.size ());

  // read 4 sequence elements at a time (all vertices of a square)
  for (size_t row = 0; row < squares.size (); row++)
    {
      sum = 0;
      for (size_t col = 0; col < squares[row].size (); col++)
	{
	  const Point *p = &squares[row][col];
	  sum = sum + p->x;
	  if ((col == 0) && (row < squares.size () - 1))
	    {
	      for (size_t yRow = row + 1; yRow < squares.size (); yRow++)
		{
		  const Point *f = &squares[yRow][0];
		  if (p->y > f->y)
		    {
		      weight[row]++;
		    }
		  else
		    {
		      weight[yRow]++;
		    }
		}

	    }
	}
      int deltaAbs = abs (sum - width2);
      printf (" sum = %i width2 = %i deltaAbs = %i delta4x = %i\n",
	      sum, width2, deltaAbs, delta4X);
      if (delta4X > deltaAbs)
	{
	  rowTarget = row;
	  printf ("here %i\n", rowTarget);
	  delta4X = deltaAbs;
	}
    }
  for (int k = 0; k < squares.size (); k++)
    {
      printf ("%i = %i\n", k, weight[k]);
    }
  printf ("size = %i rowTarget = %i\n", squares.size (), rowTarget);
  *slop = delta4X / 4;
  *top = weight[rowTarget] < 2;
  delete[]weight;
}

//
double
angle (Point & pt1, Point & pt2, Point & pt0)
{

// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
  double dx1 = pt1.x - pt0.x;
  double dy1 = pt1.y - pt0.y;
  double dx2 = pt2.x - pt0.x;
  double dy2 = pt2.y - pt0.y;
  return (dx1 * dx2 +
	  dy1 * dy2) / sqrt ((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 +
							dy2 * dy2) + 1e-10);
}

//
void
findSquares (const Mat & image, vector < vector < Point > >&squares)
{

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
  squares.clear ();
  Mat colorPlane (image.size (), CV_8U), blackWhite;
  vector < vector < Point > >contours;

  {
    int color = 1;		// red channel
    int ch[] = { color, 0 };
    mixChannels (&image, 1, &colorPlane, 1, ch, 1);

    {

#ifdef USE_CANNY
      // hack: use Canny instead of zero threshold level.
      // Canny helps to catch squares with gradient shading

      {

	// apply Canny. Take the upper threshold from slider
	// and set the lower to 0 (which forces edges merging)
	Canny (colorPlane, blackWhite, 0, thresh, 5);

	// dilate canny output to remove potential
	// holes between edge segments
	dilate (blackWhite, blackWhite, Mat (), Point (-1, -1));
      }
#else // USE_CANNY
      {

	// apply threshold if l!=0:
	//     tblackWhite(x,y) = blackWhite(x,y) < (l+1)*255/N ? 255 : 0
	blackWhite = colorPlane >= thresh;
      }
#endif // USE_CANNY
      imshow ("bw", blackWhite);

      // find contours and store them all as a list
      findContours (blackWhite, contours, CV_RETR_LIST,
		    CV_CHAIN_APPROX_SIMPLE);
      vector < Point > approx;

      // test each contour
      for (size_t i = 0; i < contours.size (); i++)
	{

	  // approximate contour with accuracy proportional
	  // to the contour perimeter
	  approxPolyDP (Mat (contours[i]), approx,
			arcLength (Mat (contours[i]), true) * 0.02, true);

	  // square contours should have 4 vertices after approximation
	  // relatively large area (to filter out noisy contours)
	  // and be convex.
	  // Note: absolute value of an area is used because
	  // area may be positive or negative - in accordance with the
	  // contour orientation
	  if (approx.size () == 4
	      && fabs (contourArea (Mat (approx))) > 1000
	      && isContourConvex (Mat (approx)))
	    {
	      double maxCosine = 0;
	      for (int j = 2; j < 5; j++)
		{

		  // find the maximum cosine of the angle between joint edges
		  double cosine = fabs (angle (approx[j % 4],
					       approx[j - 2],
					       approx[j - 1]));
		  maxCosine = MAX (maxCosine, cosine);
		}
	      // if cosines of all angles are small
	      // (all angles are ~90 degree) then write quandrange
	      // vertices to resultant sequence
	      if (maxCosine < 0.3)
		squares.push_back (approx);
	    }
	}
    }
  }
}

// the function draws all the squares in the image
void
drawSquares (Mat & image, const vector < vector < Point > >&squares)
{
  for (size_t i = 0; i < squares.size (); i++)
    {
      const Point *p = &squares[i][0];
      int n = (int) squares[i].size ();
      polylines (image, &p, &n, 1, true, Scalar (0, 255, 0), 3, CV_AA);
    } imshow ("squares", image);
}

void
on_trackbar (int a, void *unused)
{
}

void
saver (int a)
{				//signal 10 handler. produces number used in save picture filename
  save++;
}

//
void
printProperties (VideoCapture & cap)
{
  printf ("CV_CAP_PROP_POS_MSEC=%f\n", cap.get (CV_CAP_PROP_POS_MSEC));
  printf ("CV_CAP_PROP_POS_FRAMES=%f\n", cap.get (CV_CAP_PROP_POS_FRAMES));
  printf ("CV_CAP_PROP_POS_AVI_RATIO=%f\n",
	  cap.get (CV_CAP_PROP_POS_AVI_RATIO));
  printf ("CV_CAP_PROP_FRAME_WIDTH=%f\n", cap.get (CV_CAP_PROP_FRAME_WIDTH));
  printf ("CV_CAP_PROP_FRAME_HEIGHT=%f\n",
	  cap.get (CV_CAP_PROP_FRAME_HEIGHT));
  printf ("CV_CAP_PROP_FPS=%f\n", cap.get (CV_CAP_PROP_FPS));
  printf ("CV_CAP_PROP_FOURCC=%f\n", cap.get (CV_CAP_PROP_FOURCC));
  printf ("CV_CAP_PROP_FRAME_COUNT=%f\n", cap.get (CV_CAP_PROP_FRAME_COUNT));
  printf ("CV_CAP_PROP_FORMAT=%f\n", cap.get (CV_CAP_PROP_FORMAT));
  printf ("CV_CAP_PROP_MODE=%f\n", cap.get (CV_CAP_PROP_MODE));
  printf ("CV_CAP_PROP_BRIGHTNESS=%f\n", cap.get (CV_CAP_PROP_BRIGHTNESS));
  printf ("CV_CAP_PROP_CONTRAST=%f\n", cap.get (CV_CAP_PROP_CONTRAST));
  printf ("CV_CAP_PROP_SATURATION=%f\n", cap.get (CV_CAP_PROP_SATURATION));
  printf ("CV_CAP_PROP_HUE=%f\n", cap.get (CV_CAP_PROP_HUE));
  printf ("CV_CAP_PROP_GAIN=%f\n", cap.get (CV_CAP_PROP_GAIN));
  printf ("CV_CAP_PROP_EXPOSURE=%f\n", cap.get (CV_CAP_PROP_EXPOSURE));
  printf ("CV_CAP_PROP_CONVERT_RGB=%f\n", cap.get (CV_CAP_PROP_CONVERT_RGB));
  printf ("CV_CAP_PROP_RECTIFICATION=%f\n",
	  cap.get (CV_CAP_PROP_RECTIFICATION));
}

//
int
main (int /*argc */ , char ** /*argv */ )
{
  VideoCapture cap (0);
  if (!cap.isOpened ())
    return -1;
  printProperties (cap);
  namedWindow ("squares", CV_WINDOW_AUTOSIZE);
  namedWindow ("bw", CV_WINDOW_AUTOSIZE);
  signal (SIGUSR1, saver);
  createTrackbar ("tresh", "bw", &thresh, 255, on_trackbar, NULL);
  vector < vector < Point > >squares;

  //int countt = 0;
  int savesave = 0;
  Mat image;
  for (;;)
    {

      // printf ("==== %i ====\n", countt++);
      cap >> image;		// get a new frame from camera
      findSquares (image, squares);
      drawSquares (image, squares);
      int slop = 0;
      bool top = false;
      getTarget (squares, &slop, &top);
      printf ("slop = %i, top = %i\n", slop, top);
      if (savesave != save)
	{
	  savesave = save;
	  char name[32];
	  sprintf (name, "ndata%02i.bmp", save);
	  imwrite (name, image);
	  printSeq (squares, save);
	}
      waitKey (1);		//waits for a key: it also handles the GUI events.
    }
  return 0;
}
