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
					0.11*CV_IMAGE_ELEM(pVideoFrame, BLUE_CHANNEL, j, i) + 
					0.6*CV_IMAGE_ELEM(pVideoFrame, GREEN_CHANNEL, j, i) + 
					0.29*CV_IMAGE_ELEM(pVideoFrame, RED_CHANNEL, j, i);
				//cout << i << " " << j << "set\n";
			}
		}

		IplImage grayScaleImage; 
		//grayScaleImage = * pVideoFrame;		
		grayScaleImage.nSize = sizeof(IplImage);
		grayScaleImage.ID = 0;		
		grayScaleImage.nChannels = 1;
		//grayScaleImage.alphaChannel = pVideoFrame->alphaChannel;
		grayScaleImage.depth = 8;
		//memcpy(grayScaleImage.colorModel, pVideoFrame->colorModel, 4);
		//memcpy(grayScaleImage.channelSeq, pVideoFrame->channelSeq, 4);
		grayScaleImage.dataOrder = 1;
		grayScaleImage.origin = 0;
		//grayScaleImage.align = pVideoFrame->align;
		grayScaleImage.width = pVideoFrame->width;
		grayScaleImage.height = pVideoFrame->height;
		grayScaleImage.roi = NULL;
		grayScaleImage.maskROI = NULL;
		//grayScaleImage.imageId = pVideoFrame->imageId;
		//grayScaleImage.tileInfo = pVideoFrame->tileInfo;
		grayScaleImage.imageSize = pVideoFrame->height*pVideoFrame->width;
		grayScaleImage.imageData = grayScale;
		grayScaleImage.widthStep = pVideoFrame->width;
		//memcpy(grayScaleImage.BorderMode, pVideoFrame->BorderMode, 4*sizeof(int));
		//memcpy(grayScaleImage.BorderConst, pVideoFrame->BorderConst, 4*sizeof(int));
		grayScaleImage.imageDataOrigin = grayScale;
		
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