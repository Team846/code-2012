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

/*
 * TODO: <- a big special thanks to brent for fixing "to-do", a crucial part of the code.
 * (comment placed at request of brent)
 *
 * HARD-CODE! FIX!
 *
 * IT'S BURNING MY EYES
 * */

int thresholds[4];

int getKeyPixels(IplImage * frameIn) {
	int count = 0;

	for (int y = 0; y < frameIn->height; y++) {
		for (int x = 0; x < frameIn->width; x++) {
			int blue = CV_GETPIXEL(frameIn, x, y, 0);
			int red = CV_GETPIXEL(frameIn, x, y, 2);

			if ((red >= thresholds[0] && blue <= thresholds[1])
					|| (blue >= thresholds[2] && red <= thresholds[3])) {
				++count;
			}
		}
	}

	return count;
}

void setPresetThresholds()
{
	thresholds[0] = 158;
	thresholds[1] = 135;

	thresholds[2] = 80;
	thresholds[3] = 90;
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

	while(!file.eof() && linesread < 4)
	{
		getline(file, line);

		if(line[0] == '#')
			continue;

		thresholds[linesread] = atoi(line.c_str());

		linesread++;
	}

	if(linesread != 4)
	{
		printf("!!! INVALID KEY THRESHOLD FILE - DEFAULTING TO PRESETS\n");

		setPresetThresholds();
	}
}

int main() {

	loadThresholds();

	/* Open capture stream */
	CvCapture *pCapture = cvCaptureFromCAM(0);
	IplImage *pCaptureImg;

	int frameNumber = 0;

	Messenger messenger;

	while (true) {
		/* Increment frame number */
		++frameNumber;

		pCaptureImg = cvQueryFrame(pCapture);

		/* Verify that the image is valid */
		if (pCaptureImg != 0) {
			int matched = getKeyPixels(pCaptureImg);
			float matchedPixels = (float) matched
					/ (float) (pCaptureImg->width * pCaptureImg->height);

			int value = 255 * matchedPixels;

			// DbgPrint(value);

			int iSent = messenger.sendData(frameNumber, value);

			DbgPrint(value);

		} else {

		}
	}

	return 0;
}
#endif
