#ifndef FIND_TARGET_H
#define FIND_TARGET_H
//

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

using namespace cv;

//
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

//
class FindTarget
{

public:
  FindTarget ();
  ~FindTarget ();

  void tg_getFrame ();
  bool tg_ready ();
  void tg_saveImageIfAsked (int saveN);
  void tg_getTarget ();
  void tg_findSquares (int thresh);
  void tg_drawSquares ();
  void tg_printProperties ();

  inline int tg_getSlop ()
  {
    return slop;
  };
  inline int tg_getRowTarget ()
  {
    return rowTarget;
  };
  inline int tg_getTop ()
  {
    return top;
  };
private:
  VideoCapture cap;
  vector < vector < Point > >squares;
  Mat image;
  Mat blackWhite;
  int slop;
  int rowTarget;
  bool top;
  double tg_angle (Point & pt1, Point & pt2, Point & pt0);

};

#endif	// FIND_TARGET_H
