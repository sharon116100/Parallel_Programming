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
    int count=0, finalcount;                 //Count holds all the number of how many good coordinates
    double z; 
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);           //get rank of node's process
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    long long int local_tosses = tosses/world_size;
    tosses = local_tosses*world_size;
    unsigned int seed = SEED+world_rank;
    // TODO: use MPI_Reduce
    for (int i=0; i<local_tosses; i++)                  //main loop
        {
            x= ((double)rand_r(&seed))/RAND_MAX; 
            y =((double)rand_r(&seed))/RAND_MAX; 
            z = (x*x+y*y);
            if (z<=1)
                count++;
        }
    MPI_Reduce(&count, &finalcount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

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
