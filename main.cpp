#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <wiringPi.h>
#include <thread>
#include <chrono>
#include <sys/mman.h>

#include <RTLib.h>

#define PRE_ALLOCATION_SIZE (100 * 1024 * 1024) /* 100MB pagefault free buffer */

#define FRM_COLS 640
#define FRM_ROWS 480
#define FRM_RATE 90
#define CAP_INTERVAL 12

Mat src(FRM_ROWS, FRM_COLS, CV_8UC3, Scalar(0, 0, 0));
Mat b(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));
Mat g(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));
Mat r(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));
Mat bgr[3] = {b, g, r};

VideoCapture cam(0);

int main()
{
    struct sched_param sp;
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO) - 1;
    pid_t pid = getpid();
    sched_setscheduler(pid, SCHED_FIFO, &sp);
    cout << pid << "\t" << sp.sched_priority << "\n";

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

    wiringPiSetup();

    // cam.set(CAP_PROP_FRAME_WIDTH, FRM_COLS);
    // cam.set(CAP_PROP_FRAME_HEIGHT, FRM_ROWS);

    // cout << "Frame resolution: " << FRM_COLS << "x" << FRM_ROWS << "\n";

    // cam.set(CAP_PROP_FPS, FRM_RATE);
    // cout << "Frame rate: " << FRM_RATE << "\n";

    // // string window_name = "Camera Feed";
    // // namedWindow(window_name, WINDOW_NORMAL);

    // auto t2 = chrono::steady_clock::now();

    // mlockall(MCL_CURRENT | MCL_FUTURE);

    // while (true)
    // {
    //     auto next_time = chrono::steady_clock::now() + chrono::milliseconds(CAP_INTERVAL);
    //     auto t1 = chrono::steady_clock::now();
    //     chrono::duration<float, milli> t_elapse = t1 - t2;
    //     cout << t_elapse.count() << "\n";
    //     t2 = chrono::steady_clock::now();

    //     cam.read(src);
    //     split(src, bgr);

    //     // imshow(window_name, src);
    //     // if (waitKey(10) == 27)
    //     // {
    //     // cout << "Esc key pressed, stopping feed.\n";
    //     // break;
    //     // }

    //     this_thread::sleep_until(next_time);
    // }

    return 0;
}
