#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#define SEED 35791246

// Compares schedule and oldschedule and prints schedule if different
// Also displays the time in seconds since the first invocation
int fnz (int *schedule, int *oldschedule, int size)
{
    int diff = 0;

    for (int i = 0; i < size; i++)
       diff |= (schedule[i] != oldschedule[i]);

    if (diff)
    {
       int res = 0;

       // printf("[%6.3f] Schedule:", MPI_Wtime() - starttime);
       for (int i = 0; i < size; i++)
       {
          // printf("\t%d", schedule[i]);
          res += (int)(schedule[i]>0);
          oldschedule[i] = schedule[i];
       }
       // printf("\n");
       return(res == size-1);
    }
    return 0;
}

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    MPI_Win win;

    // TODO: MPI init
    double x,y;                     //x,y value for the random coordinate
    int count=0;                 //Count holds all the number of how many good coordinates
    double z; 
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);           //get rank of node's process
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    long long int local_tosses = tosses/world_size;
    tosses = local_tosses*world_size;
    unsigned int seed = SEED+world_rank;
    
    for (int i=0; i<local_tosses; i++)                  //main loop
    {
        x= ((double)rand_r(&seed))/RAND_MAX; 
        y =((double)rand_r(&seed))/RAND_MAX; 
        z = (x*x+y*y);
        if (z<=1)
            count++;
    }

    if (world_rank == 0)
    {
        // Master
        // Create a window. Set the displacement unit to sizeof(int) to simplify
        // the addressing at the originator processes
        int *oldschedule = (int*)malloc(world_size * sizeof(int));
				// Use MPI to allocate memory for the target window
				int *schedule;
				MPI_Alloc_mem(world_size * sizeof(int), MPI_INFO_NULL, &schedule);
				
				for (int i = 0; i < world_size; i++)
				{
				  schedule[i] = 0;
				  oldschedule[i] = -1;
				}
        
        // Create a window. Set the displacement unit to sizeof(int) to simplify
				// the addressing at the originator processes
				MPI_Win_create(schedule, world_size * sizeof(int), sizeof(int), MPI_INFO_NULL,
				  MPI_COMM_WORLD, &win);
				
				int ready = 0;
				while (!ready)
				{
				  // Without the lock/unlock schedule stays forever filled with 0s
				  MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
				  ready = fnz(schedule, oldschedule, world_size);
				  MPI_Win_unlock(0, win);
				}
				 										
				// Release the window
				MPI_Win_free(&win);
				pi_result = count;
			  for(int i=1; i<world_size; ++i)
				{
            pi_result += (double)schedule[i];
        }
        // pi_result = pi_result * 4.0 / tosses;
        pi_result = ((double)pi_result/(double)tosses)*4.0;
        // Free the allocated memory
				MPI_Free_mem(schedule);
				free(oldschedule);
    }
    else
    {
        // Workers
        // Worker processes do not expose memory in the window
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
				MPI_Put(&count, 1, MPI_INT, 0, world_rank, 1, MPI_INT, win);
        MPI_Win_unlock(0, win);
        // printf("Worker %d finished RMA\n", world_rank);
        MPI_Win_free(&win);
        // printf("Worker %d done\n", world_rank);
    }
    if (world_rank == 0)
    {
        // TODO: handle PI result
			  
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}