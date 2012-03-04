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
static int thresh = 200;
static int vThresh = 0;
static int camN = 6;
static int saveN = 0;
static bool sendConstantly = false;
//

void
onSignal (int a)
{				//signal 10 handler. produces number used in save picture filename
  saveN++;
}

int
main (int argc  , char ** argv  )
{
	// arguments: <brightnessThreshold(200)> <verticalThreshold(0)> <camera#(6)> <sendConstantly(false)>
  ReportSlop reportSlop;

  if (argc > 1)
    {
       thresh = atoi(argv[1]);
    }
  if (argc > 2)
  {
       vThresh = atoi(argv[2]);
  }
  if (argc > 3)
  {
       camN = atoi(argv[3]);
  }
  if (argc > 4)
    {
       sendConstantly = true;
    }
  printf ("==== bThreshold %i ====\n", thresh);
  printf ("==== vThreshold %i ====\n", vThresh);
  printf ("==== camera %i ====\n", camN);
  printf ("==== sendConst %i ====\n", sendConstantly);

  FindTarget target(camN);
  target.tg_setBrightThresh (thresh);
  target.tg_setVertThresh (vThresh);

  if (!target.tg_ready ())
    {
      printf ("camera is not accessible\n");
      return -1;
    }

  //target.tg_printProperties ();

  signal (SIGUSR1, onSignal);


    clock_t start, end;
    int avgFrameCount = 30;

int countt = 0;

  int nextPacketID = 1;

  for (;;)
    {
//if (countt++ >= 10) exit(0);
	++countt;

        if (countt % avgFrameCount == 1) {
            start = clock();
        }

      target.tg_getFrame ();	// get a new frame from camera
      // printf ("==== %i ====\n", countt++);

      target.tg_findSquares (thresh);
      target.tg_getTarget ();
      //printf ("slop = %i, top = %i, rowTarget = %i sendConstantly = %i\n", target.tg_getSlop (), target.tg_getTop (), target.tg_getRowTarget (), sendConstantly);
	if ((target.tg_getSlop() != 0xfff) || sendConstantly) {
	      reportSlop.sendResults (target.tg_getSlop (), target.tg_getTop (), nextPacketID);
	      printf("---- %i --- %i \n",target.tg_getSlop (), target.tg_getTop ());
	      
              ++nextPacketID;
        }
      //DDDtarget.tg_saveImageIfAsked (saveN);
//      waitKey (1);		//waits for a key: it also handles the GUI events.
        if (countt % avgFrameCount == 0) {
            end = clock();
            double t = CLOCKS_PER_SEC * avgFrameCount * 1.0 / (end - start);
            printf( "rtFPS: %.02f\n",t);
        }
    }
  return 0;
}
