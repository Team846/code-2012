#include <opencv.hpp>
#include <stdio.h>


using namespace std;


typedef struct targetInfo
{
	double x, y, d;
};

int main()
{
	//doodle();

    // Open the file.
	CvCapture * pCapture; //new OpenCV capture stream
	IplImage * pVideoFrame; //new OpenCV image
	
	pCapture = cvCaptureFromCAM(0); 
	//pCapture = cvCaptureFromFile("C:\\Users\\Name\\Documents\\Visual Studio 2010\\Projects\\EvaCV\\EvaCV\\exposure_on-4.png");
	
	//pVideoFrame = cvLoadImage("C:\\Users\\Name\\Documents\\Visual Studio 2010\\Projects\\EvaCV\\EvaCV\\exposure_on-4.bmp");
	pVideoFrame = cvLoadImage("C:\\Users\\Name\\Documents\\Visual Studio 2010\\Projects\\EvaCV\\EvaCV\\exposure_on-4 - Copy.png");
	IplImage* grayscale = cvCreateImage( cvGetSize(pVideoFrame), 8, 1 );
	
	IplImage* binary = cvCreateImage(cvGetSize(pVideoFrame), IPL_DEPTH_8U, 1);

	IplImage* dst = cvCreateImage( cvGetSize(pVideoFrame), 8, 1 );
    IplImage* color_dst = cvCreateImage( cvGetSize(pVideoFrame), 8, 3 );
	IplImage* harris_dst = cvCreateImage( cvGetSize(pVideoFrame), 8, 3 );

	cvNamedWindow("original");
	cvNamedWindow("binary");
	cvNamedWindow("final");
	
	int binaryThreshold = 40;
	cvCreateTrackbar("threshold (not used)", "binary", &binaryThreshold, 255, NULL);

	int areaThreshold = 115;
	cvCreateTrackbar("area", "final", &areaThreshold, 200, NULL);

	int cornerThreshold = 50;
	cvCreateTrackbar("corner", "final", &cornerThreshold, 500, NULL);

	int topThreshold = 200;
	cvCreateTrackbar("top", "final", &topThreshold, 500, NULL);

	CvMemStorage* blob_storage = cvCreateMemStorage( 0 );    // container of retrieved contours

	CvSeq* contours         = NULL;
	CvScalar black          = CV_RGB( 0, 0, 0 ); // black color
	CvScalar white          = CV_RGB( 255, 255, 255);   // white color
	CvScalar gray           = CV_RGB( 124, 124, 124);   // white color

	int key = -1;
	while(key==-1)
	{
		cvShowImage("original", pVideoFrame);
		
		//Convert to a binary image
		cvCvtColor(pVideoFrame, grayscale, CV_BGR2GRAY);
		cvThreshold(grayscale, binary, binaryThreshold, 255, CV_THRESH_BINARY | CV_THRESH_OTSU); //chooses best threshold automagically
		cvDilate(binary, binary);
		cvShowImage("binary", binary);

		/********************** Remove small blobs *********************************/
		
		
		double area;

		cvFindContours( binary, blob_storage, &contours, sizeof( CvContour ), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE );
		//contours = cvApproxPoly(contours, sizeof(CvContour), blob_storage, CV_POLY_APPROX_DP, 3, 1);
		vector<targetInfo> targets;
		while( contours )   // loop over all the contours
		{
			area = cvContourArea( contours, CV_WHOLE_SEQ );
			if( fabs( area ) <= areaThreshold)  // if the area of the contour is less than threshold remove it
			{
				// draws the contours into a new image
				cvDrawContours( binary, contours, black, black, -1, CV_FILLED, 8 ); // removes white dots
			}
	        else
			{
				cvDrawContours( binary, contours, white, white, -1, CV_FILLED, 8 ); // fills in holes
				CvSeq* hull;
				hull = cvConvexHull2( contours,0,  
                             CV_COUNTER_CLOCKWISE,  
                             0);
				
				CvPoint* points = new CvPoint[hull->total];
				CvPoint pt0 = **CV_GET_SEQ_ELEM( CvPoint*, hull, hull->total - 1 );
				//points[hull->total - 1] = pt0;
				for( int i = 0; i < hull->total; i++ )
				{
					CvPoint pt = **CV_GET_SEQ_ELEM( CvPoint*, hull, i );
					points[i] = pt;
				}
				//int * scores = icvFast9Score((unsigned char * )(binary->imageData), cvGetSize(pVideoFrame).width, points, hull->total, cornerThreshold);
				int bottomLeft = 0;
				int bottomRight = 0;
				int topLeft = 0;
				int topRight = 0;

				for( int i = 0; i < hull->total; i++ )
				{	
					if ( points[i].x + points[i].y > points[topRight].x + points[topRight].y)
						topRight = i;
					if ( points[i].x + points[i].y < points[bottomLeft].x + points[bottomLeft].y)
						bottomLeft = i;
					if (points[i].y - points[i].x > points[topLeft].y - points[topLeft].x)
						topLeft = i;
					if (points[i].y - points[i].x < points[bottomRight].y - points[bottomRight].x)
						bottomRight = i;
					//cout << scores[i] << endl;
					//if (scores[i] > 253)//detected as a corner
					{ //is a candidate point
						//cout << scores[i] << endl;
						//cvCircle(binary, points[i], 10, white);
					}
				}
				cvCircle(binary, points[bottomLeft], 10, white);
				cvCircle(binary, points[bottomRight], 10, white);
				cvCircle(binary, points[topLeft], 10, white);
				cvCircle(binary, points[topRight], 10, white);

				targetInfo result;
				result.x = points[bottomLeft].x + points[bottomRight].x + points[topLeft].x + points[topRight].x;
				result.x /= 4;

				result.y = points[bottomLeft].y + points[bottomRight].y + points[topLeft].y + points[topRight].y;
				result.y /= 4;

				//distance is sorta proportional to the real distance. It should be close enough.
				result.y = points[topLeft].y - points[bottomLeft].y + points[topRight].y - points[topLeft].y;

				targets.push_back(result);

				//cout << "Done" << endl;	
				cvFillConvexPoly(binary, points, hull->total, white);
				
				CvPoint a, b;
				a.x = result.x;
				a.y = points[topRight].y;
				b.x = result.x;
				b.y = points[bottomLeft].y;
				cvLine(binary, a, b, black, 1);
				
				//cvDrawContours( binary, contours, gray, gray , -1, CV_FILLED, 8 );
				
				//CvSeq* poly = 
				//cvFillConvexPoly(binary, poly->first, poly->total, gray);
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


		CvPoint a, b;
		a.x = cvGetSize(binary).width;
		a.y = topThreshold;
		b.x = 0;
		b.y = topThreshold;
		cvLine(binary, a, b, gray, 1);
		cvShowImage("final", binary);
		
		key = cvWaitKey(20);
		
	}

	cvReleaseImage( &pVideoFrame );
	cvReleaseCapture ( &pCapture );
	cvDestroyWindow( "video" );

        return 0;
}