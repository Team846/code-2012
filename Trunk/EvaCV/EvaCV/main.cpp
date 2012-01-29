#include <opencv.hpp>
#include <stdio.h>
using namespace std;

#define CALC_POS(y, x, width) (y*width + x) 


int main()
{

        // Open the file.
	CvCapture * pCapture; //new OpenCV capture stream
	IplImage * pVideoFrame; //new OpenCV image

	char *grayScale;
	
	pCapture = cvCaptureFromCAM(0); 

	cvNamedWindow( "video", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "grey", CV_WINDOW_AUTOSIZE );

	int key = -1;
	while(key==-1)
	{
		pVideoFrame = cvQueryFrame(pCapture);
		
		grayScale = new char[pVideoFrame->width * pVideoFrame->height];
		
		//bgr - .6green+.29red+.11blue
		for (int i = 0; i < pVideoFrame->width; i++)
		{
			for (int j = 0; j < pVideoFrame->height; j++)
			{
				
				//grayScale[CALC_POS(j, i, pVideoFrame->width)];
				//cout << "get\n";
				grayScale[CALC_POS(j, i, pVideoFrame->width)] = 
					0.11*CV_IMAGE_ELEM(pVideoFrame, char, j, i*3+0); + 
					0.6*CV_IMAGE_ELEM(pVideoFrame, char, j, i*3+1) + 
					0.29*CV_IMAGE_ELEM(pVideoFrame, char, j, i*3+2);
				//cout << i << " " << j << "set\n";
			}
		}

		IplImage grayScaleImage; 
		grayScaleImage.nSize = sizeof(IplImage);
		grayScaleImage.ID = 0;		
		grayScaleImage.nChannels = 1;
		grayScaleImage.depth = 8;
		grayScaleImage.dataOrder = 1;
		grayScaleImage.origin = 0;
		grayScaleImage.width = pVideoFrame->width;
		grayScaleImage.height = pVideoFrame->height;
		grayScaleImage.widthStep = pVideoFrame->width;
		grayScaleImage.imageDataOrigin = grayScale;
		grayScaleImage.imageData = grayScale;

		cvShowImage( "video", pVideoFrame);
		cvShowImage( "grey", &grayScaleImage);

		key = cvWaitKey(20);
	}

	cvReleaseImage( &pVideoFrame );
	cvReleaseCapture ( &pCapture );
	cvDestroyWindow( "video" );


	/*
        IplImage *img = cvLoadImage("photo.jpg");
        if (!img) {
                printf("Error: Couldn't open the image file.\n");
                return 1;
        }

        // Display the image.
        cvNamedWindow("Image:", CV_WINDOW_AUTOSIZE);
        cvShowImage("Image:", img);

        // Wait for the user to press a key in the GUI window.
        cvWaitKey(0);

        // Free the resources.
        cvDestroyWindow("Image:");
        cvReleaseImage(&img);*/
        
        return 0;
}