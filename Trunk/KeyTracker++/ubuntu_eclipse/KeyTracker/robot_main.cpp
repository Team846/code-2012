#include "global.h"

#if defined(__BEAGLEBOARD__)

#include <stdio.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

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

int thresholds[2];

int getKeyPixels(IplImage * frameIn, int &red_out, int &blue_out) {
	int count = 0;
	int red = 0, blue = 0;

	for (int y = 0; y < frameIn->height; y++) {
		for (int x = 0; x < frameIn->width; x++) {
			int blue = CV_GETPIXEL(frameIn, x, y, 0);
			int green = CV_GETPIXEL(frameIn, x, y, 1);
			int red = CV_GETPIXEL(frameIn, x, y, 2);

			bool redPass = (red - blue >= thresholds[0] && red - green >= thresholds[0]);
			bool bluePass = (blue - red >= thresholds[1] && blue - green >= thresholds[1]);

			if (redPass || bluePass) {
				++count;

				if(redPass)
					++red;
				if(bluePass)
					++blue;
			}
		}
	}

	red_out = red;
	blue_out = blue;

	return count;
}

void setPresetThresholds()
{
	thresholds[0] = 71;
	thresholds[1] = 19;
}

void loadThresholds()
{
	if(access("KEY_thresholds.txt", F_OK) == -1)
	{
		printf("!!! KEY THRESHOLD FILE NOT FOUND - DEFAULTING TO PRESETS\n");
		setPresetThresholds();
		return; // use default values: Rmin: 158, Rmaxother: 135, Bmin: 80, Bmaxother: 90
	}

	string line = "";
	ifstream file;

	file.open("KEY_thresholds.txt");

	int linesread = 0;

	while(!file.eof() && linesread < 2)
	{
		getline(file, line);

		if(line[0] == '#')
			continue;

		thresholds[linesread] = atoi(line.c_str());

		linesread++;
	}

	if(linesread != 2)
	{
		printf("!!! INVALID KEY THRESHOLD FILE - DEFAULTING TO PRESETS\n");

		setPresetThresholds();
	}
}

int main() {

	loadThresholds();

	/* Open capture stream */
	CvCapture *pCapture = cvCaptureFromCAM(1);
	IplImage *pCaptureImg;

	cvSetCaptureProperty(pCapture,  CV_CAP_PROP_FRAME_WIDTH, 320);
	cvSetCaptureProperty(pCapture,  CV_CAP_PROP_FRAME_HEIGHT, 240);

	int frameNumber = 0;

	Messenger messenger;

	while (true) {
		/* Increment frame number */
		++frameNumber;

		pCaptureImg = cvQueryFrame(pCapture);

		/* Verify that the image is valid */
		if (pCaptureImg != 0) {

			float totalPixels = (float)(pCaptureImg->width * pCaptureImg->height);

			int red=0,blue=0;

			int matched = getKeyPixels(pCaptureImg, red, blue);

			float matchedPixels = (float) matched / totalPixels,
					matchedPixels_R = (float)red / totalPixels,
					matchedPixels_B = (float)blue / totalPixels;

			int value = 255 * matchedPixels,
					value_R = 255 * matchedPixels_R,
					value_B = 255 * matchedPixels_B;

			// DbgPrint(value);

			int iSent = messenger.sendData(frameNumber, value_R, value_B);

			DbgPrint(matched);

		} else {

		}
	}

	return 0;
}
#endif
