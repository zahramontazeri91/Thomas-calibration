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
	Mat votes = Mat::zeros(270, R, CV_32FC(seg));
	Mat votes_fore = Mat::zeros(270, R, CV_32FC1);


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
					int deg = degree - 90;
					//here is the problem: degree range should be bound in [-90,180] but it will terminate if I change its interval;
					float rad = ((degree) / 180.0) * PI;
					int r = int(y * cos(rad) + x * sin(rad));

					if((rad>_3pi_4 && rad<_pi_4) || (rad>__pi_4 && rad<__3pi_4))
					{
						int ch=int(x/horizontal_subbin_width);
						votes.at<Vec3f>(degree,r)[ch] +=20;
						//in order to imshow the image effected of each channel, we add them all in a new Matrix.
						//if(ch==4)
						//here we can see image of one channel
						votes_fore.at<float>(degree,r) += votes.at<Vec3f>(degree,r)[ch];
					}
					else
					{
						int ch=int(y/vertical_subbin_width);
						votes.at<Vec3f>(degree,r)[ch] += 20;
						//if(ch==4)
						votes_fore.at<float>(degree,r) += votes.at<Vec3f>(degree,r)[ch];
					}
				}
			}
		}
	}

	//cast votes matrix to 8u so we can show it as an image
	//**** here is the next problem!****

	convertScaleAbs(votes_fore, spatialized_image, 1, 0);
	imshow( "spatialized hough transform",spatialized_image);

	waitKey(0);
	return votes;
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
	//spatialized_hough_image=spatialized_hough(edge,6);

	//initialing lines:
	Point pt1= Point(2,4);
	Point pt2= Point(200,400);
	line(imgage, Point pt1, Point pt2);

	//creating poses:
	int tilt, pan, f;
	float tilt_r, pan_r;
	Mat R = Mat::zeros(3,3,CV_32FC1);

	for(tilt=-1; tilt<1; tilt++)
		for(pan=-1; pan<1; pan++)
			for(f=0; f<1; f+=0.5)   //i am not sure for the interval of focal lenght!
			{
				tilt_r = ((tilt) / 180.0) * PI;
				pan_r = ((pan) / 180.0) * PI;

				Mat X = (Mat_<double>(3,3) << 1, 0 , 0, 0, cos(pan_r), -1*sin(pan_r), 0, sin(pan_r), cos(pan_r));
				Mat Y = (Mat_<double>(3,3) << cos(tilt_r), 0 , sin(tilt_r), 0, 1, 0, -1*sin(tilt_r), 0, cos(tilt_r));
				Mat Z = (Mat_<double>(3,3) << 1, 0 , 0, 0, 1, 0, 0, 0, 1);  //there is no rolling
					//cout << "C = " << endl << " " << X << endl << endl;
				R = X * Y;
				R = R * Z;
				Mat t = (Mat_<double>(3,1) << 0,0,0);
				Mat P = Mat::zeros(3,4,CV_32FC1);  //p is the projection matrix
				cv::hconcat(R, t, P);
				//point_iplane = focal_lenght * projection * point_3D
				Mat point_iplane = Mat::zeros(3,1,CV_32FC1);
				//here is the coordinate of the 3D point
				Mat point_3D = (Mat_<double>(4,1) << 1, 20, 0, 1);
				//int cx, cy; //I assumed cx=xy=0;
				Mat A = (Mat_<double>(3,3) << f, 0, 20, 0, f, 65, 0, 0, 1);
				point_iplane = A* P;
				point_iplane = point_iplane * point_3D;
				//normalizing corresponding point in image plane:
				double w=point_iplane.at<double>(1,1);
				cout<<point_iplane<<endl;
				cout<<point_iplane.at<float>(-1,0)<<"   "<<point_iplane.at<float>(1,1)<<"   "<<endl;
				Mat point_iplane_normalized = (Mat_<double>(2,1) << (point_iplane.at<double>(-1,0)/=w), (point_iplane.at<double>(0,0)/w));
				cout<<point_iplane_normalized<<endl<<endl;



			}


	return 0;
}
