#ifndef CV_FUNC_H_
#define CV_FUNC_H_

#include <opencv2/opencv.hpp>

#define USE_DIFFERENCE_METHOD 1

void processImage(cv::Mat& cap_img, int redThreshold, int blueThreshold, int *redcount, int *bluecount) {
    cv::Mat img = cv::Mat(cap_img.cols / 2, cap_img.rows / 2, cap_img.type());
    
    cv::pyrDown(cap_img, img);

    cv::Mat r = cv::Mat(img.size(), CV_8UC1);
    cv::Mat g = cv::Mat(img.size(), CV_8UC1);
    cv::Mat b = cv::Mat(img.size(), CV_8UC1);

#if USE_DIFFERENCE_METHOD
    cv::Mat rdiff = cv::Mat(img.size(), CV_8UC1);
    cv::Mat rdiffb = cv::Mat(img.size(), CV_8UC1);
    cv::Mat bdiff = cv::Mat(img.size(), CV_8UC1);
    cv::Mat bdiffr = cv::Mat(img.size(), CV_8UC1);
#endif

    // split into rgb
    cv::Mat bgr[] = {b, g, r};
    cv::split(img, bgr);

#if USE_DIFFERENCE_METHOD
    // get differences
    rdiff = r - g;
    rdiffb = r - b;
    bdiff = b - g;
    bdiffr = b - r;

    // threshold
    cv::threshold(rdiff, rdiff, redThreshold, 255, CV_THRESH_BINARY);
    cv::threshold(rdiffb, rdiffb, redThreshold, 255, CV_THRESH_BINARY);
    cv::threshold(bdiff, bdiff, blueThreshold, 255, CV_THRESH_BINARY);
    cv::threshold(bdiffr, bdiffr, blueThreshold, 255, CV_THRESH_BINARY);

    // recombine
    r = rdiff + rdiffb;
    b = bdiff + bdiffr;
#else
    // get rid of white in the red channel
    g = b + g;
    r = r - g;

    // get rid of white in the blue channel
    r = r + g;
    g = g - b;
    b = b - g;

    cv::threshold(r, r, redThreshold, 255, CV_THRESH_BINARY);
    cv::threshold(b, b, blueThreshold, 255, CV_THRESH_BINARY);
#endif

    *redcount = cv::countNonZero(r);
    *bluecount = cv::countNonZero(b);

#ifndef BEAGLEBOARD
    cv::imshow("red", r);
    cv::imshow("blue", b);
#endif
}

#endif
