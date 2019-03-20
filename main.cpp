#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <wiringPi.h>
#include <thread>

#define PRE_ALLOCATION_SIZE (100 * 1024 * 1024) /* 100MB - total amount of memory locked for this process */
#define MY_STACK_SIZE (10 * 1024 * 1024)        /* 10MB - stack size for this process */

#include <RTLib.h>

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
    show_new_pagefault_count("Initial count", ">=0", ">=0");

    configure_malloc_behavior();

    show_new_pagefault_count("mlockall() generated", ">=0", ">=0");

    reserve_process_memory(PRE_ALLOCATION_SIZE);

    show_new_pagefault_count("malloc() and touch generated",
                             ">=0", ">=0");

    /* Now allocate the memory for the 2nd time and prove the number of
	   pagefaults are zero */
    reserve_process_memory(PRE_ALLOCATION_SIZE);
    show_new_pagefault_count("2nd malloc() and use generated",
                             "0", "0");

    printf("\n\nLook at the output of ps -leyf, and see that the "
           "RSS is now about %d [MB]\n",
           PRE_ALLOCATION_SIZE / (1024 * 1024));

    start_rt_thread();

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
        // t1 = micros();
        cam.read(src);
        split(src, bgr);

        // imshow(window_name, src);
        // if (waitKey(10) == 27)
        // {
        // cout << "Esc key pressed, stopping feed.\n";
        // break;
        // }

        // t2 = micros();
        // cout << (t2 - t1) << "\n";
    }

    return 0;
}
