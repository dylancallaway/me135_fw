#ifndef RTLIB_INCLUDED
#define RTLIB_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h> // Needed for mlockall()
#include <unistd.h>   // needed for sysconf(int name);
#include <malloc.h>
#include <sys/time.h>     // needed for getrusage
#include <sys/resource.h> // needed for getrusage
#include <pthread.h>
#include <limits.h>

#ifndef PRE_ALLOCATION_SIZE
#define PRE_ALLOCATION_SIZE (100 * 1024 * 1024) /* 100MB pagefault free buffer */
#endif

#ifndef MY_STACK_SIZE
#define MY_STACK_SIZE (100 * 1024)              /* 100 kB is enough for now. */
#endif

void setprio(int prio, int sched);

void show_new_pagefault_count(const char *logtext, const char *allowed_maj, const char *allowed_min);

void prove_thread_stack_use_is_safe(int stacksize);

void *my_rt_thread(void *args);

void error(int at);

void start_rt_thread(void);

void configure_malloc_behavior(void);

void reserve_process_memory(int size);

#endif