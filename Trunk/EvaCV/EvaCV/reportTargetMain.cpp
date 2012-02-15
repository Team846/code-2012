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
static int thresh = 100;
static int saveN = 0;
//

void
onSignal (int a)
{				//signal 10 handler. produces number used in save picture filename
  saveN++;
}

void
onTrackbar (int a, void *unused)
{
}

int
main (int argc  , char ** argv  )
{
  FindTarget target;
  ReportSlop reportSlop;

  if (!target.tg_ready ())
    {
      printf ("camera is nmot accessible\n");
      return -1;
    }

  target.tg_printProperties ();

  namedWindow ("squares", CV_WINDOW_AUTOSIZE);
  if (argc > 1)
  namedWindow ("bw", CV_WINDOW_AUTOSIZE);
  createTrackbar ("tresh", "bw", &thresh, 255, onTrackbar, NULL);
  signal (SIGUSR1, onSignal);

  for (;;)
    {

      target.tg_getFrame ();	// get a new frame from camera
      // printf ("==== %i ====\n", countt++);

      target.tg_findSquares (thresh, argc > 1);
      target.tg_getTarget ();
      printf ("slop = %i, top = %i, rowTarget = %i\n", target.tg_getSlop (),
	      target.tg_getTop (), target.tg_getRowTarget ());
      target.tg_drawSquares ();
      reportSlop.sendResults (target.tg_getSlop (), target.tg_getTop ());
      target.tg_saveImageIfAsked (saveN);
      waitKey (1);		//waits for a key: it also handles the GUI events.
    }
  return 0;
}
