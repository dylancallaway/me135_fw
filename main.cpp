#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
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
    // cpu_set_t my_set;                                 /* Define your cpu_set bit mask. */
    // CPU_ZERO(&my_set);                                /* Initialize it all to 0, i.e. no CPUs selected. */
    // CPU_SET(0, &my_set);                              /* set the bit that represents core 0. */
    // sched_setaffinity(0, sizeof(cpu_set_t), &my_set); /* Set affinity of this process to the defined mask, i.e. only 0. */

    // struct sched_param sp;
    // sp.sched_priority = 99;
    // pid_t pid = getpid();
    // sched_setscheduler(pid, SCHED_RR, &sp);

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
        split(src, bgr);

        // imshow(window_name, src);
        // if (waitKey(10) == 27)
        // {
        // cout << "Esc key pressed, stopping feed.\n";
        // break;
        // }

        t2 = clock();
        cout << (t2 - t1) << "\n";
        delayMicroseconds(10);
    }

    return 0;
}
