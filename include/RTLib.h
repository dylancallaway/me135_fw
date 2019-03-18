#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h> // Needed for mlockall()
#include <unistd.h>   // needed for sysconf(int name);
#include <malloc.h>
#include <sys/time.h>     // needed for getrusage
#include <sys/resource.h> // needed for getrusage
#include <pthread.h>
#include <limits.h>

#define PRE_ALLOCATION_SIZE (100 * 1024 * 1024) /* 100MB pagefault free buffer */
#define MY_STACK_SIZE (100 * 1024)              /* 100 kB is enough for now. */

static void setprio(int prio, int sched);

void show_new_pagefault_count(const char *logtext, const char *allowed_maj, const char *allowed_min);

static void prove_thread_stack_use_is_safe(int stacksize);

static void *my_rt_thread(void *args);

static void error(int at);

static void start_rt_thread(void);

static void configure_malloc_behavior(void);

static void reserve_process_memory(int size);
