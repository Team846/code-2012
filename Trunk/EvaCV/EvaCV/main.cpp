#include <opencv.hpp>
#include <stdio.h>
using namespace std;

#define CALC_POS(y, x, width) (y*width + x) 

/* get reference to pixel at (col,row),
for multi-channel images (col) should be multiplied by number of channels */
#undef CV_IMAGE_ELEM
#define CV_IMAGE_ELEM( image, color, row, col ) \
	(*((image)->imageData + (image)->widthStep*(row)+(col)*((image)->widthStep/(image)->width)+(color)))
	
#define BLUE_CHANNEL 0
#define GREEN_CHANNEL 1
#define RED_CHANNEL 2

void processImage(IplImage * colorImage, IplImage &processedImage, char * processedFrame)
{
	//processedImage = * colorImage;		
	processedImage.nSize = sizeof(IplImage);
	processedImage.ID = 0;		
	processedImage.nChannels = 1;
	//processedImage.alphaChannel = colorImage->alphaChannel;
	processedImage.depth = 8;
	//memcpy(processedImage.colorModel, colorImage->colorModel, 4);
	//memcpy(processedImage.channelSeq, colorImage->channelSeq, 4);
	processedImage.dataOrder = 1;
	processedImage.origin = 0;
	//processedImage.align = colorImage->align;
	processedImage.width = colorImage->width;
	processedImage.height = colorImage->height;
	processedImage.roi = NULL;
	processedImage.maskROI = NULL;
	//processedImage.imageId = colorImage->imageId;
	//processedImage.tileInfo = colorImage->tileInfo;
	processedImage.imageSize = colorImage->height*colorImage->width;
	processedImage.imageData = processedFrame;
	processedImage.widthStep = colorImage->width;
	//memcpy(processedImage.BorderMode, colorImage->BorderMode, 4*sizeof(int));
	//memcpy(processedImage.BorderConst, colorImage->BorderConst, 4*sizeof(int));
	processedImage.imageDataOrigin = processedFrame;
}

void getGrayPlane(IplImage * colorImage, IplImage &processedImage)
{
	char * processedFrame;
	processedFrame = new char[colorImage->width * colorImage->height];
	for (int i = 0; i < colorImage->width; i++)
	{
		for (int j = 0; j < colorImage->height; j++)
		{
			processedFrame[CALC_POS(j, i, colorImage->width)] = 
				(11L*(int)CV_IMAGE_ELEM(colorImage, BLUE_CHANNEL, j, i) + 
				60L*(int)CV_IMAGE_ELEM(colorImage, GREEN_CHANNEL, j, i) + 
				29L*(int)CV_IMAGE_ELEM(colorImage, RED_CHANNEL, j, i))/100;
			//cout << i << " " << j << "set\n";
		}
	}
	processImage(colorImage, processedImage, processedFrame);
}

void getIntensity()
{
}

void getRedPlane(IplImage * colorImage, IplImage &processedImage)
{
	char * processedFrame;
	processedFrame = new char[colorImage->width * colorImage->height];
	for (int i = 0; i < colorImage->width; i++)
	{
		for (int j = 0; j < colorImage->height; j++)
		{
			processedFrame[CALC_POS(j, i, colorImage->width)] = 
				CV_IMAGE_ELEM(colorImage, RED_CHANNEL, j, i);
			//cout << i << " " << j << "set\n";
		}
	}
	processImage(colorImage, processedImage, processedFrame);
}

void getGreenPlane(IplImage * colorImage, IplImage &processedImage)
{
	char * processedFrame;
	processedFrame = new char[colorImage->width * colorImage->height];
	for (int i = 0; i < colorImage->width; i++)
	{
		for (int j = 0; j < colorImage->height; j++)
		{
			processedFrame[CALC_POS(j, i, colorImage->width)] = 
				CV_IMAGE_ELEM(colorImage, GREEN_CHANNEL, j, i);
			//cout << i << " " << j << "set\n";
		}
	}
	processImage(colorImage, processedImage, processedFrame);
}

void getBluePlane(IplImage * colorImage, IplImage &processedImage)
{
	char * processedFrame;
	processedFrame = new char[colorImage->width * colorImage->height];
	for (int i = 0; i < colorImage->width; i++)
	{
		for (int j = 0; j < colorImage->height; j++)
		{
			processedFrame[CALC_POS(j, i, colorImage->width)] = 
				CV_IMAGE_ELEM(colorImage, BLUE_CHANNEL, j, i);
			//cout << i << " " << j << "set\n";
		}
	}
	processImage(colorImage, processedImage, processedFrame);
}

int main()
{
    // Open the file.
	CvCapture * pCapture; //new OpenCV capture stream
	IplImage * pVideoFrame; //new OpenCV image
	
	pCapture = cvCaptureFromCAM(0); 

	cvNamedWindow( "video", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "grey", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "red" , CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "green" , CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "blue" , CV_WINDOW_AUTOSIZE );

	int key = -1;
	while(key==-1)
	{
		pVideoFrame = cvQueryFrame(pCapture);

		//bgr - .6green+.29red+.11blue
		IplImage grayScaleImage;
		IplImage redPlaneImage;
		IplImage greenPlaneImage;
		IplImage bluePlaneImage;

		getGrayPlane(pVideoFrame, grayScaleImage);
		getRedPlane(pVideoFrame, redPlaneImage);
		getGreenPlane(pVideoFrame, greenPlaneImage);
		getBluePlane(pVideoFrame, bluePlaneImage);
		
		cvShowImage( "video", pVideoFrame);
		cvShowImage( "grey", &grayScaleImage);
		cvShowImage( "red" , &redPlaneImage );
		cvShowImage( "green" , &greenPlaneImage );
		cvShowImage( "blue" , &bluePlaneImage );

		key = cvWaitKey(20);
	}

	cvReleaseImage( &pVideoFrame );
	cvReleaseCapture ( &pCapture );
	cvDestroyWindow( "video" );

	/*
        IplImage *img = cvLoadImage("photo.jpg");
        if (!img) 
		{
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