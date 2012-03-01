#ifndef CV_FUNC_H_
#define CV_FUNC_H_

#include <cv.h>
#include <ml.h>
#include <cxcore.h>

void processImage(IplImage * img, int redThreshold, int blueThreshold, int *redcount, int *bluecount) {
    IplImage * r = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * g = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * b = cvCreateImage(cvGetSize(img), 8, 1);
    // split into rgb
    cvSplit(img, b, g, r, NULL);

    // get rid of white in the red channel
    cvAdd(b, g, g);
    cvSub(r, g, r);

    // get rid of white in the blue channel
    cvAdd(r, g, g);
    cvSub(g, b, g);
    cvSub(b, g, b);

    // threshold red
    // cvCmpS(r, loRPosition, r, CV_CMP_LE);
    // cvCmpS(r, hiRPosition, r, CV_CMP_GE);
    cvThreshold(r, r, redThreshold, 255, CV_THRESH_BINARY);

    // threshold blue
    //cvCmpS(b, loBPosition, b, CV_CMP_LE);
    //cvCmpS(b, hiBPosition, b, CV_CMP_GE);
    cvThreshold(b, b, blueThreshold, 255, CV_THRESH_BINARY);

    *redcount = cvCountNonZero(r);
    *bluecount = cvCountNonZero(b);

    cvShowImage("red", r);
    cvShowImage("blue", b);
    cvReleaseImage(&r);
    cvReleaseImage(&g);
    cvReleaseImage(&b);
}

#endif
