#include "global.h"

#if defined(__BEAGLEBOARD__)

#include <stdio.h>
#include <ctime>

#include <opencv/cv.h>
#include <opencv/ml.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include "headers/messenger.h"

using namespace std;

#define CV_GETPIXEL(img, x, y, c) ((uchar*)(img->imageData + img->widthStep*y))[x*3+c]

#define DEBUGGING
//#undef DEBUGGING

#ifdef DEBUGGING


#define DbgPrint(a) cout << a << endl
#else
#define DbgPrint(a)
#endif

/*
 * TODO: <- a big special thanks to brent for fixing "to-do", a crucial part of the code.
 * (comment placed at request of brent)
 *
 * HARD-CODE! FIX!
 *
 * IT'S BURNING MY EYES
 * */
int redLowThresh = 158;
int redMaxOtherThresh = 135;

int bluLowThresh = 80;
int bluMaxOtherThresh = 80;

int getKeyPixels(IplImage * frameIn)
{
    int count=0;

    for(int y = 0; y < frameIn->height; y++)
    {
        for(int x = 0; x < frameIn->width; x++)
        {
            int blue = CV_GETPIXEL(frameIn, x, y, 0);
            int red = CV_GETPIXEL(frameIn, x, y, 2);

            if((red >= redLowThresh && blue <= redMaxOtherThresh) ||
            	(blue >= bluLowThresh && red <= bluMaxOtherThresh))
            {
                ++count;
            }
        }
    }

    return count;
}

int main()
{
	/* Open capture stream */
	CvCapture *pCapture = cvCaptureFromCAM(0);
	IplImage *pCaptureImg;

	int frameNumber = 0;

	Messenger messenger;

	while(true)
	{
		/* Increment frame number */
		++frameNumber;

		pCaptureImg = cvQueryFrame(pCapture);

		/* Verify that the image is valid */
		if(pCaptureImg != 0)
		{
			int matched = getKeyPixels(pCaptureImg);
			float matchedPixels = (float)matched / (float)(pCaptureImg->width * pCaptureImg->height);

			int value = 255 * matchedPixels;

			// DbgPrint(value);

			/*
			 * TODO:
			 *
			 * Implement packet sending
			 */
			int iSent = messenger.sendData(frameNumber, value);

			DbgPrint(iSent);
		}
		else
		{

		}
	}

	return 0;
}
#endif
