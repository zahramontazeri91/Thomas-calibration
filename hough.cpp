#include <cv.h>
#include <highgui.h>
#include <iostream>

#define PI acos(-1)

using namespace cv;
using namespace std;

Mat hough_transform (Mat image)
{

	Mat grey;
	cvtColor(image, grey, CV_RGB2GRAY);
	Mat edge;
	Sobel(grey, edge, CV_8U, 1, 0);
	//calcute R_MAX:
	int R = int(sqrt(double(edge.cols * edge.cols + edge.rows * edge.rows)));
	Mat hough_image = Mat::zeros( 270, R , CV_8UC1 );
	Mat votes = Mat::zeros(270, R, CV_32FC1 );

	cv::Size s = edge.size();	//size of original image
	int l = s.height;
	int w = s.width;

    int degree;
    float value=0;
    for (int y = 0; y < l; y++) {
        for (int x = 0; x < w; x++) {
            if (edge.at<char>(x, y) > 10) {
                for(degree = 0; degree < 270; degree++){
                	//here is the problem: degree range should be bound [-90,180]
                    float rad = ((degree) / 180.0) * PI;
                    int r = int(x * cos(rad) + y * sin(rad));
                    value = votes.at<float>(degree,r) += 20;
                    votes.at<float>(degree,r)= value;
                }
            }
        }
    }

    convertScaleAbs(votes, hough_image, 1, 0);
    imshow( "hough transform", hough_image);

waitKey(0);
return hough_image;
}

int main()
{
	Mat image;
	image = imread("index.jpeg");
	Mat img;
	img=hough_transform(image);
	return 0;
}
