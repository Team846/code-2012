//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <signal.h>

#include "reportTarget.h"

//
using namespace cv;
using namespace std;
//

ReportSlop::ReportSlop ()
{
  rs_init ();
}

void
ReportSlop::rs_init ()
{
  m_soc = socket (AF_INET, SOCK_DGRAM, 0);
  if (m_soc < 0)
    {
      perror ("socket:");
    }
  else
    {

      int bcast = 1;
      if (setsockopt (m_soc, SOL_SOCKET, SO_BROADCAST, &bcast, sizeof (bcast))
	  < 0)
	{
	  perror ("setsockopt:");
	  rs_reset ();
	}

      int reuse = 1;
      if (setsockopt (m_soc, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse))
	  < 0)
	{
	  perror ("setsockopt:");
	  rs_reset ();
	}

      m_dst.sin_family = AF_INET;
      m_dst.sin_addr.s_addr = htonl (INADDR_BROADCAST);
      m_dst.sin_port = htons (8000);

    }
}

ReportSlop::~ReportSlop ()
{
  rs_reset ();
}

void
ReportSlop::rs_reset ()
{
  close (m_soc);
  m_soc = -1;
}

int
ReportSlop::sendResults (int slop, bool top)
{
  if (m_soc < 0)
    {
      rs_init ();
    }

  if (m_soc < 0)
    {
      return -1;
    }

  char buf[2];
  buf[0] = slop;
  buf[1] = top;

  if (sendto (m_soc, buf, sizeof (buf), 0, (struct sockaddr *) &m_dst,
	      sizeof (m_dst)) < 0)
    {
      perror ("sendto:");
      rs_reset ();
      return -1;
    }
  return 0;
}

void
FindTarget::tg_saveImageIfAsked (int saveN)
{
  static int saveNsave = 0;
  if (saveNsave != saveN)
    {
      saveNsave = saveN;
      char name[32];
      sprintf (name, "ndata%02i.bmp", saveN);
      imwrite (name, image);

      sprintf (name, "nseq%02i.txt", saveN);
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
		}
	      fprintf (fp, "\n");
	    }
	  fclose (fp);
	}
    }
}

	//
void
FindTarget::tg_getTarget ()
{
  if (squares.size() != 0)
    {
      const int width2 = 1280;
      int sum = 0;
      int delta4xAbsMin = width2;
      int delta4xMin = width2;
      int delta4x = 0;
      int *weight = new int[squares.size ()];
      memset (weight, 0, sizeof (int) * squares.size ());

      for (size_t i = 0; i < squares.size (); i++)
	{
	  for (size_t j = 0; j < squares[i].size (); j++)
	    {
	      const Point *p = &squares[i][j];
	      printf ("(%i,%i), ", p->x, p->y);
	    }
	  printf ("\n");
	}

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
	  delta4x = sum - width2;
	  int delta4xAbs = abs (delta4x);
	  printf
	    (" row %i sum = %i width2 = %i delta4xAbsMin = %i delta4x = %i\n",
	     row, sum, width2, delta4xAbsMin, delta4x);
	  if (delta4xAbs < delta4xAbsMin)
	    {
	      rowTarget = row;
	      delta4xAbsMin = delta4xAbs;
	      delta4xMin = delta4x;
	    }
	}
      for (int k = 0; k < squares.size (); k++)
	{
	  printf ("%i = %i,  ", k, weight[k]);
	}
      printf ("\n");

      slop = delta4xMin / 4;
      top = weight[rowTarget] < 2;
      delete[]weight;
    }
  else
    {
      slop = 0xfff;
      top = true;
      rowTarget = -1;
    }
}

	//
double
FindTarget::tg_angle (Point & pt1, Point & pt2, Point & pt0)
{

  // finds a cosine of angle between vectors
  // from pt0->pt1 and from pt0->pt2
  double dx1 = pt1.x - pt0.x;
  double dy1 = pt1.y - pt0.y;
  double dx2 = pt2.x - pt0.x;
  double dy2 = pt2.y - pt0.y;
  return (dx1 * dx2 + dy1 * dy2) / sqrt ((dx1 * dx1 +
					  dy1 * dy1) * (dx2 * dx2 +
							dy2 * dy2) + 1e-10);
}

	//
void
FindTarget::tg_findSquares (int thresh)
{

  // returns sequence of squares detected on the image.
  // the sequence is stored in the specified memory storage
  squares.clear ();
  Mat colorPlane (image.size (), CV_8U);
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
	  double area = fabs (contourArea (Mat (approx)));
	  if ((approx.size () == 4) && (area < 200000) && (area > 1000)
	      && isContourConvex (Mat (approx)))
	    {
	      double maxCosine = 0;
	      for (int j = 2; j < 5; j++)
		{

		  // find the maximum cosine of the angle between joint edges
		  double cosine =
		    fabs (tg_angle (approx[j % 4], approx[j - 2],
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

//
void
FindTarget::tg_printProperties ()
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

FindTarget::FindTarget ():
cap (0), squares (), image (), slop (-1), rowTarget (-1), top (true)
{
}

FindTarget::~FindTarget ()
{

}

	//
bool
FindTarget::tg_ready ()
{
  return cap.isOpened ();
}

	//
void
FindTarget::tg_getFrame ()
{
  cap >> image;			// get a new frame from camera
}
