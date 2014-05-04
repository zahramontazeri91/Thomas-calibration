#include <cv.h>
#include <highgui.h>
#include <iostream>

#define PI acos(-1)

using namespace cv;
using namespace std;

Mat hough_transform (Mat edge)
{
	//input of the function: a binary image
	//output: hough matrix

	//calcute R_MAX:
	int R = int(sqrt(double(edge.cols * edge.cols + edge.rows * edge.rows)));
	Mat hough_image = Mat::zeros( 270, R , CV_8UC1 );
	Mat votes = Mat::zeros(270, R, CV_32FC1 );

	cv::Size s = edge.size();	//size of original image
	int l = s.height; //edge.rows
	int w = s.width;	//edge.cols

	int degree;
	for (int y = 0; y < w; y++) {
		for (int x = 0; x < l; x++) {
			if (edge.at<char>(x, y) > 10) {
				for(degree = 0; degree < 270; degree++){
					//here is the problem: degree range should be bound in [-90,180] but it will terminate if I change its interval;
					float rad = ((degree) / 180.0) * PI;
					int r = int(x * cos(rad) + y * sin(rad));
					votes.at<float>(degree,r) += 20;
				}
			}
		}
	}
	//cast votes matrix to 8u to show it as an image
	convertScaleAbs(votes, hough_image, 1, 0);
	imshow( "hough transform", hough_image);

	waitKey(0);
	return hough_image;
}

Mat spatialized_hough (Mat edge, int seg)
{
	//input of the function: a binary image and a integer number showing number of segmentations
	//output: spatialized hough matrix

	//some constants
	float _3pi_4 = -1*3*PI/4;
	float _pi_4 = -1*PI/ 4;
	float __3pi_4 = 3*PI/ 4;
	float __pi_4 = PI/ 4;
	//calcute R_MAX:
	int R = int(sqrt(double(edge.cols * edge.cols + edge.rows * edge.rows)));

	Mat spatialized_image = Mat::zeros( 270, R , CV_8UC(seg) );
	Mat votes = Mat::zeros(270, R, CV_32FC(seg) );


	cv::Size s = edge.size();	//size of original image
	int l = s.height; //edge.rows
	int w = s.width;	//edge.cols

	int horizontal_subbin_width = int((edge.cols - 1)/seg) + 1;
	int vertical_subbin_width = int((edge.rows - 1)/seg) + 1;

	int degree;
	for (int y = 0; y < w; y++) {
		for (int x = 0; x < l; x++) {
			if (edge.at<char>(x, y) > 10) {
				for(degree = 0; degree < 270; degree++){
					//here is the problem: degree range should be bound in [-90,180] but it will terminate if I change its interval;
					float rad = ((degree) / 180.0) * PI;
					int r = int(x * cos(rad) + y * sin(rad));

					if((rad>_3pi_4 && rad<_pi_4) || (rad>__pi_4 && rad<__3pi_4))
					{
						int ch=int(x/horizontal_subbin_width);
						votes.at<Vec3f>(degree,r)[ch] +=20;
					}
					else{
						int ch=int(y/vertical_subbin_width);
						votes.at<Vec3f>(degree,r)[ch] +=20;
					}
				}
			}
		}
	}

	//cast votes matrix to 8u to show it as an image
	convertScaleAbs(votes, spatialized_image, 1, 0);
	imshow( "spatialized hough transform",spatialized_image);

	waitKey(0);
	return spatialized_image;
}

int main()
{
	Mat image;
	image = imread("index.jpeg");
	Mat grey;
	cvtColor(image, grey, CV_RGB2GRAY);
	Mat edge;
	Sobel(grey, edge, CV_8U, 1, 0);
	Mat hough_image;
	//hough_image=hough_transform(edge);

	Mat spatialized_hough_image;
	spatialized_hough_image=spatialized_hough(edge,2);
	int i=1;

	Mat L = Mat::zeros( 2,3 , CV_8UC(i) );
	L.at<Vec3b>(1,1)[1]=300;
	cout<<L.at<Vec3f>(1,1)[1]<<endl;
	//	Mat M(1, 3, CV_32SC(i));
	//	M.at<Vec3d>(0,0)[1]=44;
	//	cout <<M<<endl<< M.at<Vec3d>(0,0)[1] << endl;  // This works


	return 0;
}
