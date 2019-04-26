#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <wiringPi.h>
#include <wiringSerial.h>
#include <thread>
#include <sys/mman.h> // Needed for mlockall()
#include <unistd.h>   // needed for sysconf(int name);
#include <malloc.h>
#include <sys/time.h>     // needed for getrusage
#include <sys/resource.h> // needed for getrusage

/***************Camera and frame capture configuration******************/
#define FRM_COLS 320
#define FRM_ROWS 240
#define FRM_RATE 90

// Initialize image matrices
Mat src(FRM_ROWS, FRM_COLS, CV_8UC3, Scalar(0, 0, 0)); // 8 bit, 3 channel
Mat thresh(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));    // 8 bit, 1 channel
vector<vector<Point>> contours;                        // Vector of integer vectors for contours
Mat b(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));         // 8 bit, 1 channel
Mat g(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));         // 8 bit, 1 channel
Mat r(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0));         // 8 bit, 1 channel
Mat bgr[3] = {b, g, r};

VideoCapture cam(0); // Camera object
/* ****************************************************************/

/* *********************************Memory configuration***********************/
#define PRE_ALLOCATION_SIZE 10 * 1000 * 1000 /* Size of pagefault free buffer in bytes */
/* *************************************************************************/

/* **************************Image processing configuration***************************/
Mat homography_matrix(3, 3, CV_8UC1, Scalar(0)); // 3 x 3, 8 bit, 1 channel
/* *****************************************************************************/

/* ********************Function prototypes************************************/
void setMaxPriority(pid_t pid); // Sets thread FIFO priority high
// Shows pagefaults generated when accessing memory space
void showNewPageFaultCount(const char *logtext, const char *allowed_maj, const char *allowed_min);
void reserveProcessMemory(int size); // "Touches" memory space of size
/* ***********************************************************************/

