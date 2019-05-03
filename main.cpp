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
    Scalar upperb = Scalar(30, 70, 150);           // Upper bound for thresholding
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

    /*************** BEHAVIOR *****************/
    short int difficulty = 0; //[0, 1, 2] = [Easy, Medium, Hard]
    short int state = 0; //[0, 1, 2, 3, 4] = [Wait, Catch, Bank Left, Straight, Bank Right]
    float v_x_0, v_y_0;

    /*************** MAIN LOOP ****************/
    printf("\nProgram started...\n");
    auto t0 = chrono::steady_clock::now(); // Initialize timer

    while (true)
    {
        cam.read(src);
        src = src(roi_1);
        warpPerspective(src, src, homography_matrix, Size(230, 250));
        src = src(roi_2);

        // normalize(src, src, 0, 255, NORM_MINMAX); // $$$
        inRange(src, lowerb, upperb, thresh);
        medianBlur(thresh, thresh, 7); // $$

        morphologyEx(thresh, thresh, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(7, 7)));

        findContours(thresh, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
        Point puck_center;
        for (uint8_t i = 0; i < contours.size(); i++)
        {
            Rect rect = boundingRect(contours[i]);
            double peri = arcLength(contours[i], 1);

            // cout << rect << "\t" << peri << "\n";

            if (rect.width >= 7 && rect.width <= 15 && rect.height >= 5 && rect.height <= 13 && peri >= 28 && peri <= 40)
            {
                puck_center = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);

                circle(src, puck_center, 2, Scalar(0, 255, 0), -1, 8, 0);

                // Initialize prediction variables
                float x_0, y_0, x_1, y_1;
                float v_x, v_y;

                // Current point x_1, y_1
                x_1 = puck_center.x, y_1 = puck_center.y;

                auto t1 = chrono::steady_clock::now();     // Update current time
                chrono::duration<float> t_delta = t1 - t0; // Update t_delta
                t0 = t1;                                   // Update past time
                                                           // printf("Time between captures: %.3fms.\n", 1000 * t_delta.count());

                if (y_1 >= 112)
                {
                    v_x = (x_1 - x_0) / t_delta.count();
                    v_y = (y_1 - y_0) / t_delta.count();
                    x_0 = x_1, y_0 = y_1; // Update past point

                    if (abs(v_x) > 300)
                    {
                        v_x = 0;
                    }
                    if (abs(v_y) > 300)
                    {
                        v_y = 0;
                    }

                    // float x_min = 8, y_min = 3;
                    // float x_max = 139, y_max = 175;
                    float x_pred = x_1 + 20 * v_x * t_delta.count();
                    float y_pred = y_1 + 20 * v_y * t_delta.count();

                    // cout << v_x << "\t" << v_y << "\n";

                    line(src, Point(x_1, y_1), Point(x_pred, y_pred), Scalar(255, 255, 0), 1, LINE_8);

                    // Puck goes from
                    // X = 8 to X = 139
                    // Y = 3 to Y = 175
                    // Middle of table is Y = 112

                    // cout << puck_center.x << "\t" << puck_center.y << "\n";

                    // Send to predicted position
                    int16_t coord[2] = {100, 10};
                    serialFlush(fd);
                    write(fd, &coord, 4);
                }
            }
            break;
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
