#include <iostream>
using namespace std;

#include "opencv2/opencv.hpp"
//#include "opencv2/imgproc.hpp"
//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/highgui.hpp"
using namespace cv;

Mat src, planes[3];



int main( int argc, char** argv )
{
	String imageName("../data/stuff.jpg"); // by default
	
	if (argc > 1){imageName = argv[1];}
	
	src = imread(imageName.c_str(), IMREAD_COLOR); // Load an image
	if(src.empty())
		{cout << "Failed to read image file.\n"; return -1;}

	namedWindow("Src", WINDOW_NORMAL); // Create a window to display results
	namedWindow("B", WINDOW_NORMAL); // Create a window to display results
	namedWindow("G", WINDOW_NORMAL); // Create a window to display results
	namedWindow("R", WINDOW_NORMAL); // Create a window to display results

	// BGR
	split(src, planes);

	imshow("Src", src);
	imshow("B", planes[0]);		
	imshow("G", planes[1]);
	imshow("R", planes[2]);

	waitKey();

	return 0;
}
