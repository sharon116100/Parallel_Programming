#include "page_rank.h"

#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <omp.h>
#include <utility>

#include "../common/CycleTimer.h"
#include "../common/graph.h"
using namespace std;

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is num_nodes(g))
// damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//
void pageRank(Graph g, double *solution, double damping, double convergence)
{

  // initialize vertex weights to uniform probability. Double
  // precision scores are used to avoid underflow for large graphs
  
  int numNodes = num_nodes(g);
  bool converged = false;
  double equal_prob = 1.0 / numNodes;
  double* score_old = new double[numNodes];
  //Vertex* no_outgoing= new Vertex[numNodes];
  //int index = 0;
  //#pragma omp parallel for firstprivate(numNodes, equal_prob) shared(solution)
  #pragma omp parallel for
  for (int i = 0; i < numNodes; ++i)
  {
    solution[i] = equal_prob;
    /*
    if(!outgoing_size(g, i))
    {
      no_outgoing[index] = i;
      index++; 
			//cout<<"no_outgoing"<<index<<endl;     
		}
	  */
  }
  while(!converged)
  {
    //cout<<"iter:"<<iter<<endl;
    double sum_no_outgoing_old = 0, global_diff = 0;
		//#pragma omp parallel for firstprivate(numNodes) lastprivate(score_old) shared(g, solution) reduction(+:sum_no_outgoing_old)
		#pragma omp parallel for schedule(auto)
    for(int i=0;i<numNodes;i++)
	  {
      score_old[i] = solution[i];
      if(!outgoing_size(g, i))
      {
        #pragma omp atomic
        sum_no_outgoing_old += solution[i];
      } 
			/*   
      else
      {
        continue;
      }	
      */
		}
	  fill_n(solution, numNodes, 0);
	  //cout<<"finish sum_no"<<endl;
	  //sum_no_outgoing_old /= numNodes;
    //#pragma omp parallel for firstprivate(score_old, convergence, damping, sum_no_outgoing_old, numNodes) shared(g, solution) reduction(+:global_diff)
    #pragma omp parallel for
		for(int i=0; i<numNodes; i++)
    {
      const Vertex* start = incoming_begin(g, i);
      const Vertex* end = incoming_end(g, i);
      
			for(const Vertex* v=start; v!=end; v++)
			{
			  solution[i] += score_old[*v] / outgoing_size(g, *v);
			}
			solution[i] = (damping * solution[i]) + (1.0-damping) / numNodes;
 	    solution[i] += (damping*sum_no_outgoing_old)/numNodes;
      #pragma omp atomic
      global_diff += abs(solution[i]-score_old[i]);
      
		}
	  converged = (global_diff < convergence);
    //cout << "global_diff"<<global_diff << endl;
  }
  

  /*
     For PP students: Implement the page rank algorithm here.  You
     are expected to parallelize the algorithm using openMP.  Your
     solution may need to allocate (and free) temporary arrays.

     Basic page rank pseudocode is provided below to get you started:

     // initialization: see example code above
     score_old[vi] = 1/numNodes;

     while (!converged) {

       // compute score_new[vi] for all nodes vi:
       score_new[vi] = sum over all nodes vj reachable from incoming edges
                          { score_old[vj] / number of edges leaving vj  }
       score_new[vi] = (damping * score_new[vi]) + (1.0-damping) / numNodes;

       score_new[vi] += sum over all nodes v in graph with no outgoing edges
                          { damping * score_old[v] / numNodes }

       // compute how much per-node scores have changed
       // quit once algorithm has converged

       global_diff = sum over all nodes vi { abs(score_new[vi] - score_old[vi]) };
       converged = (global_diff < convergence)
     }

   */
}
