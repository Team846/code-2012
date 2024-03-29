#include "global.h"

#include <stdio.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/opencv.hpp>

#include "cvFunction.h"

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

void setPresetThresholds() {
	thresholds[0] = 71;
	thresholds[1] = 19;
}

void loadThresholds() {
	if (access("KEY_thresholds.txt", F_OK) == -1) {
		printf("!!! KEY THRESHOLD FILE NOT FOUND - DEFAULTING TO PRESETS\n");
		setPresetThresholds();
		return; // use default values: Rmin: 158, Rmaxother: 135, Bmin: 80, Bmaxother: 90
	}

	string line = "";
	ifstream file;

	file.open("KEY_thresholds.txt");

	int linesread = 0;

	while (!file.eof() && linesread < 2) {
		getline(file, line);

		if (line[0] == '#')
			continue;

		thresholds[linesread] = atoi(line.c_str());

		linesread++;
	}

	if (linesread != 2) {
		printf("!!! INVALID KEY THRESHOLD FILE - DEFAULTING TO PRESETS\n");

		setPresetThresholds();
	}
}

int main(int argc, char** argv) {

#ifndef BEAGLEBOARD
	cout << "WARNING: Not Beagleboard" << endl;
	return 0;
#endif

	loadThresholds();
    
    int index = 0;
    if (argc > 1) {
	    index = atoi(argv[1]);
    }

	/* Open capture stream */
    cv::VideoCapture pCapture = cv::VideoCapture(index);

    pCapture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    pCapture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

    cv::Mat pCaptureImg;


	int frameNumber = 0;

	Messenger messenger;

    clock_t start, end;
    int avgFrameCount = 30;

	while (true) {
		/* Increment frame number */
		++frameNumber;

        if (frameNumber % avgFrameCount == 1) {
            start = clock();
        }

		pCapture >> pCaptureImg; // retrieve image frame

        float totalPixels = (float) (pCaptureImg.rows
                * pCaptureImg.cols / 4);

        int red = 0, blue = 0;

        processImage(pCaptureImg, thresholds[0], thresholds[1], &red, &blue);

        float matchedPixels_R = (float) red / totalPixels;
        float matchedPixels_B = (float) blue / totalPixels;

        int value_R = 255 * matchedPixels_R;
        int value_B = 255 * matchedPixels_B;
       
        char buf[100];
        sprintf(buf, "%.02f %.02f", matchedPixels_R, matchedPixels_B);
        DbgPrint(buf);

        // DbgPrint(value);

        int iSent = messenger.sendData(frameNumber, value_R, value_B);

        if (frameNumber % avgFrameCount == 0) {
            end = clock();
            double t = CLOCKS_PER_SEC * avgFrameCount * 1.0 / (end - start);
            cout << "FPS: " << t << endl;
        }
	}

	return 0;
}
