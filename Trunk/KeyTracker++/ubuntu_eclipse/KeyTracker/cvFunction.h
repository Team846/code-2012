#ifndef CV_FUNC_H_
#define CV_FUNC_H_

#include <cv.h>
#include <ml.h>
#include <cxcore.h>

#define USE_DIFFERENCE_METHOD 1

void processImage(IplImage * cap_img, int redThreshold, int blueThreshold, int *redcount, int *bluecount) {
    IplImage * img = cvCreateImage(cvSize(cap_img->width / 2, cap_img->height / 2), cap_img->depth, cap_img->nChannels);
    
    cvPyrDown(cap_img, img);

    IplImage * r = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * g = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * b = cvCreateImage(cvGetSize(img), 8, 1);

#if USE_DIFFERENCE_METHOD
    IplImage * rdiff = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * rdiffb = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * bdiff = cvCreateImage(cvGetSize(img), 8, 1);
    IplImage * bdiffr = cvCreateImage(cvGetSize(img), 8, 1);
#endif

    // split into rgb
    cvSplit(img, b, g, r, NULL);

#if USE_DIFFERENCE_METHOD
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
#else
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
#endif

    *redcount = cvCountNonZero(r);
    *bluecount = cvCountNonZero(b);

#ifndef BEAGLEBOARD
    cvShowImage("red", r);
    cvShowImage("blue", b);
#endif
    cvReleaseImage(&r);
    cvReleaseImage(&g);
    cvReleaseImage(&b);
#if USE_DIFFERENCE_METHOD
    cvReleaseImage(&rdiff);
    cvReleaseImage(&rdiffb);
    cvReleaseImage(&bdiff);
    cvReleaseImage(&bdiffr);
#endif
    cvReleaseImage(&img);
}

#endif
