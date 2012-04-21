#include <opencv.hpp>
#include <stdio.h>


using namespace std;


typedef struct targetInfo
{
	double x, y;
	double height, width;
};

#define DEBUG 1

int main()
{
	CvCapture * pCapture; //new OpenCV capture stream
	IplImage * pVideoFrame; //new OpenCV image
	
	pCapture = cvCaptureFromCAM(0); 
	
#if DEBUG
	pVideoFrame = cvLoadImage("C:\\Users\\Name\\Documents\\Visual Studio 2010\\Projects\\EvaCV\\EvaCV\\exposure_on-4 - Copy.png");
#else
	pVideoFrame = cvQueryFrame(pCapture);
#endif
	IplImage* grayscale = cvCreateImage( cvGetSize(pVideoFrame), 8, 1 );
	IplImage* binary = cvCreateImage(cvGetSize(pVideoFrame), IPL_DEPTH_8U, 1);


	int binaryThreshold = 40;
	int areaThreshold = 115;
	int cornerThreshold = 50;
	int topThreshold = 200;

#if DEBUG
	cvNamedWindow("original");
	cvNamedWindow("binary");
	cvNamedWindow("final");

	cvCreateTrackbar("threshold (not used)", "binary", &binaryThreshold, 255, NULL);
	cvCreateTrackbar("area", "final", &areaThreshold, 200, NULL);
	cvCreateTrackbar("corner", "final", &cornerThreshold, 500, NULL);
	cvCreateTrackbar("top", "final", &topThreshold, 500, NULL);
#endif

	CvMemStorage* storage = cvCreateMemStorage( 0 );  

	CvSeq* contours         = NULL;
	CvScalar black          = CV_RGB( 0, 0, 0 ); 
	CvScalar white          = CV_RGB( 255, 255, 255);   
	CvScalar gray           = CV_RGB( 124, 124, 124);   

	int key = -1;
	while(key==-1)
	{

#if DEBUG
		cvShowImage("original", pVideoFrame);
#else
		pVideoFrame = cvQueryFrame(pCapture);
#endif
		
		//Convert to a binary image
		cvCvtColor(pVideoFrame, grayscale, CV_BGR2GRAY);
		cvThreshold(grayscale, binary, binaryThreshold, 255, CV_THRESH_BINARY | CV_THRESH_OTSU); //chooses best threshold automagically
		cvDilate(binary, binary);
#if DEBUG
		cvShowImage("binary", binary);
#endif
		
		
		double area;

		//Find all the blobs
		cvFindContours( binary, storage, &contours, sizeof( CvContour ), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );
		
		vector<targetInfo> targets;
		while( contours )   // loop over all the blobs
		{
			area = cvContourArea( contours, CV_WHOLE_SEQ );
#if DEBUG
			if( fabs( area ) <= areaThreshold)  // if the area of the contour is less than threshold, count it as noise
			{
				// draws the contours into a new image
				cvDrawContours( binary, contours, black, black, -1, CV_FILLED, 8 ); // removes white dots, on image, only needed for debug
			}
	        else
#endif
			{
#if DEBUG
				cvDrawContours( binary, contours, white, white, -1, CV_FILLED, 8 ); // fills in holes
#endif				
				CvSeq* hull;
				hull = cvConvexHull2( contours,0,  
                             CV_COUNTER_CLOCKWISE,  
                             0); //find a convex hull around this blob
				
				CvPoint* points = new CvPoint[hull->total]; //this is going to store all the points that define the convex hull
				
				for( int i = 0; i < hull->total; i++ )
				{
					CvPoint pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i );
					points[i] = pt;
				}
				
				int bottomLeft = 0;
				int bottomRight = 0;
				int topLeft = 0;
				int topRight = 0;

				//iterate through the hull points, and find the ones that are the best corners for the target
				for( int i = 1; i < hull->total; i++ )
				{	
					if ( points[i].x + points[i].y > points[topRight].x + points[topRight].y)
						topRight = i;
					if ( points[i].x + points[i].y < points[bottomLeft].x + points[bottomLeft].y)
						bottomLeft = i;
					if (points[i].y - points[i].x > points[topLeft].y - points[topLeft].x)
						topLeft = i;
					if (points[i].y - points[i].x < points[bottomRight].y - points[bottomRight].x)
						bottomRight = i;
				}
#if DEBUG
				cvCircle(binary, points[bottomLeft], 10, white);
				cvCircle(binary, points[bottomRight], 10, white);
				cvCircle(binary, points[topLeft], 10, white);
				cvCircle(binary, points[topRight], 10, white);
#endif

				targetInfo result;
				result.x = points[bottomLeft].x + points[bottomRight].x + points[topLeft].x + points[topRight].x;
				result.x /= 4;

				result.y = points[bottomLeft].y + points[bottomRight].y + points[topLeft].y + points[topRight].y;
				result.y /= 4;

				//distance is sorta proportional to the real distance. It should be close enough.
				result.height = points[topLeft].y - points[bottomLeft].y + points[topRight].y - points[topLeft].y;
				result.width  = points[topRight].x - points[topLeft].x + points[bottomRight].x - points[bottomLeft].x;

				targets.push_back(result);

#if DEBUG
				cvFillConvexPoly(binary, points, hull->total, white);
		
				CvPoint a, b;
				a.x = result.x;
				a.y = points[topRight].y;
				b.x = result.x;
				b.y = points[bottomLeft].y;
				cvLine(binary, a, b, black, 1);
#endif
			}
			contours = contours->h_next;    // jump to the next contour
		}

		//process the targets vector
		int maxIndex = 0;
		for (int i = 0; i < targets.size(); i++)
		{
			if (targets[i].y > targets[maxIndex].y)
				maxIndex = i;
		}
		if (targets[maxIndex].y > topThreshold)//Or less than distance threshold if we need close up shooting
		{
			//Be happy! And send this back to the cRio of course. 
		}

#if DEBUG
		CvPoint a, b;
		a.x = cvGetSize(binary).width;
		a.y = topThreshold;
		b.x = 0;
		b.y = topThreshold;
		cvLine(binary, a, b, gray, 1);
		cvShowImage("final", binary);
#endif		
		key = cvWaitKey(20);
		
	}

	cvReleaseImage( &pVideoFrame );
	cvReleaseCapture ( &pCapture );
#if DEBUG
	cvDestroyWindow( "video" );
#endif
    return 0;
}