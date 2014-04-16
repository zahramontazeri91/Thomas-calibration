#include <cv.h>
#include <highgui.h>
#include <iostream>

#define PI acos(-1)

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
	Mat image;
	image = imread("images.jpeg");
	Mat grey;
	cvtColor(image, grey, CV_RGB2GRAY);
	Mat edge;
	Sobel(grey, edge, CV_8U, 1, 0);
    
	int R = int(sqrt(double(edge.cols * edge.cols + edge.rows * edge.rows)));
	Mat votes = Mat::zeros( 270, R, CV_8UC3 );
	cv::Size s = edge.size();
	int l = s.height;
	int w = s.width;
    
   
    int degree;
    double value=1;
    
    for (int x = 0; x < l; x++) {
        for (int y = 0; y < w; y++) {
            if (edge.at<uchar>(x, y) > 200) {
                //int intensity = edge.at<int>(x, y);
                for(degree = 0; degree < 270; degree++){
                    // int deg = degree - 90;
                    double rad = (degree / 180.0) * PI;
                    int r = int(x * cos(rad) + y * sin(rad));
                    votes.at<uchar>(degree,r)= (0,0,value);
                    value=value+1;
                }
            }
        }
    }
    
    if( argc != 2 || !image.data )
    {
        printf( "No image data \n" );
        return -1;
    }
    
    namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
    imshow( "Display Image", votes );
    
    waitKey(0);
    return 0;
}
