#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <wiringPi.h>
#include <thread>

#define FRM_COLS 640
#define FRM_ROWS 480

Mat src(FRM_ROWS, FRM_COLS, CV_8UC3, Scalar(0, 0, 0));
Mat b(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));
Mat g(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));
Mat r(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));
Mat bgr[3] = {b, g, r};

VideoCapture cam(0);

int t1, t2, t_elapse;

int main(int argc, char **argv)
{
    wiringPiSetup();

    double frm_width = FRM_COLS;
    double frm_height = FRM_ROWS;
    cam.set(CAP_PROP_FRAME_WIDTH, frm_width);
    cam.set(CAP_PROP_FRAME_HEIGHT, frm_height);

    cout << "Frame resolution: " << frm_width << "x" << frm_height << "\n";

    double frm_rate = 90;
    cam.set(CAP_PROP_FPS, frm_rate);
    cout << "Frame rate: " << frm_rate << "\n";

    // string window_name = "Camera Feed";
    // namedWindow(window_name, WINDOW_NORMAL);

    while (true)
    {
        t1 = clock();
        cam.read(src);
        // split(src, bgr);

        // imshow(window_name, src);
        // if (waitKey(10) == 27)
        // {
        // cout << "Esc key pressed, stopping feed.\n";
        // break;
        // }

        t2 = clock();
        cout << (t2 - t1) << "\n";
    }

    return 0;
}
