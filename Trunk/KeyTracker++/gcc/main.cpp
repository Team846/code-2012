#pragma region includes/usings
#include <stdio.h>
#include <ctime>

using namespace std;

#include <cv.h>
#include <ml.h>
#include <cxcore.h>
#include <highgui.h>

#pragma endregion

#pragma region Preprocessor defines
#define CV_GETPIXEL(img, x, y, c) ((uchar*)(img->imageData + img->widthStep*y))[x*3+c]

#define PRODUCTION_BUILD
//#undef PRODUCTION_BUILD

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

int getKeyPixels(IplImage * frameIn)
{
	//IplImage * redFrameOut = cvCreateImage(cvSize(frameIn->width, frameIn->height), IPL_DEPTH_8U, 1);

    int count=0;

    for(int y = 0; y < frameIn->height; y++)
    {
        for(int x = 0; x < frameIn->width; x++)
        {
            int blue = CV_GETPIXEL(frameIn, x, y, 0);
			int green = CV_GETPIXEL(frameIn, x, y, 1);
            int red = CV_GETPIXEL(frameIn, x, y, 2);

			//int rPixel = CV_GETPIXEL(frameIn, x, y, 0);

			//rPixel = red;

			//cvSet2D(redFrameOut, y, x, cvScalar(red));

            if(red >= loRPosition || blue >= loBPosition)
                ++count;
        }
    }

	//cvShowImage("red", redFrameOut);

    return count;
}

#ifdef PRODUCTION_BUILD
int main()
{
	/* Open capture stream */
	CvCapture *pCapture = cvCaptureFromCAM(0);
	IplImage *pCaptureImg;

	int pressed = -1;

	int frameNumber = 0;

	while(pressed != 27)
	{
		/* Increment frame number */
		++frameNumber;

		/* These preprocessor blocks look ugly... */
#ifdef DEBUGGING
		int begin = clock();
#endif
		pCaptureImg = cvQueryFrame(pCapture);

		/* Verify that we queried a proper image */
		if(pCaptureImg != 0)
		{
			int matchedPixels = getKeyPixels(pCaptureImg);

			DbgPrint(matchedPixels);
		}

#ifdef DEBUGGING
		DbgPrint("Frame#" << frameNumber << " took " << (clock() - begin) << " ms.");
#endif
		//pressed = cvWaitKey(1);
	}
}
#else
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

    cvCreateTrackbar("RedMin", "operations", &loRPosition, 255, onLoRThresholdSlide);
    cvCreateTrackbar("BlueMin", "operations", &loBPosition, 255, onLoBThresholdSlide);

    int key;
    int redPass;
    int bluePass;
    double percentage;

    CvSize size;

    while(true)
    {
        clock_t start = clock();

        cap_img = cvQueryFrame(cv_cap);

        if(cap_img != 0)
        {
            int width = cap_img->width;
            int height = cap_img->height;
            size = cvSize(width, height);

			cvShowImage("video", cap_img);

			int value = getKeyPixels(cap_img);

			cout << value << endl;

            // Remove double-slashes for death.
            // /*
            //cvReleaseImage(&cRed);
            //cvReleaseImage(&cBlue); //*/
            //cvReleaseImage(&cRedThresh);
            //cvReleaseImage(&cBlueThresh);

        }

        clock_t end = clock();

        cout << "time: " << (end - start) << endl;

        key = cvWaitKey(1);

        if(key == 27)
            break;
    }

    cvReleaseCapture(&cv_cap);
    //cvDestroyWindow("video");
    cvDestroyWindow("operations");
	cvDestroyWindow("red");

    return 0;
}
#endif