/************** MAIN FUNCTION ***************/
int main()
{
    /************* MEMORY CONFIGURATION ****************/
    printf("\nMemory configuration:\n");

    pid_t primary_pid = getpid();
    setMaxPriority(primary_pid);

    /* Now lock all current and future pages from preventing of being paged */
    if (mlockall(MCL_CURRENT | MCL_FUTURE))
        perror("mlockall failed");

    /* Turn off malloc trimming.*/
    mallopt(M_TRIM_THRESHOLD, -1);

    /* Turn off mmap usage. */
    mallopt(M_MMAP_MAX, 0);

    showNewPageFaultCount("mlockall() generated", ">=0", ">=0");

    reserveProcessMemory(PRE_ALLOCATION_SIZE);

    showNewPageFaultCount("malloc() and touch generated",
                          ">=0", ">=0");

    /* Now allocate the memory for the 2nd time and prove the number of pagefaults is zero */
    reserveProcessMemory(PRE_ALLOCATION_SIZE);
    showNewPageFaultCount("2nd malloc() and use generated",
                          "0", "0");

    printf("Look at the output of ps -leyf, and see that the "
           "RSS is now about %d [kB]\n",
           PRE_ALLOCATION_SIZE / (1000));
    /*******************************************************/

    /***************** CPU CONFIGURATION ********************/
    printf("\nCPU Configuration:\n");
    cpu_set_t mask;
    CPU_ZERO(&mask);   // Set CPU core usage of program to zero
    CPU_SET(0, &mask); // Set usage on core 0
    CPU_SET(1, &mask); // Set usage on core 1
    CPU_SET(2, &mask); // Set usage on core 2
    CPU_SET(3, &mask); // Set usage on core 3
    cout << "CPU affinity set to: ";
    for (int i = 0; i < 4; i++)
    {
        cout << CPU_ISSET(i, &mask) << " "; // Verify CPU usage
    }
    cout << "\n";
    sched_setaffinity(primary_pid, sizeof(mask), &mask); // Update CPU core usage

    printf("\nCamera configration:\n");
    cam.set(CAP_PROP_FRAME_WIDTH, FRM_COLS);  // Set frame width
    cam.set(CAP_PROP_FRAME_HEIGHT, FRM_ROWS); // Set frame height
    printf("Frame Resolution: %d x %d\n", FRM_COLS, FRM_ROWS);

    cam.set(CAP_PROP_FPS, FRM_RATE); // Set nominal frame rate.
    printf("Camera nominal frame rate: %d\n", FRM_RATE);
    /*******************************************************/

    /************** PERSPECTIVE CORRECTION SETUP ********************/
    vector<Point2f> table_corners(4); // Actual table corners (trapezoidal)
    // Point2f table_corners_pixels[4] = {Point2f(189, 37), Point2f(361, 37), Point2f(424, 299), Point2f(121, 299)};
    Point2f table_corners_pixels[4] = {Point2f(88, 16), Point2f(174, 16), Point2f(204, 147), Point2f(54, 147)};

    vector<Point2f> desired_corners(4); // Desried table corners (rectangular)
    // Point2f desired_corners_pixels[4] = {Point2f(200, 100), Point2f(400, 100), Point2f(400, 400), Point2f(200, 400)};
    Point2f desired_corners_pixels[4] = {Point2f(100, 50), Point2f(200, 50), Point2f(200, 200), Point2f(100, 200)};

    // Set table_corners and desried_corners value to those in *pixels
    // Written this way for neatness...
    for (int i = 0; i < 4; i++)
    {
        table_corners[i] = table_corners_pixels[i];
        desired_corners[i] = desired_corners_pixels[i];
        cout << "Table Corner: " << table_corners[i] << "\tDesired Corner: " << desired_corners[i] << "\n";
    }

    homography_matrix = findHomography(table_corners, desired_corners); // Generate perspective transformation matrix
    cout << "Generated Homography Matrix:\n"
         << homography_matrix << "\n\n";
    /*******************************************************/

    /*************** THRESHOLDING AND CROPPING SETUP ****************/
    Scalar lowerb = Scalar(0, 0, 50);              // Lower bound for thresholding
    Scalar upperb = Scalar(40, 70, 150);           // Upper bound for thresholding
    Rect roi_1 = Rect(28, 14, 257, 206);           // Initial crop
    Rect roi_2 = Rect(77, 14, 225 - 77, 235 - 14); // Crop after perspective correction
    /*******************************************************/

    /****************** IMAGE DISPLAY SETUP ******************/
#define DISP_IMGS 1
#if DISP_IMGS == 1
    namedWindow("SRC", WINDOW_NORMAL);    // src image window
    namedWindow("THRESH", WINDOW_NORMAL); // thresh image window
#endif
    /*******************************************************/

    /********** UART SETUP **************/
    int fd;
    if ((fd = serialOpen("/dev/ttyS0", 115200)) < 0) // 115200 baud, ttyS0 is mini-uart
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
        return 1;
    }
    if (wiringPiSetup() == -1) // Required for using UART with wiringPi library
    {
        fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
        return 1;
    }

    /*************** MAIN LOOP ****************/
    printf("\nProgram started...\n");
    auto t0 = chrono::steady_clock::now(); // Initialize timer

    while (true)
    {
        cam.read(src);
        src = src(roi_1);
        warpPerspective(src, src, homography_matrix, Size(230, 250));
        src = src(roi_2);

        // split(src, bgr);
        // for (int i = 0; i < 3; i++)
        // {
        //     normalize(bgr[i], bgr[i], 1, 0, NORM_L2);
        // }
        // merge(bgr, 3, src);

        // multiply(src, Scalar(0.5, 0.5, 1), src);
        // normalize(src, src, 0, 255, NORM_MINMAX); // $$$

        inRange(src, lowerb, upperb, thresh);
        medianBlur(thresh, thresh, 5); // $$

        // blur(thresh, thresh, Size(5, 5));
        // inRange(thresh, Scalar(100), Scalar(255), thresh);

        // morphologyEx(thresh, thresh, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(5, 5)));

        findContours(thresh, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
        Point puck_center;
        for (uint8_t i = 0; i < contours.size(); i++)
        {
            Rect rect = boundingRect(contours[i]);
            double peri = arcLength(contours[i], 1);

            // cout << rect << "\t" << peri << "\n";

            if (rect.width >= 11 && rect.width <= 19 && rect.height >= 6 && rect.height <= 14 && peri >= 30 && peri <= 45)
            {
                puck_center = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);

                static Scalar color = Scalar(0, 255, 0);
                circle(src, puck_center, 2, color, -1, 8, 0);

                // Initialize prediction variables
                float x0, y0, x1, y1;
                float v_x, v_y;

                // Current point x1, y1
                x1 = puck_center.x, y1 = puck_center.y;

                /************** VELOCITY CALCULATION **************/
                auto t1 = chrono::steady_clock::now();     // Update current time
                chrono::duration<float> t_delta = t1 - t0; // Update t_delta

                v_x = (x1 - x0) / t_delta.count(), v_y = (y1 - y0) / t_delta.count();

                t0 = t1; // Update past time
                printf("Time between captures: %.3fms.\n", 1000 * t_delta.count());
                /*************************************************/

                float x_min = 10, y_min = 5;
                float x_max = 136, y_max = 210;
                float x_pred[5] = {x1}, y_pred[5] = {y1};
                float delta_t_pred[5] = {0};

                for (int j = 1; j < 5; j++)
                {
                    //How long it takes to hit boundary in each direction
                    float delta_t_x = max((x_min - x_pred[j - 1]) / v_x, (x_max - x_pred[j - 1]) / v_x);
                    float delta_t_y = max((y_min - y_pred[j - 1]) / v_y, (y_max - y_pred[j - 1]) / v_y);

                    if (delta_t_x < delta_t_y) //Hits x first
                    {
                        x_pred[j] = x_pred[j - 1] + (int16_t)(v_x * delta_t_x);
                        y_pred[j] = y_pred[j - 1] + (int16_t)(v_y * delta_t_x);
                        delta_t_pred[j] = delta_t_pred[j - 1] + delta_t_x;
                        v_x *= -1;
                    }
                    else //Hits y first
                    {
                        x_pred[j] = x_pred[j - 1] + (int16_t)(v_x * delta_t_y);
                        y_pred[j] = y_pred[j - 1] + (int16_t)(v_y * delta_t_y);
                        delta_t_pred[j] = delta_t_pred[j - 1] + delta_t_y;
                        v_y *= -1;
                    }
                }

                // if (abs(v_x) <= 50 && abs(v_y) <= 50)
                // {
                for (int j = 1; j < 5; j++)
                {
                    line(src, Point(x_pred[j - 1], y_pred[j - 1]), Point(x_pred[j], y_pred[j]), Scalar(255, 255, 0), 2, LINE_8);
                }

                // Update past point
                x0 = x1, y0 = y1;

                // printf("X: %d\tY: %d\n", puck_center.x, puck_center.y);
                int16_t coord[2] = {(int16_t)puck_center.x, (int16_t)puck_center.y};
                write(fd, &coord, 4);

                break;
            }
        }

#if DISP_IMGS == 1
        imshow("SRC", src);
        imshow("THRESH", thresh);

        if (waitKey(10) == 27)
        {
            printf("Esc key pressed, stopping feed.\n");
            break;
        }
#endif
    }
    /************* END MAIN LOOP ****************/
    return 0;
}
/*********** END MAIN FUNCTION *****************/

