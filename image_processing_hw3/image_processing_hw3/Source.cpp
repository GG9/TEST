#include  <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include <cmath>
#define  inputFile "f10.png"
#define  maskFile  "f10_roi.png"


using namespace std;
using namespace cv;

void MASK(Mat& ,Mat& );
void CalH(Mat&);
void drawH();
double MEAN( );
double VARI(int);
double SKEW(int);
double VARI_normalize(double);
double UNIF();
double ENTR();
void cooccurrenceM(Mat& ,int*);
void drawCM(int* );
void calCMD(int*,int);
double p[256]={0};
double P[256]={0};

int main()
{
	Mat image = imread(inputFile,CV_LOAD_IMAGE_GRAYSCALE);
	if(! image.data )                              
    {
        cout <<  "Could not open or find the image" <<endl ;
        return -1;
    } 
	namedWindow( "Display window", WINDOW_AUTOSIZE );
    imshow( "Display window", image );
	waitKey(0);

	Mat imageMASK = imread(maskFile,CV_LOAD_IMAGE_GRAYSCALE);
	if(! image.data )                              
    {
        cout <<  "Could not open or find the imageROI" <<endl ;
        return -1;
    } 
	MASK(image,imageMASK);
	imageMASK.release();
	namedWindow( "Display window2", WINDOW_AUTOSIZE );
    imshow( "Display window2", image );
	waitKey(0);



	


	memset(p,0,sizeof(p));
	memset(P,0,sizeof(P));
	CalH(image);


	double m=MEAN();
	double V=VARI(m);
	cout<<"=============="<<inputFile<<"====================="<<endl;
	cout<<"mean :"<<m<<endl;
	cout<<"varience :"<<V<<"  R(normalize):"<<VARI_normalize(V)<<endl;
	cout<<"skewness :"<<SKEW(m)<<endl;
	cout<<"uniformity :"<<UNIF()<<endl;
	cout<<"entropy :"<<ENTR()<<endl;

	

	int Matrix[256][256];
	memset(Matrix,0,sizeof(Matrix));
	cooccurrenceM(image,&Matrix[0][0]);

	destroyAllWindows();
	image.release();
	
	return 0;
}
void MASK(Mat& inputImage,Mat& maskImage){
	int ROW=inputImage.rows ;
	int COL=inputImage.cols ;
	for(int i=0;i<ROW;i++)
	{
		for(int j=0;j<COL;j++)
		{
			if(maskImage.at<uchar>(i,j)==0)
				inputImage.at<uchar>(i,j) = 0;
		}
	}

}
void CalH(Mat& inputImage){
	int ROW=inputImage.rows ;
	int COL=inputImage.cols ;
	int TOTAL=0 ;
	for(int i=0;i<ROW;i++)
	{
		for(int j=0;j<COL;j++)
		{
			if(inputImage.at<uchar>(i,j)!=0)
			{
				P[inputImage.at<uchar>(i,j)]++;
				TOTAL++;
			}
		}
	}
	for(int i=0;i<256 ;i++)
		p[i] = P[i] / TOTAL ;
	drawH();
}
void drawH(){
	Mat histImage(400, 512, CV_8UC3, Scalar(0, 0, 0));

	int P_max=0 ;
	double P_normal[256];
	memset(P_normal,0,sizeof(P_normal));
	for(int i=0;i<256;i++)
	{
		if(P[i] > P_max)
			P_max = P[i];
	}
	for(int i=0;i<256;i++)
	{
		P_normal[i] = P[i]/P_max;
	}
		
	for (int i = 0; i < 256; i++)
	{
		line(histImage, Point((2*i), (int)(400*( 1- P_normal[i]))),Point(2*i ,400),Scalar(255, 255, 255), 2, 8, 0);
	}
	namedWindow( "Display HISTOGRAM", WINDOW_AUTOSIZE );
    imshow( "Display HISTOGRAM", histImage );
	waitKey(0);
}
double MEAN(){
	double m=0;
	for(int i=0;i<256;i++)
		m +=  p[i]*i;
	return m;
}
double VARI(int m){
	double V=0;
	for(int i=0;i<256;i++)
		V += (i-m)*(i-m)*p[i];
	return V;
}
double SKEW(int m){
	double S=0;
	for(int i=0;i<256;i++)
		S += p[i]*(i-m)*(i-m)*(i-m);
	return S;
}
double VARI_normalize(double S){
	S=S/(255*255);
	return S/(1+S);
}
double UNIF(){
	double U=0;
	for(int i=0;i<256;i++)
		U += p[i]*p[i];
	return U;
}
double ENTR(){

	double e=0;
	for(int i=0;i<256;i++)
		if(p[i]>0)
			e += p[i]*log(p[i]);
	return -e;
}
void cooccurrenceM(Mat& inputImage,int* M){
	int ROW=inputImage.rows ;
	int COL=inputImage.cols ;
	int TOTAL=0;

	for(int i=0;i<ROW-1 ;i++)
	{
		for(int j=0;j<COL-1 ;j++)
		{
			if(inputImage.at<uchar>(i,j)!=0 && inputImage.at<uchar>(i,j+1)!=0)
			{
				*(M+inputImage.at<uchar>(i,j)*255+inputImage.at<uchar>(i,j+1))+=1;
				TOTAL++;
			}

		}
	}
	
	drawCM(M);
	calCMD( M, TOTAL);
}
void drawCM(int* M)
{
	Mat CM(512, 512, CV_8UC1, Scalar(0, 0, 0));
	for(int i=0;i<256;i++)
	{
		for(int j=0;j<256;j++)
		{
			int value = *(M+i*255+j);
			CM.at<uchar>(2*i,2*j) = value;
			CM.at<uchar>(2*i+1,2*j) = value;
			CM.at<uchar>(2*i,2*j+1) = value;
			CM.at<uchar>(2*i+1,2*j+1) = value;
		}
	}
	namedWindow( "Display co-occurrence", WINDOW_AUTOSIZE );
    imshow( "Display co-occurrence", CM );
	waitKey(0);
	
}
void calCMD(int* M,int total){
	cout<<"     co-occurrence Matrix  "<<endl;
	double p_max=0;
	double U=0,e=0,C=0;
	for(int i=0;i<256;i++)
	{
		for(int j=0;j<256;j++)
		{
			double value = *(M+255*i+j) ;
			if(value>0)
			{
				U += value * value /total /total;
				e += value/total *(log(value/total));
				C += (i-j)*(i-j)*value /total; 
			}
			if(value >p_max)
				p_max = value;
		}
	}
	cout<<"maximum probability :"<<p_max/ total<<endl;
	cout<<"uniformity :"<< U <<endl;
	cout<<"entropy :"<< -e <<endl;
	cout<<"contrast :"<< C <<endl;
}


