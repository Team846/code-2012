#pragma region includes/usings
#include <stdio.h>
#include <ctime>

#include <cv.h>
#include <ml.h>
#include <cxcore.h>
#include <highgui.h>

using namespace std;
#pragma endregion

#define DEBUGGING

int threshold = 100;

int loRPosition = 0;
int hiRPosition = 255;

int loBPosition = 0;
int hiBPosition = 255;

void onLoRThresholdSlide(int slideValue)
{
    loRPosition = slideValue;
}

void onHiRThresholdSlide(int slideValue)
{
    hiRPosition = slideValue;
}

void onLoBThresholdSlide(int slideValue)
{
    loBPosition = slideValue;
}

void onHiBThresholdSlide(int slideValue)
{
    hiBPosition = slideValue;
}


int getKey(IplImage * frameIn)
{
    uchar *ptr;

    int count=0;

    for(int y = 0; y < frameIn->height; y++)
    {
        for(int x = 0; x < frameIn->width; x++)
        {
            ptr = cvPtr2D(frameIn, y, x, NULL);

            int blue = ptr[0];
            int red = ptr[2];

            if(red >= loRPosition || blue >= loBPosition)
                count++;
        }
    }

    return count;
}

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


    cvNamedWindow("video", CV_WINDOW_AUTOSIZE);*/
    cvNamedWindow("red", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("blue", CV_WINDOW_AUTOSIZE);

    cvCreateTrackbar("Low", "red", &loRPosition, 255, onLoRThresholdSlide);
    cvCreateTrackbar("High", "red", &hiRPosition, 255, onHiRThresholdSlide);

    cvCreateTrackbar("Low", "blue", &loBPosition, 255, onLoBThresholdSlide);
    cvCreateTrackbar("High", "blue", &hiBPosition, 255, onHiBThresholdSlide);

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

			int value = getKey(cap_img);

			cout << value << endl;
            /*
            IplImage *cRed = cvCreateImage(size, IPL_DEPTH_8U, 1);
            IplImage *cBlue = cvCreateImage(size, IPL_DEPTH_8U, 1);
            IplImage *cRedThresh = cvCreateImage(size, IPL_DEPTH_8U, 1);
            IplImage *cBlueThresh = cvCreateImage(size, IPL_DEPTH_8U, 1);

            cvSplit(cap_img, cRed, NULL, cBlue, NULL);

            cvInRangeS(cRed, cvScalar(loRPosition, loRPosition, loRPosition), cvScalar(hiRPosition, hiRPosition, hiRPosition), cRedThresh);
            cvInRangeS(cBlue, cvScalar(loBPosition, loBPosition, loBPosition), cvScalar(hiBPosition, hiBPosition, hiBPosition), cBlueThresh);

            int white = getWhite(cap_img);

            double percent = ((double)white / (double)(width*height)) * 100.0;
            char buffer[32];

            sprintf(buffer, "%g", percent);

            cvRectangle(cap_img, cvPoint(0, 0), cvPoint(100, 16), CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
            cvPutText(cap_img, buffer, cvPoint(0, 10), &font, CV_RGB(0, 0, 255));
            //getGrayscale(cap_img, grayscale);
            cvShowImage("video", cap_img);
            //cvShowImage("averaged", &grayscale);

            cvShowImage("red", cRedThresh);
            cvShowImage("blue", cBlueThresh);*/

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
    cvDestroyWindow("red");
    cvDestroyWindow("blue");

    return 0;
}