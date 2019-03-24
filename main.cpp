#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

#include <wiringPi.h>
#include <thread>
#include <sys/mman.h> // Needed for mlockall()
#include <unistd.h>   // needed for sysconf(int name);
#include <malloc.h>
#include <sys/time.h>     // needed for getrusage
#include <sys/resource.h> // needed for getrusage

/* **************Camera and frame capture configuration***************** */
#define FRM_COLS 640
#define FRM_ROWS 480
#define FRM_RATE 90
#define CAP_INTERVAL 12

Mat src(FRM_ROWS, FRM_COLS, CV_8UC3, Scalar(0, 0, 0));
Mat bgr[3] = {Mat(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0)), Mat(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0)), Mat(FRM_ROWS, FRM_COLS, CV_8UC1, Scalar(0))};

VideoCapture cam(0);
/* *************************************************************** */

/* *********************************Memory configuration********************** */
#define PRE_ALLOCATION_SIZE 10 * 1000 * 1000 /* Size of pagefault free buffer in bytes */
/* ************************************************************************ */

/* **************************Image processing configuration************************** */
Mat homography_matrix(3, 3, CV_8UC1, Scalar(0));
Mat table(FRM_ROWS, FRM_COLS, CV_8UC3, Scalar(0, 0, 0));
/* **************************************************************************** */

/* ********************Function prototypes*********************************** */
void setMaxPriority(pid_t pid);
void showNewPageFaultCount(const char *logtext, const char *allowed_maj, const char *allowed_min);
void reserveProcessMemory(int size);
void capTable(void);
/* ********************************************************************** */

int main()
{
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

    printf("\nCPU Configuration:\n");
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    CPU_SET(1, &mask);
    CPU_SET(2, &mask);
    CPU_SET(3, &mask);
    cout << "CPU affinity set to: ";
    for (int i = 0; i < 4; i++)
    {
        cout << CPU_ISSET(i, &mask) << " ";
    }
    cout << "\n";
    sched_setaffinity(primary_pid, sizeof(mask), &mask);

    // wiringPiSetup();

    printf("\nCamera configration:\n");
    cam.set(CAP_PROP_FRAME_WIDTH, FRM_COLS);
    cam.set(CAP_PROP_FRAME_HEIGHT, FRM_ROWS);
    printf("Frame Resolution: %d x %d\n", FRM_COLS, FRM_ROWS);

    cam.set(CAP_PROP_FPS, FRM_RATE);
    printf("Camera nominal frame rate: %d\n", FRM_RATE);
    printf("Program actual frame rate: %d\n", 1000 / CAP_INTERVAL);

    // string window_name = "Camera Feed";
    // namedWindow(window_name, WINDOW_NORMAL);

    printf("\nBegin program:\n");
    auto t2 = chrono::steady_clock::now();

    vector<Point2f> table_corners(4);
    Point2f table_corners_pixels[4] = {Point2f(100, 100), Point2f(100, 200), Point2f(300, 70), Point2f(300, 140)};

    vector<Point2f> desired_corners(4);
    Point2f desired_corners_pixels[4] = {Point2f(100, 100), Point2f(100, 200), Point2f(300, 100), Point2f(300, 200)};

    for (int i = 0; i < 4; i++)
    {
        table_corners[i] = table_corners_pixels[i];
        desired_corners[i] = desired_corners_pixels[i];
        cout << "Table Corner: " << table_corners[i] << "\tDesired Corner: " << desired_corners[i] << "\n";
    }

    homography_matrix = findHomography(table_corners, desired_corners);
    cout << "Generated Homography Matrix:\n"
         << homography_matrix << "\n\n";

    while (true)
    {
        auto next_time = chrono::steady_clock::now() + chrono::milliseconds(CAP_INTERVAL);
        auto t1 = chrono::steady_clock::now();
        chrono::duration<float, milli> t_elapse = t1 - t2;
        printf("Time between captures: %.3fms.\n", t_elapse.count());
        t2 = chrono::steady_clock::now();

        capTable();

        // imshow(window_name, src);
        // if (waitKey(10) == 27)
        // {
        // cout << "Esc key pressed, stopping feed.\n";
        // break;
        // }

        this_thread::sleep_until(next_time);
    }
    return 0;
}

Mat thresh;
Scalar lowerb = Scalar(0, 0, 150);
Scalar upperb = Scalar(20, 20, 256);
void capTable(void)
{

    cam.read(src);
    split(src, bgr);
    inRange(src, lowerb, upperb, thresh);
    cout << thresh.channels();

    // warpPerspective(thresh, thresh, homography_matrix, thresh.size());
}

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
