/** h file for ReportTarget.
 *
 *  This program finds the targets (shaped like rectangles) in the frames provided
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

/** @namespace  cv: openCV namespace
 */
using namespace cv;

/** @class ReportSlop
 *  @brief Sends packets with the slop in it.
 */
class ReportSlop
{
  public:
    int sendResults (int slop, bool top);
       ReportSlop ();
      ~ReportSlop ();

  private:
    void rs_init ();
    void rs_reset ();
    int m_soc;
    struct sockaddr_in m_dst;
};

/** @class FindTarget
 *  @brief Finds the target closest to the center.
 * 
 *  The class returns the distance from the target to the center on the x-axis, and
 *  whether the target is the top or middle one.
 */
class FindTarget
{
  public:
     FindTarget ();
    ~FindTarget ();
    void tg_getFrame ();                             //Get frame from camera.
    bool tg_ready ();                                //Check that camera is ready.
    void tg_saveImageIfAsked (int saveN);            //Save frame to disk.
    void tg_getTarget ();                            //Find the target.
    void tg_findSquares (int thresh, bool showWin);  //Find the squares in the image.
    void tg_drawSquares ();                          //Outline the squares.
    void tg_printProperties ();                      //Prints all camera parameters.
    inline int tg_getSlop ()        //Finds distance between target and center.
    {
      return slop;
    };
    inline int tg_getRowTarget ()    
    {
      return rowTarget;
    };
    inline int tg_getTop ()         //Finds whether target is top or middle one.
    {
      return top;
    };

  private:
    VideoCapture cap;
    vector < vector < Point > >squares;
    Mat image;
    Mat blackWhite; 
    int slop;         //Distance between target and center of screen.
    int rowTarget;        
    bool top;         //Top or middle target.   
    double tg_angle (Point & pt1, Point & pt2, Point & pt0);  //Finds right angles.
};