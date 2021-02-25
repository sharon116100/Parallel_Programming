#include <iostream>
#include <mpi.h>
#include <cstdio>
#include <string>
#include <fstream>

using namespace std;

void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr, int **a_mat_ptr, int **b_mat_ptr)
{
		// string input_line;
		int rank, numprocs;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);           //get rank of node's process
	  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
		
		if(rank==0)
		{      
        cin >> *n_ptr;
	      cin >> *m_ptr;
        cin >> *l_ptr;
        // cout<< (*n_ptr)*(*m_ptr) <<endl;
        *a_mat_ptr = (int *)malloc((*n_ptr)*(*m_ptr)*sizeof(int *));
        // b_mat_ptr = (int **)malloc((*m_ptr)*sizeof(int *)+(*m_ptr)*(*l_ptr)*sizeof(int));
        *b_mat_ptr = (int *)malloc(((*l_ptr)*(*m_ptr))*sizeof(int *));
        // printf("start matrix1\n");
				for (int i=0;i < (*n_ptr); ++i)
			      for (int j=0;j <(*m_ptr); ++j)
						    cin >> (*a_mat_ptr)[(*m_ptr)*i+j];
 		    // printf("finish matrix1\n");
        for (int i=0;i < (*m_ptr); ++i)
            for (int j=0;j <(*l_ptr); ++j)
                cin >> (*b_mat_ptr)[(*l_ptr)*i+j];
	  }
	  MPI_Bcast(n_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(m_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(l_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0)
		    *b_mat_ptr = (int *)malloc(((*l_ptr)*(*m_ptr))*sizeof(int *));
		MPI_Bcast(*b_mat_ptr, (*m_ptr) * (*l_ptr), MPI_INT, 0, MPI_COMM_WORLD);
											
}
void matrix_multiply(const int n, const int m, const int l, const int *a_mat, const int *b_mat)
{
    int rank, numprocs, count, remainder, myRowsSize;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);           //get rank of node's process
	  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
		int* result = NULL;
		int* sendcounts = NULL;
		int* senddispls = NULL;
		int* recvcounts = NULL;
		int* recvdispls = NULL;
		
		// printf("matrix_multiply\n");
		count = n / numprocs;
		remainder = n - count * numprocs;
		if (rank == 0)
	  {
	      sendcounts = new int[numprocs];
				senddispls = new int[numprocs];
				recvcounts = new int[numprocs];
				recvdispls = new int[numprocs];
        int prefixSum = 0;
				for (int i = 0; i < numprocs; ++i)
				{
						int t1 = (i < remainder) ? count + 1 : count;
						recvcounts[i] = t1 * l;
						sendcounts[i] = t1 * m;
						int t2 = prefixSum;
						recvdispls[i] = t2 * l;
						senddispls[i] = t2 * m;
						prefixSum += t1;
		    } 
		}
		 
		myRowsSize = rank < remainder ? count + 1 : count;
		int* matrixPart = new int[myRowsSize * m];
		MPI_Scatterv(a_mat, sendcounts, senddispls, MPI_INT, matrixPart, myRowsSize * m, MPI_INT, 0, MPI_COMM_WORLD);
		if (0 == rank)
		{
			delete[] sendcounts;
			delete[] senddispls;
		}
    int* resultPart = new int[myRowsSize * l];
		for (int i = 0; i < myRowsSize; ++i)
		{
				for (int j = 0; j < l; ++j)
				{
						resultPart[i * l + j] = 0;
						for (int k = 0; k < m; ++k)
						{
							  resultPart[i * l + j] += matrixPart[i * m + k] * b_mat[k * l + j];
						}
				}
		}
		delete[] matrixPart;
	  if (0 == rank)
        result = new int[n * l];
		MPI_Gatherv(resultPart, myRowsSize * l, MPI_INT, result, recvcounts, recvdispls, MPI_INT, 0, MPI_COMM_WORLD); 
		delete[] resultPart;
		if (0 == rank)
		{
			delete[] recvcounts;
			delete[] recvdispls;
		}
    //ofstream fout("test.txt");
    if (0 == rank)
		{
        
				// cout << "result:" << endl;
        for (int i = 0; i < n; ++i)
				{
						// cout << i << endl;
						for (int j = 0; j < l-1; ++j)
	 	 		    {
                cout << result[i * l + j] << ' ';
						}
				    cout << result[i * l + l-1] << endl;
            /*for (int j = 0; j < l; ++j)
            {
                if(j==l-1)
                {
                    fout << result[i * l + j] << '\n';
								}
							  else
						    {
            		fout << result[i * l + j] << ' ';
								}
						}*/
            
				}
        //fout.close();
				delete[] result;
		}		
}
void destruct_matrices(int *a_mat, int *b_mat)
{
    int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);           //get rank of node's process
		if (0 == rank)
		{
				delete[] a_mat;
	      delete[] b_mat;	
		}
		
}