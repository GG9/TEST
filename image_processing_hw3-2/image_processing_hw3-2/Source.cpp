#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include <cmath>
#define  inputFile "lenna.png"
#define  outputFile "output.png"


//double filter[]={0.23037781,0.71484657,0.63088076,-0.02798376,-0.18703481,0.03084138,0.03288301,-0.01059740};
  double filter[]={1,1};
using namespace std;
using namespace cv;

void waveletA(Mat&,Mat&,int,int,int,int);
void waveletB(Mat&,Mat&,int,int,int,int);
void cl(Mat&,int,int,int,int);
int main()
{
	Mat image = imread(inputFile,CV_LOAD_IMAGE_GRAYSCALE);
	if(! image.data )                              
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    } 
	Mat image2= image.zeros(image.rows,image.cols,CV_8UC1);
	Mat image3= image.zeros(image.rows,image.cols,CV_8UC1);
	
	waveletA(image,image2,0,image.rows,0,image.cols);
	waveletB(image2,image3,0,image.rows,0,image.cols/2);
	waveletB(image2,image3,0,image.rows,image.cols/2,image.cols);

	Mat image4= image3.clone();
	cl(image4,0,image.rows/2,0,image.cols/2);
	Mat image5= image4.clone();

	waveletA(image3,image4,0,image.rows/2,0,image.cols/2);
	waveletB(image4,image5,0,image.rows/2,0,image.cols/4);
	waveletB(image4,image5,0,image.rows/2,image.cols/4,image.cols/2);





	namedWindow( "Display window", WINDOW_AUTOSIZE );
    imshow( "Display window", image );
	namedWindow( "Display window2", WINDOW_AUTOSIZE );
    imshow( "Display window2", image2);
	namedWindow( "Display window3", WINDOW_AUTOSIZE );
    imshow( "Display window3", image3);
	namedWindow( "Display window4", WINDOW_AUTOSIZE );
    imshow( "Display window4", image4);
	namedWindow( "Display window5", WINDOW_AUTOSIZE );
    imshow( "Display window5", image5);
	waitKey(0);                              
	destroyAllWindows();
	image.release();
	return 0;
}
void waveletA(Mat& inputImage,Mat& outputImage,int ROW1,int ROW2,int COL1,int COL2){ //  L | R
	
	int SOM=sizeof(filter)/sizeof(filter[0]);
	for(int i=ROW1;i<ROW2;i++)
	{
		for(int j=COL1;j<(COL2-SOM);j++)
		{
			j++;
			double temp=0;
			for(int k=0;k<SOM;k++)
				 temp +=  inputImage.at<uchar>(i,j+k) *  filter[k];
			//cout<<temp<<endl;
			outputImage.at<uchar>(i,COL1/2+j/2) =  saturate_cast<uchar>(temp/2);
		}
		for(int j=COL1;j<(COL2-SOM);j++)
		{
			j++;
			double temp=0;
			for(int k=0;k<SOM;k++)
				temp +=  inputImage.at<uchar>(i,j+k) * pow(-1,k) * filter[SOM-1-k];
			//cout<<temp<<endl;
			outputImage.at<uchar>(i,COL2/2+j/2) = 127 + temp;
		}
	}
}
void waveletB(Mat& inputImage,Mat& outputImage,int ROW1,int ROW2,int COL1,int COL2){   // U
																					  //  D	
	int SOM=sizeof(filter)/sizeof(filter[0]);
	for(int j=COL1;j<(COL2-SOM);j++)
	{
		for(int i=ROW1;i<(ROW2-SOM);i++)
		{
			i++;
			double temp=0;
			for(int k=0;k<SOM;k++)
				 temp +=  inputImage.at<uchar>(i+k,j) *  filter[k];
		
			outputImage.at<uchar>(ROW1/2+i/2,j) =  saturate_cast<uchar>(temp/2);
		}
		for(int i=ROW1;i<(ROW2-SOM);i++)
		{
			i++;
			double temp=0;
			for(int k=0;k<SOM;k++)
				temp +=  inputImage.at<uchar>(i+k,j) * pow(-1,k) * filter[SOM-1-k];
			
			outputImage.at<uchar>(ROW2/2+i/2,j) = 127 + temp;
		}
	}
}
void cl(Mat& inputImage,int ROW1,int ROW2,int COL1,int COL2){  //CLEAR LOCAL REGION
	for(int i=ROW1;i<ROW2;i++)
	{
		for(int j=COL1;j<COL2;j++)
		{
			inputImage.at<uchar>(i,j) = 0;
		}
	}
}