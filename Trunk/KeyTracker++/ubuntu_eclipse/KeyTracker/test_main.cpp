#include "global.h"

//#pragma region includes/usings
#include <stdio.h>
#include <ctime>

#include <opencv/cv.h>
#include <opencv/ml.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

#include "cvFunction.h"

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

int threshold = 30;

int loRPosition = 0;
int hiRPosition = 255;

int loBPosition = 0;
int hiBPosition = 255;

void mouseHandler(int event, int x, int y, int flags, void* param) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		uchar* ptr;

		IplImage *img0 = (IplImage*) param;
		IplImage *img1 = cvCloneImage(img0);

		/* read pixel *
		 ptr = cvPtr2D(img1, y, x, NULL);

		 /*
		 * display the BGR value
		 * */

		//cout << (int) ptr[0] << ", " << (int) ptr[1] << ", " << (int) ptr[2]
		//<< endl;
	}
}

int getKeyPixels(IplImage * frameIn, int &red_out, int &blue_out) {
	IplImage * redFrameOut = cvCreateImage(
			cvSize(frameIn->width, frameIn->height), IPL_DEPTH_8U, 3);
	IplImage * blueFrameOut = cvCreateImage(
			cvSize(frameIn->width, frameIn->height), IPL_DEPTH_8U, 3);

	int count = 0;

	for (int y = 0; y < frameIn->height; y++) {
		for (int x = 0; x < frameIn->width; x++) {
			int blue = CV_GETPIXEL(frameIn, x, y, 0);
			int red = CV_GETPIXEL(frameIn, x, y, 2);
			int green = CV_GETPIXEL(frameIn, x, y, 1);

			bool alreadyset = false;

			//int rPixel = CV_GETPIXEL(frameIn, x, y, 0);

			//rPixel = red;
			cvSet2D(redFrameOut, y, x, cvScalar(red, red, red));
			cvSet2D(blueFrameOut, y, x, cvScalar(blue, blue, blue));

			if (/*red >= loRPosition && */red - blue >= hiRPosition && red
					- green >= hiRPosition) {
				cvSet2D(redFrameOut, y, x, cvScalar(0, 0, 255));
				++count;
				++red_out;
				alreadyset = true;
			}
			if (/*blue >= loBPosition && */blue - red >= hiBPosition && blue
					- green >= hiBPosition) {
				cvSet2D(blueFrameOut, y, x, cvScalar(255, 0, 0));

				++blue_out;

				if (!alreadyset)
				++count;
			}
		}
	}

	cvShowImage("red", redFrameOut);
	cvShowImage("blue", blueFrameOut);

	return count;
}

void onLoRThresholdSlide(int slideValue) {
	loRPosition = slideValue;
}
void onHiRThresholdSlide(int slideValue) {
	hiRPosition = slideValue;
}
void onLoBThresholdSlide(int slideValue) {
	loBPosition = slideValue;
}
void onHiBThresholdSlide(int slideValue) {
	hiBPosition = slideValue;
}

int main() {
	IplImage * cap_img;
	CvCapture * cv_cap = cvCaptureFromCAM(0);
    
    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_WIDTH, 320);
    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_HEIGHT, 240);

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

	cvCreateTrackbar("RedMin", "operations", &loRPosition, 255,
			onLoRThresholdSlide);
	cvCreateTrackbar("RedMax", "operations", &hiRPosition, 255,
			onHiRThresholdSlide);
	cvCreateTrackbar("BlueMin", "operations", &loBPosition, 255,
			onLoBThresholdSlide);
	cvCreateTrackbar("BlueMax", "operations", &hiBPosition, 255,
			onHiBThresholdSlide);

	int key;
	int frameNumber = 0;

	Messenger messenger;

	CvSize size;
    
    clock_t start, end;

	while (true) {
		++frameNumber;
        if (frameNumber % 30 == 1) {
            start = clock();
        }
		cap_img = cvQueryFrame(cv_cap);

		if (cap_img != NULL) {
			float totalPixels = (float) (cap_img->width * cap_img->height);

			//int red = 0, blue = 0;

			//int matched = getKeyPixels(cap_img, red, blue);
			//float matchedPixels = (float) matched / totalPixels,
			//matchedPixels_R = (float) red / totalPixels,
			//matchedPixels_B = (float) blue / totalPixels;
           
            int redPixels, bluePixels;
            processImage(cap_img, loRPosition, loBPosition, &redPixels, &bluePixels);
            float matchedPixels_R = redPixels * 1.0 / totalPixels;
            float matchedPixels_B = bluePixels * 1.0 / totalPixels;

			cvSetMouseCallback("video", mouseHandler, (void*) cap_img);

			cvShowImage("video", cap_img);

            //cout << "r: " << matchedPixels_R << " b: " << matchedPixels_B << endl;

			//int value = 255 * matchedPixels, value_r = 255 * matchedPixels_R,
			//value_b = 255 * matchedPixels_R;

			// DbgPrint(value);

			//int iSent = messenger.sendData(frameNumber, value_r, value_b);

			//DbgPrint(matched << ";" << red << ";" << blue);

			// Remove double-slashes for death.
			// /*
			//cvReleaseImage(&cRed);
			//cvReleaseImage(&cBlue); // */
			//cvReleaseImage(&cRedThresh);
			//cvReleaseImage(&cBlueThresh);

		}

        if (frameNumber % 30 == 0) {
            end = clock();
            double t = CLOCKS_PER_SEC * 30 / (end - start * 1.0);
            cout << "FPS: " << t << endl;
        }

		key = cvWaitKey(1);

		if (key == 27)
		break;
	}

	cvReleaseCapture(&cv_cap);
	cvDestroyWindow("video");
	cvDestroyWindow("operations");
	cvDestroyWindow("red");
	cvDestroyWindow("blue");

	return 0;
}
