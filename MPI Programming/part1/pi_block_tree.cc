#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#define SEED 35791246

void tree(int &count, int rank, int start, int end, int world_size)
{
		if(start==end)
        return;
    int mid = (start+end)/2;
    if(rank <= mid)
        tree(count, rank, start, mid, world_size);
    else
        tree(count, rank, mid++, end, world_size);
    if(rank == (mid+1))
        MPI_Send(&count, 1, MPI_INT, start, 1, MPI_COMM_WORLD);
    else if(rank==start)
    {
        int recieved;
				MPI_Recv(&recieved, world_size, MPI_INT, (mid+1), 1, MPI_COMM_WORLD,  MPI_STATUS_IGNORE);
        count += recieved;
    }
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

    // TODO: MPI init
    double x,y;                     //x,y value for the random coordinate
    int count=0, finalcount = 0;                 //Count holds all the number of how many good coordinates
    double z; 
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);           //get rank of node's process
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    long long int local_tosses = tosses/world_size;
    tosses = local_tosses*world_size;
    int recieved;
    unsigned int seed = SEED+world_rank;
    // TODO: binary tree redunction
    for (int i=0; i<local_tosses; i++)                  //main loop
    {
        x= ((double)rand_r(&seed))/RAND_MAX; 
        y =((double)rand_r(&seed))/RAND_MAX; 
        z = (x*x+y*y);
        if (z<=1)
            count++;
    }
    // tree(count, world_rank, 0, world_size-1, world_size);
    int alive = 1;
	  while(alive<world_size) // round up to nearest power of 2
	    alive *= 2;
	  
	  while(alive>1){
	
	    if(world_rank>=alive/2){
	      int dest = world_rank - (alive/2); 
	      // MPI_Send to rank - (alive/2);
	      if(dest>=0)
	      {
						MPI_Send(&count, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
		 		    break;
		    }			       
      }
	    
	    if(world_rank<(alive/2)){
	      MPI_Status status;
	      int source = world_rank + (alive/2); // round up
	      
	      //      MPI_Recv from rank + (alive/2);
	      if(source<world_size)
						MPI_Recv(&recieved, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
	      // add incoming data to our data;
	      count += recieved;
	    }
	    
	    alive = alive/2;
	  }
 

    if (world_rank == 0)
    {
        // TODO: PI result
        pi_result = ((double)count/(double)tosses)*4.0;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
