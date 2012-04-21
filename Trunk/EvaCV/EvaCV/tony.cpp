#pragma region includes/usings
#include <opencv.hpp>

#include <stdio.h>

using namespace std;
#pragma endregion

#define DEBUGGING

CvScalar lowThreshold;
CvScalar highThreshold;

int smoothOption = 0;
bool useSmooth = true; // false - hist

int loHPosition = 0;
int loSPosition = 0;
int loVPosition = 0;

int hiHPosition = 255;
int hiSPosition = 255;
int hiVPosition = 255;

void onSmoothChange(int slideValue)
{
	useSmooth = slideValue == 0;
}

void onLoHThresholdSlide(int slideValue)
{
	loHPosition = slideValue;
	lowThreshold = cvScalar(loHPosition, loSPosition, loVPosition);
}

void onHiHThresholdSlide(int slideValue)
{
	hiHPosition = slideValue;
	highThreshold = cvScalar(hiHPosition, hiSPosition, hiVPosition);
}

void onLoSThresholdSlide(int slideValue)
{
	loSPosition = slideValue;
	lowThreshold = cvScalar(loHPosition, loSPosition, loVPosition);
}

void onHiSThresholdSlide(int slideValue)
{
	hiSPosition = slideValue;
	highThreshold = cvScalar(hiHPosition, hiSPosition, hiVPosition);
}

void onLoVThresholdSlide(int slideValue)
{
	loVPosition = slideValue;
	lowThreshold = cvScalar(loHPosition, loSPosition, loVPosition);
}

void onHiVThresholdSlide(int slideValue)
{
	hiVPosition = slideValue;
	highThreshold = cvScalar(hiHPosition, hiSPosition, hiVPosition);
}

int mainNo()
{
	IplImage * cap_img;
	CvCapture * cv_cap = cvCaptureFromCAM(0);

	lowThreshold = cvScalar(0, 0, 0);

	highThreshold = cvScalar(255, 255, 255);
	
	cvNamedWindow("video", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("grayscale", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("threshold", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("operations", CV_WINDOW_AUTOSIZE);

	cvCreateTrackbar("0=S/1=H", "operations", &smoothOption, 1, onSmoothChange);

	cvCreateTrackbar("Lo H", "operations", &loHPosition, 255, onLoHThresholdSlide);
	cvCreateTrackbar("Hi H", "operations", &hiHPosition, 255, onHiHThresholdSlide);

	cvCreateTrackbar("Lo S", "operations", &loSPosition, 255, onLoSThresholdSlide);
	cvCreateTrackbar("Hi S", "operations", &hiSPosition, 255, onHiSThresholdSlide);

	cvCreateTrackbar("Lo V", "operations", &loVPosition, 255, onLoVThresholdSlide);
	cvCreateTrackbar("Hi V", "operations", &hiVPosition, 255, onHiVThresholdSlide);
	
	int key;

	CvSize size;

	while(true)
	{
		cap_img = cvQueryFrame(cv_cap);

		if(cap_img != 0)
		{	
			int width = cap_img->width;
			int height = cap_img->height;

			size = cvSize(width, height);

			cvShowImage("video", cap_img);

			IplImage * grayscale_img = cvCreateImage(size, IPL_DEPTH_8U, 1);

			cvCvtColor(cap_img, grayscale_img, CV_RGB2GRAY);

			cvShowImage("grayscale", grayscale_img);

			IplImage * hsv_img = cvCreateImage(size, IPL_DEPTH_8U, 3);
			cvCvtColor(cap_img, hsv_img, CV_RGB2HSV);

			IplImage * thresh_img = cvCreateImage(size, IPL_DEPTH_8U, 1);
			IplImage * smooth_img = cvCreateImage(size, IPL_DEPTH_8U, 1);

			cvInRangeS(hsv_img, lowThreshold, highThreshold, thresh_img);
			
			if(useSmooth)
				cvSmooth(thresh_img, smooth_img, CV_MEDIAN);
			else
				cvEqualizeHist(thresh_img, smooth_img);

			cvShowImage("threshold", smooth_img);

			cvReleaseImage(&grayscale_img);
			cvReleaseImage(&thresh_img);
			cvReleaseImage(&hsv_img);
		}
		
		key = cvWaitKey(1);

		if(key == 27)
			break;
	}

	cvReleaseCapture(&cv_cap);
	cvDestroyWindow("video");
	cvDestroyWindow("grayscale");
	cvDestroyWindow("threshold");
	cvDestroyWindow("operations");

	return 0;
}