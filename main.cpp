#include <iostream>
using namespace std;

#include "opencv2/opencv.hpp"
using namespace cv;

Mat src; Mat planes[3];

int main( int argc, char** argv ) {
	String imageName("../data/stuff.jpg"); // by default

	if (argc > 1){imageName = argv[1];}

	src = imread(imageName, IMREAD_COLOR); // Load an image

	if(src.empty())
		{cout << "Failed to read image file.\n"; return -1;}
	else
		{cout << "Image read successfully.\n";}

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
