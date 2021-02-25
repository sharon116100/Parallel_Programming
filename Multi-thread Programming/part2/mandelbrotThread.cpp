#include <stdio.h>
#include<iostream>
#include <thread>
#include <ctime>
#include "CycleTimer.h"

typedef struct
{
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int *output;
    int threadId;
    int numThreads;
} WorkerArgs;

extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);

//
// workerThreadStart --
//
// Thread entrypoint.

void workerThreadStart(WorkerArgs *const args)
{
    // TODO FOR PP STUDENTS: Implement the body of the worker
    // thread here. Each thread should make a call to mandelbrotSerial()
    // to compute a part of the output image.  For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.

    int threadId = args->threadId;
    int height = args->height;
    int numThreads = args->numThreads;
    int cut = 120;
    int gap = args->height/cut;
    // int startRow = threadId * (height/numThreads);
    // int totalRow = height/numThreads;
    /*
    if(numThreads == 3)
    {
      if(threadId == 0)
			  mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow, (int)(totalRow*1.25), args->maxIterations, args->output);
	    else if(threadId == 1)
			    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow + (int)(totalRow*0.25), (int)(totalRow*0.5), args->maxIterations, args->output);
      else
        mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow - (int)(totalRow*0.25), (int)(totalRow*1.25), args->maxIterations, args->output);
		}
    else if(numThreads == 4)
    {
      if(threadId == 0)
        mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow, (int)(totalRow*1.25), args->maxIterations, args->output);
      else if(threadId == 1)
			    mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow + (int)(totalRow*0.25), (int)(totalRow*0.75), args->maxIterations, args->output);
	     else if(threadId == 2)
          mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow, (int)(totalRow*0.75), args->maxIterations, args->output);
      else
        mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow - (int)(totalRow*0.25), (int)(totalRow*1.25), args->maxIterations, args->output);
		}
    else     
		  mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, args->height, startRow, totalRow, args->maxIterations, args->output);
    */
    //pthread_mutex_lock(&mutex_mon);
    //threadId_ += 1;
    //pthread_mutex_unlock(&mutex_mon);
    //printf("Hello world from thread %d\n", args->threadId);
    
    //unsigned int height = args->height;
    for(int i = threadId*gap; i < height; i+=gap*numThreads)
      mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, args->width, height, i, gap, args->maxIterations, args->output);

    //double endTime = CycleTimer::currentSeconds();
    //minSerial = std::min(minSerial, endTime - startTime);
    //printf("[thread %d]:\t\t[%.3f] ms\n", threadId, minSerial * 1000);
    
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];
    // WorkerArgs args0 = {x0, x1, y0, y1, (unsigned int)width, (unsigned int)height, maxIterations, output, 1, numThreads};
    //pthread_mutex_init(&mutex_mon,NULL);
    for (int i = 0; i < numThreads; i++)
    {
        // TODO FOR PP STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        args[i].output = output;
        args[i].threadId = i;
    }
    


    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i = 1; i < numThreads; i++)
    {
        // args[i] = args[0];
        // args[i].threadId = i;
				workers[i] = std::thread(workerThreadStart, &args[i]);
    }
    workerThreadStart(&args[0]);

    // join worker threads
    for (int i = 1; i < numThreads; i++)
    {
        workers[i].join();
    }
}