void setMaxPriority(pid_t pid)
{
    struct sched_param sp;
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    sched_setscheduler(pid, SCHED_FIFO, &sp);
    printf("PID: %d \t Priority: %d\n", pid, sp.sched_priority);
}

int last_majflt = 0, last_minflt = 0;
void showNewPageFaultCount(const char *logtext, const char *allowed_maj, const char *allowed_min)
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    printf("%-30.30s: Pagefaults, Major:%ld (Allowed %s), "
           "Minor:%ld (Allowed %s)\n",
           logtext,
           usage.ru_majflt - last_majflt, allowed_maj,
           usage.ru_minflt - last_minflt, allowed_min);

    last_majflt = usage.ru_majflt;
    last_minflt = usage.ru_minflt;
}

void reserveProcessMemory(int size)
{
    int i;
    char *buffer;
    buffer = (char *)malloc(size);

    /* Touch each page in this piece of memory to get it mapped into RAM */
    for (i = 0; i < size; i += sysconf(_SC_PAGESIZE))
    {
        /* Each write to this buffer will generate a pagefault.
		   Once the pagefault is handled a page will be locked in
		   memory and never given back to the system. */
        buffer[i] = 0;
    }

    /* buffer will now be released. As Glibc is configured such that it 
	   never gives back memory to the kernel, the memory allocated above is
	   locked for this process. All malloc() and new() calls come from
	   the memory pool reserved and locked above. Issuing free() and
	   delete() does NOT make this locking undone. So, with this locking
	   mechanism we can build C++ applications that will never run into
	   a major/minor pagefault, even with swapping enabled. */
    free(buffer);
}
