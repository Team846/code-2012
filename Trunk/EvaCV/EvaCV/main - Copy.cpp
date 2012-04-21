#include <opencv.hpp>
#include <stdio.h>

#include "windows.h" 

using namespace std;


int main()
{
    // Open the file.
	CvCapture * pCapture; //new OpenCV capture stream
	IplImage * pVideoFrame; //new OpenCV image
	
	pCapture = cvCaptureFromCAM(0); 
	//pCapture = cvCaptureFromFile("C:\\Users\\Name\\Documents\\Visual Studio 2010\\Projects\\EvaCV\\EvaCV\\exposure_on-4.png");
	
	pVideoFrame = cvLoadImage("C:\\Users\\Name\\Documents\\Visual Studio 2010\\Projects\\EvaCV\\EvaCV\\exposure_on-4.bmp");
	IplImage* grayscale = cvCreateImage( cvGetSize(pVideoFrame), 8, 1 );
	cvCvtColor(pVideoFrame, grayscale, CV_BGR2GRAY);
	IplImage* binary = cvCreateImage(cvGetSize(pVideoFrame), IPL_DEPTH_8U, 1);
	cvThreshold(grayscale, binary, 40, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cvNamedWindow( "video", CV_WINDOW_AUTOSIZE );
	cvShowImage("binary", binary);
	cvShowImage("video", pVideoFrame);
	int i;
	Sleep(5000);
	cin >> i;

	IplImage* dst = cvCreateImage( cvGetSize(pVideoFrame), 8, 1 );
    IplImage* color_dst = cvCreateImage( cvGetSize(pVideoFrame), 8, 3 );
	IplImage* harris_dst = cvCreateImage( cvGetSize(pVideoFrame), 8, 3 );

	
	
	int sw = 0;
	double rho = 1.0;
	double theta = CV_PI/180;
	double threshold = 100.0;
	double min = 30;
	int gap = 10;

	int rho_track = 10.0;
	cvCreateTrackbar("rho", "video", &rho_track, 100, NULL);
	
	int theta_track = 10.0;//0.01 is default
	cvCreateTrackbar("theta", "video", &theta_track, 100, NULL);
	
	int min_track = 30;
	cvCreateTrackbar("min", "video", &min_track, 100, NULL);

	int thresh_track = 100.0;
	cvCreateTrackbar("threshold", "video", &thresh_track, 200, NULL);
	
	cvCreateTrackbar("gap", "video", &gap, 100, NULL);
	int key = -1;
	while(key==-1)
	{
		

		rho = (rho_track + 1)/ 10.0;//from 0.1 to 10.0
		theta = (theta_track + 1)/ 1000.0;//10 / 1000  = 0.01
		min = min_track + 1;
		threshold = thresh_track + 1;
		//pVideoFrame = cvQueryFrame(pCapture);
		//pVideoFrame = cvLoadImage("C:\\Users\\Name\\Documents\\Visual Studio 2010\\Projects\\EvaCV\\EvaCV\\exposure_on-4.bmp");

		

#if 1

		cvCvtColor(pVideoFrame, grayscale, CV_BGR2GRAY);
		CvMemStorage* storage = cvCreateMemStorage(0);
        CvSeq* lines = 0;
        int i;
		cvShowImage( "video", grayscale);
		
		cvCanny( grayscale, dst, 50, 200, 3 );
		cvShowImage( "edges", dst);

		cvCornerHarris(dst, harris_dst, 1);
		cvShowImage("harris", harris_dst);

		cvCvtColor( dst, color_dst, CV_GRAY2BGR );
#if 0
        lines = cvHoughLines2( dst,
                               storage,
                               CV_HOUGH_STANDARD,
                               1*2, //rho
                               CV_PI/180 * 5, //theta
                               80, //threshold
                               0, //unused param
                               0 );
		cout << lines->total << endl;
        for( i = 0; i < MIN(lines->total,100); i++ )
        {
            float* line = (float*)cvGetSeqElem(lines,i);
            float rho = line[0];
            float theta = line[1];
            CvPoint pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0), 3, 8 );
        }
#else
        lines = cvHoughLines2( dst,
                               storage,
                               CV_HOUGH_PROBABILISTIC,
                               rho,
                               theta,
							   threshold,
                               30,
                               10 );
		cout << lines->total << endl;
        for( i = 0; i < lines->total; i++ )
        {
            CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
            cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 3, 8 );
        }
#endif

        cvNamedWindow( "Hough", 1 );
        cvShowImage( "Hough", color_dst );
#endif

		key = cvWaitKey(20);
		cvReleaseMemStorage(&storage);
		
	}

	cvReleaseImage( &pVideoFrame );
	cvReleaseCapture ( &pCapture );
	cvDestroyWindow( "video" );

        return 0;
}