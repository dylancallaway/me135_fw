#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <wiringPi.h>

#include <sched.h>
#include <sys/mman.h>

#define FRM_COLS 640
#define FRM_ROWS 480

VideoCapture init_camera(void)
{
    VideoCapture cam(0);

    if (!cam.isOpened())
    {
        cout << "Unable to open camera.\n";
        cin.get();
        return -1;
    }

    double frm_width = FRM_COLS;
    double frm_height = FRM_ROWS;
    cam.set(CAP_PROP_FRAME_WIDTH, frm_width);
    cam.set(CAP_PROP_FRAME_HEIGHT, frm_height);

    cout << "Frame resolution: " << frm_width << "x" << frm_height << "\n";

    double frm_rate = 90;
    cam.set(CAP_PROP_FPS, frm_rate);
    cout << "Frame rate: " << frm_rate << "\n";

    return cam;
}

int main(int argc, char **argv)
{
    wiringPiSetup();

    cam = init_camera();

    // string window_name = "Camera Feed";
    // namedWindow(window_name, WINDOW_NORMAL);

    Mat src(FRM_ROWS, FRM_COLS, CV_8UC3, Scalar(0, 0, 0));
    Mat b(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));
    Mat g(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));
    Mat r(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));
    Mat bgr[3] = {b, g, r};

    clock_t t1, t2, delta_t;

    while (true)
    {
        t1 = clock();
        delta_t = t1 - t2;
        t2 = clock();

        bool read_success = cam.read(src);

        if (!read_success)
        {
            cout << "Unable to read frame.\n";
            cin.get(); // Wait for keypress
            break;
        }

        split(src, bgr);

        // imshow(window_name, src);

        // if (waitKey(10) == 27)
        // {
        // cout << "Esc key pressed, stopping feed.\n";
        // break;
        // }

        cout << delta_t << "\n"; // in clock ticks

        // delayMicroseconds(10); // Delay allows background processes to run at a fixed interval
    }

    return 0;
}
