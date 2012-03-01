#ifndef CV_FUNC_H_
#define CV_FUNC_H_

#include <cv.h>
#include <ml.h>
#include <cxcore.h>

void processImage(IplImage * img, int redThreshold, int blueThreshold, int *redcount, int *bluecount) {
    IplImage * r = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * g = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * b = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * rdiff = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * rdiffb = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * bdiff = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * bdiffr = cvCreateImage(cvGetSize(img), 8, 1);

    // split into rgb
    cvSplit(img, b, g, r, NULL);

    // get differences
    cvSub(r, g, rdiff);
    cvSub(r, b, rdiffb);

    cvSub(b, g, bdiff);
    cvSub(b, r, bdiffr);

    // threshold
    cvThreshold(rdiff, rdiff, redThreshold, 255, CV_THRESH_BINARY);
    cvThreshold(rdiffb, rdiffb, redThreshold, 255, CV_THRESH_BINARY);
    cvThreshold(bdiff, bdiff, blueThreshold, 255, CV_THRESH_BINARY);
    cvThreshold(bdiffr, bdiffr, blueThreshold, 255, CV_THRESH_BINARY);

    // recombine
    cvAdd(rdiff, rdiffb, r);
    cvAdd(bdiff, bdiffr, b);

    /*
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
    */

    *redcount = cvCountNonZero(r);
    *bluecount = cvCountNonZero(b);

#ifndef BEAGLEBOARD
    cvShowImage("red", r);
    cvShowImage("blue", b);
#endif
    cvReleaseImage(&r);
    cvReleaseImage(&g);
    cvReleaseImage(&b);
    cvReleaseImage(&rdiff);
    cvReleaseImage(&rdiffb);
    cvReleaseImage(&bdiff);
    cvReleaseImage(&bdiffr);
}

#endif
