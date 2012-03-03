#include "global.h"

//#pragma region includes/usings
#include <stdio.h>
#include <ctime>

#include <opencv2/opencv.hpp>

#include "cvFunction.h"

#include "headers/messenger.h"

using namespace std;

#define DEBUGGING
//#undef DEBUGGING

#ifdef DEBUGGING
#define DbgPrint(a) cout << a << endl
#else
#define DbgPrint(a)
#endif

int main(int argc, char **argv) {
    int index = 0;
    if (argc > 1) {
        index = atoi(argv[1]);
    }

    cv::VideoCapture cap = cv::VideoCapture(index);
    cv::Mat cap_img;

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

    cv::namedWindow("video", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("operations", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("red", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("blue", CV_WINDOW_AUTOSIZE);

    int redThreshold = 0;
    int blueThreshold = 0;

    cv::createTrackbar("Red Threshold", "operations", &redThreshold, 255);
    cv::createTrackbar("Blue Threshold", "operations", &blueThreshold, 255);

	int frameNumber = 0;

	Messenger messenger;

    clock_t start, end;

	while (true) {
		++frameNumber;
        if (frameNumber % 30 == 1) {
            start = clock();
        }
        cap >> cap_img; // retrieve image frame
        float totalPixels = (float) (cap_img.rows * cap_img.cols) / 4;

        int redPixels, bluePixels;
        processImage(cap_img, redThreshold, blueThreshold, &redPixels, &bluePixels);
        float matchedPixels_R = redPixels * 1.0 / totalPixels;
        float matchedPixels_B = bluePixels * 1.0 / totalPixels;

        cv::imshow("video", cap_img);

        //cout << "r: " << matchedPixels_R << " b: " << matchedPixels_B << endl;

        if (frameNumber % 30 == 0) {
            end = clock();
            double t = CLOCKS_PER_SEC * 30 / (end - start * 1.0);
            cout << "FPS: " << t << endl;
        }

		if (cv::waitKey(1) == 27)
		    break;
	}

	return 0;
}
