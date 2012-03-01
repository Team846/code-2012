
#include "global.h"

#if defined(_WIN32) || defined(__MINGW32__) || (defined(__GNUC__) && !defined(__BEAGLEBOARD__))
#if defined(USE_SATURATION_)
//#pragma region includes/usings
#include <stdio.h>
#include <ctime>

#include <opencv/cv.h>
#include <opencv/ml.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include "headers/messenger.h"

using namespace std;

//#pragma endregion

//#pragma region Preprocessor defines
#define CV_GETPIXEL(img, x, y, c) ((uchar*)(img->imageData + img->widthStep*y))[x*3+c]

#define DEBUGGING
//#undef DEBUGGING

#ifdef DEBUGGING
#define DbgPrint(a) cout << a << endl
#else
#define DbgPrint(a)
#endif

int threshold = 100;

int loRPosition = 0;
int hiRPosition = 255;

int loBPosition = 0;
int hiBPosition = 255;

void
mouseHandler(int event, int x, int y, int flags, void* param)
{
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        uchar*    ptr;

        IplImage *img0 = (IplImage*) param;
        IplImage *img1 = cvCloneImage(img0);

        /* read pixel */
        ptr = cvPtr2D(img1, y, x, NULL);

        /*
         * display the BGR value
         */
        cout << (int)ptr[0] << ", " << (int)ptr[1] << ", " << (int)ptr[2] << endl;
    }
}

int getKeyPixels(IplImage * frameIn)
{
	IplImage * redFrameOut = cvCreateImage(cvSize(frameIn->width, frameIn->height), IPL_DEPTH_8U, 3);
	IplImage * blueFrameOut = cvCreateImage(cvSize(frameIn->width, frameIn->height), IPL_DEPTH_8U, 3);

    int count=0;

    for(int y = 0; y < frameIn->height; y++)
    {
        for(int x = 0; x < frameIn->width; x++)
        {
            int hue = CV_GETPIXEL(frameIn, x, y, 0);
            int val = CV_GETPIXEL(frameIn, x, y, 2);
            int sat = CV_GETPIXEL(frameIn, x, y, 1);

            bool alreadyset = false;

			//int rPixel = CV_GETPIXEL(frameIn, x, y, 0);

			//rPixel = red;
			cvSet2D(redFrameOut, y, x, cvScalar(val, val, val));
			cvSet2D(blueFrameOut, y, x, cvScalar(sat, sat, sat));

            if(sat >= loRPosition)
            {
            	cvSet2D(redFrameOut, y, x, cvScalar(0, 0, 255));
                ++count;
                alreadyset = true;
            }/*
            if(sat >= loBPosition)
            {
            	//cvSet2D(blueFrameOut, y, x, cvScalar(255, 0, 0));
            	if(!alreadyset)
            		++count;
            }*/
        }
    }

	cvShowImage("red", redFrameOut);
	cvShowImage("blue", blueFrameOut);

    return count;
}

void onLoRThresholdSlide(int slideValue) { loRPosition = slideValue; }
void onHiRThresholdSlide(int slideValue) { hiRPosition = slideValue; }
void onLoBThresholdSlide(int slideValue) { loBPosition = slideValue; }
void onHiBThresholdSlide(int slideValue) { hiBPosition = slideValue; }

int main()
{
    IplImage * cap_img;
    CvCapture * cv_cap = cvCaptureFromCAM(0);
/*
    CvFont font;

    cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 0.8, 0.8, 0, 1, 8);

    IplImage grayscale;

    IplImage *cRed;
    IplImage *cBlue;
*/

    cvNamedWindow("video", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("operations", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("red", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("blue", CV_WINDOW_AUTOSIZE);

    cvCreateTrackbar("RedMin", "operations", &loRPosition, 255, onLoRThresholdSlide);
    cvCreateTrackbar("RedMax", "operations", &hiRPosition, 255, onHiRThresholdSlide);
    cvCreateTrackbar("BlueMin", "operations", &loBPosition, 255, onLoBThresholdSlide);
    cvCreateTrackbar("BlueMax", "operations", &hiBPosition, 255, onHiBThresholdSlide);

    int key;
    int frameNumber = 0;

    Messenger messenger;

    CvSize size;

    while(true)
    {
        clock_t start = clock();
        ++frameNumber;
        cap_img = cvQueryFrame(cv_cap);

        if(cap_img != 0)
        {
        	IplImage *hsv_img = cvCreateImage(cvSize(cap_img->width, cap_img->height), IPL_DEPTH_8U, 3);

        	cvCvtColor(cap_img, hsv_img, CV_RGB2HSV);

            int width = cap_img->width;
            int height = cap_img->height;
            size = cvSize(width, height);
            int matched = getKeyPixels(hsv_img);
            float matchedPixels = (float)matched / (float)(width*height);

        	cvSetMouseCallback("video", mouseHandler, (void*)cap_img);

			cvShowImage("video", cap_img);

			int value = 255 * matchedPixels;

			// DbgPrint(value);

			int iSent = messenger.sendData(frameNumber, value);

			cvReleaseImage(&hsv_img);

			//DbgPrint(value);

            // Remove double-slashes for death.
            // /*
            //cvReleaseImage(&cRed);
            //cvReleaseImage(&cBlue); //*/
            //cvReleaseImage(&cRedThresh);
            //cvReleaseImage(&cBlueThresh);

        }

        clock_t end = clock();

        key = cvWaitKey(1);

        if(key == 27)
            break;
    }

    cvReleaseCapture(&cv_cap);
    cvDestroyWindow("video");
    cvDestroyWindow("operations");
	cvDestroyWindow("red");
	cvDestroyWindow("blue");

    return 0;
}
#endif
#endif