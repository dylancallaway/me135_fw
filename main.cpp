#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <wiringPi.h>

#include <sched.h>
#include <sys/mman.h>

#include <RTLib.h>

#define PRE_ALLOCATION_SIZE (100 * 1024 * 1024) /* 100MB pagefault free buffer */
#define MY_STACK_SIZE (100 * 1024)              /* 100 kB is enough for now. */

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

    /*---------------------------------------------------*/

    Mat src, planes[3];

    wiringPiSetup();

    VideoCapture cam(0);

    if (!cam.isOpened())
    {
        cout << "Unable to open camera.\n";
        cin.get();
        return -1;
    }

    double frm_width = 640;
    double frm_height = 480;
    cam.set(CAP_PROP_FRAME_WIDTH, frm_width);
    cam.set(CAP_PROP_FRAME_HEIGHT, frm_height);

    cout << "Frame resolution: " << frm_width << "x" << frm_height << "\n";

    double frm_rate = 90;
    cam.set(CAP_PROP_FPS, frm_rate);
    cout << "Frame rate: " << frm_rate << "\n";

    string window_name = "Camera Feed";
    namedWindow(window_name, WINDOW_NORMAL);

    clock_t t1, t2, delta_t;

    while (true)
    {
        t1 = clock();
        delta_t = t1 - t2;
        t2 = clock();

        Mat src;
        bool read_success = cam.read(src);

        if (!read_success)
        {
            cout << "Unable to read frame.\n";
            cin.get();
            break;
        }

        // imshow(window_name, src);

        if (waitKey(10) == 27)
        {
            cout << "Esc key pressed, stopping feed.\n";
            break;
        }
        cout << float(delta_t) / CLOCKS_PER_SEC << "\n";
        delayMicroseconds(50);
    }

    return 0;
}
