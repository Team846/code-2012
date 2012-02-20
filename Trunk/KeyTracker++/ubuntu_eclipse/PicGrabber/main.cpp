#include <stdio.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

#include <opencv/cv.h>
#include <opencv/ml.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

using namespace std;

int main()
{
	CvCapture *pCapture = cvCaptureFromCAM(0);
	IplImage *pCaptureImg;

	cout << "enter an ID for the save file (snapshot-<id>.jpg)" << endl;

	int id = 0;

	cin >> id;

	pCaptureImg = cvQueryFrame(pCapture);

	char buf[32];

	sprintf(buf, "snapshot-%d", id);

	cvSaveImage(buf, pCaptureImg);
}
