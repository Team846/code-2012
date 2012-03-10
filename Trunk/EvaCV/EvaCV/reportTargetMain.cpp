/*  This program finds the targets (shaped like rectangles) in the frames provided
 *  by the camera. It then states the location of the square closest to the center in
 *  terms of the x-values. Lastly it figures out whether the square is on the top or 
 *  the middle.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h> 
#include <string.h> 
#include <signal.h>

#include "reportTarget.h"

/*  @namespace  cv: openCV namespace
 *  @namespace std: STL namespace
 */
using namespace cv;
using namespace std;

/*  @var         thresh: Sets the brightness threshold (where the program changes the
 *                       pixels to either black or white) to 200.
 *  @var        vThresh: Sets the vertical threshold (where the square is considered 
 *                       top) to 0.
 *  @var           camN: Sets the camera number to camera 6.
 *  @var          saveN: Sets the number of the file (the one to save to) to 0.
 *  @var sendConstantly: If present send packets constantly.
 */
static int thresh = 200;
static int vThresh = 0;
static int camN = 6;
static int saveN = 0;
static bool sendConstantly = false;

/*  @brief Handles signal 10.
 *  @param a: Signal number; must be 10.
 *
 *  Produces the number used in the file name to save the picture.
 */
void
onSignal (int a)
{			
  saveN++;
}

/*  @brief  Main function that calls all other functions.
 *  @param  argc: Number of arguments or variables.
 *  @param  argv: List of arguments or variables.
 *  @return 0
 *  
 *  Defines all arguments or variables. Creates the camera object and processes the 
 *  image by calling methods which take the image, turn it to black and white
 *  according to a specific threshold, find the targets in the image, locate the 
 *  target closest to the center on the x-axis, and determine whether it is the top or 
 *  middle target. In the process, one of the methods it calls, tells cRio how many
 *  pixels the target is off-center.
 */
int
main (int argc  , char ** argv  )
{
  /*  @class ReportSlop 
   *  @brief Sends packets to cRio.
   */
  ReportSlop reportSlop;

  /*  Arguments are defined in the following order depending on the value of argC.
   *  Default arguments: <brightnessThreshold(200)>
   *		             <verticalThreshold(0)>
   *                     <camera#(6)>
   *                     <sendConstantly(false)>
   */
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
  printf ("==== bThreshold %i ====@n", thresh);
  printf ("==== vThreshold %i ====@n", vThresh);
  printf ("==== camera %i ====@n", camN);
  printf ("==== sendConst %i ====@n", sendConstantly);

  /*  @class FindTarget
   *  @brief Creates target object for a specified camera number
   *  @param camN: camera number
   */
  FindTarget target(camN);

  /*  @brief Sets the brightness threshold.
   *  @param thresh: The brightness threshold.
   *
   *  @brief Sets the vertical threshold.
   *  @param vThresh: The vertical threshold.
   */
  target.tg_setBrightThresh (thresh);
  target.tg_setVertThresh (vThresh);

  /*  @brief Makes sure that the target object is ready.
   *  
   *  If it isn't, it prints out an error statement and returns -1.
   */
  if (!target.tg_ready ())
  {
      printf ("camera is not accessible@n");
      return -1;
  }

  //target.tg_printProperties ();

  /*  @brief Sets function onSignal as callback to signal 10.
   *  @param  SIGUSR1: Signal 10 provided by user.
   *  @param onSignal: Callback name.
   */
  signal (SIGUSR1, onSignal);

  clock_t start, end;

  int avgFrameCount = 30;
  int countt = 0;
  int nextPacketID = 1;

  for (;;)
  {
      //if (countt++ >= 10) exit(0);
	  ++countt;

      if (countt % avgFrameCount == 1) 
	  {
           start = clock();
      }

	  /*  @brief Gets a new frame from the camera.
	   */
      target.tg_getFrame ();

      // printf ("==== %i ====@n", countt++);

	  /*  @brief Finds the squares in the frame provided.
	   *  @param thresh: The Brightness threshold.
	   *
	   *  @brief CHooses the target closest to the center.
	   */
      target.tg_findSquares (thresh);
      target.tg_getTarget ();

      //printf ("slop = %i, top = %i, rowTarget = %i sendConstantly = %i@n", 
	  //        target.tg_getSlop (), target.tg_getTop (), target.tg_getRowTarget (), 
	  //        sendConstantly);

	  if ((target.tg_getSlop() != 0xfff) || sendConstantly) 
	  {
	       /*  @brief Sends the results to the cRio.
		    *
		    *  The results to the cRio include the slop, whether the target is the 
			*  top or middle one, and the package ID, respectively. Then it prints it
			*  out.
			*/
	       reportSlop.sendResults (target.tg_getSlop (), target.tg_getTop (), nextPacketID);
	       printf("---- %i --- %i @n",target.tg_getSlop (), target.tg_getTop ());
	       
		   ++nextPacketID;
      }

      //DDDtarget.tg_saveImageIfAsked (saveN);
      //waitKey (1);		
	  //^waits for a key: it also handles the GUI events.

      if (countt % avgFrameCount == 0) 
	  {
		    /*  @brief Times the process to recieve a number of frames per second.
			 */
            end = clock();
            double t = CLOCKS_PER_SEC * avgFrameCount * 1.0 / (end - start);
            printf( "rtFPS: %.02f@n",t);
      }
  }
  return 0;
}
