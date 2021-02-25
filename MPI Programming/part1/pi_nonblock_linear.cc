#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#define SEED 35791246

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: MPI init
    double x,y;                     //x,y value for the random coordinate
    int count=0, finalcount = 0;                 //Count holds all the number of how many good coordinates
    double z; 
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);           //get rank of node's process
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    long long int local_tosses = tosses/world_size;
    tosses = local_tosses*world_size;
    int recieved[world_size-1];
    unsigned int seed = SEED+world_rank;

    if (world_rank > 0)
    {
        // TODO: MPI workers
        for (int i=0; i<local_tosses; i++)                  //main loop
        {
            x= ((double)rand_r(&seed))/RAND_MAX; 
            y =((double)rand_r(&seed))/RAND_MAX; 
            z = (x*x+y*y);
            if (z<=1)
                count++;
        }
        
        /*for(int i=0; i<world_size; i++)
            MPI_Send(&count, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);*/
        MPI_Send(&count, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
        // TODO: non-blocking MPI communication.
        // Use MPI_Irecv, MPI_Wait or MPI_Waitall.
        MPI_Request requests[world_size-1];
        
        for(int i=0; i<world_size-1; i++)
        {
            MPI_Irecv(&recieved[i], world_size, MPI_INT, (i+1), 1, MPI_COMM_WORLD,  &requests[i]);
            
        }
        
				for (int i=0; i<local_tosses; i++)                  //main loop
        {
            x= ((double)rand_r(&seed))/RAND_MAX;
            y =((double)rand_r(&seed))/RAND_MAX;
            z = (x*x+y*y);
            if (z<=1)
                finalcount++;
        }
        for(int i=0; i<world_size-1; i++)
        {
            MPI_Wait(&requests[i], MPI_STATUS_IGNORE);
            finalcount += recieved[i];
        }
        // MPI_Wait(&request, MPI_STATUS_IGNORE);
        // MPI_Waitall();
    }

    if (world_rank == 0)
    {
        // TODO: PI result
        pi_result = ((double)finalcount/(double)tosses)*4.0; 

        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
