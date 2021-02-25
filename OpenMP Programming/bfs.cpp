#include "bfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <malloc.h>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1
#define THRESHOLD 1000000
using namespace std;

void bfs_top_down_big(Graph graph, solution *sol);	
void vertex_set_clear(vertex_set *list)
{
    list->count = 0;
}

void vertex_set_init(vertex_set *list, int count)
{
    list->max_vertices = count;
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}


// Take one step of "top-down" BFS.  For each vertex on the frontier,
// follow all outgoing edges, and add all neighboring vertices to the
// new_frontier.
void top_down_step_small(
    Graph g,
    vertex_set *frontier,
    vertex_set *new_frontier,
    int *distances)
{
    int count = 0;
    //int* local_frontier = (int*)malloc(sizeof(int) * (g->num_nodes/NUM_THREADS));
		//#pragma omp parallel
		{
        //#pragma omp for reduction(+ : count)
		    for (int i = 0; i < frontier->count; i++)
		    {
		        int node = frontier->vertices[i];
		
		        int start_edge = g->outgoing_starts[node];
		        int end_edge = (node == g->num_nodes - 1)
		                           ? g->num_edges
		                           : g->outgoing_starts[node + 1];
		
		        // attempt to add all neighbors to the new frontier
		        for (int neighbor = start_edge; neighbor < end_edge; neighbor++)
		        {
		            int outgoing = g->outgoing_edges[neighbor];
		
                if( __sync_bool_compare_and_swap(&distances[outgoing], NOT_VISITED_MARKER, distances[node] + 1)) 
										{                    
		                    new_frontier->vertices[count] = outgoing;
		                    count++;
                    }
								//if (distances[outgoing] == NOT_VISITED_MARKER)
		            //{
	              //   distances[outgoing] = distances[node] + 1;
		            //    
		            //    //int index = new_frontier->count++;
		            //    new_frontier->vertices[count++] = outgoing;
		            //    //count++;
		                

		            //}
		        }
		    }
	      new_frontier->count = count;
    }
}

// Implements top-down BFS.
//
// Result of execution is that, for each node in the graph, the
// distance to the root is stored in sol.distances.
void bfs_top_down(Graph graph, solution *sol)
{
    if(graph->num_nodes <= THRESHOLD)
    {
        //cout<<"small"<<endl;
		    vertex_set list1;
		    vertex_set list2;
		    vertex_set_init(&list1, graph->num_nodes);
		    vertex_set_init(&list2, graph->num_nodes);
		
		    vertex_set *frontier = &list1;
		    vertex_set *new_frontier = &list2;
		
		    // initialize all nodes to NOT_VISITED
		    #pragma omp parallel for
		    for (int i = 0; i < graph->num_nodes; i++)
		        sol->distances[i] = NOT_VISITED_MARKER;
		
		    // setup frontier with the root node
		    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
		    sol->distances[ROOT_NODE_ID] = 0;
        while (frontier->count != 0)
		    {
		        //cout<<frontier->count<<endl;
		        vertex_set_clear(new_frontier);
		
		        top_down_step_small(graph, frontier, new_frontier, sol->distances);
		
		        // swap pointers
		        vertex_set *tmp = frontier;
		        frontier = new_frontier;
		        new_frontier = tmp;
		    }
		}
	  else
        bfs_top_down_big(graph, sol);		  
    
}


void top_down_step_big(
    graph* g,
    vertex_set* frontier,    
    int* distances,    
    int iter)
{
    
    int count = 0;  
    #pragma omp parallel 
    {
        #pragma omp for reduction(+ : count)
        for (int i=0; i < g->num_nodes; i++) {   
            if (frontier->vertices[i] == iter){        
                int start_edge = g->outgoing_starts[i];
                int end_edge = (i == g->num_nodes-1) ? g->num_edges : g->outgoing_starts[i+1];
                // attempt to add all neighbors to the new frontier            
                for (int neighbor=start_edge; neighbor<end_edge; neighbor++) {
                    int outgoing = g->outgoing_edges[neighbor];
                    if(frontier->vertices[outgoing] == NOT_VISITED_MARKER) {  
                        
						            //cout<<"here!!"<<endl;
                    // if(__sync_bool_compare_and_swap(&frontier->vertices[outgoing], BOTTOMUP_NOT_VISITED_MARKER, iter + 1)) {                  
                        distances[outgoing] = distances[i] + 1;
                        count ++;
                        frontier->vertices[outgoing] = iter + 1;
                    }
                }
            }
        }
        
        // #pragma omp atomic                    
        //     frontier->count += count;
    }
    frontier->count = count;
}

// Implements top-down BFS.
//
// Result of execution is that, for each node in the graph, the
// distance to the root is stored in sol.distances.
void bfs_top_down_big(graph* graph, solution* sol) {
    

    vertex_set list1;
    
    vertex_set_init(&list1, graph->num_nodes);    

    int iter = 1;

    vertex_set* frontier = &list1;
            
    memset(frontier->vertices, NOT_VISITED_MARKER, sizeof(int) * graph->num_nodes);

    frontier->vertices[frontier->count++] = 1;        
	  #pragma omp parallel for
    for (int i = 0; i < graph->num_nodes; i++)
        sol->distances[i] = NOT_VISITED_MARKER;
    // set the root distance with 0
    sol->distances[ROOT_NODE_ID] = 0;    
        
    
    // just like pop the queue
    while (frontier->count != 0) {
        top_down_step_big(graph, frontier, sol->distances, iter);
        // bottom_up_step(graph, frontier1, sol->distances, iter);

        iter++;
        //cout<<sol->distances[1]<<endl;
    }
    

}

void bottom_up_step(
    graph* g,
    vertex_set* frontier,    
    int* distances,
    int iter) {
    
    // #pragma omp parallel num_threads(NUM_THREADS) private(count) 
    // #pragma omp parallel private(count)    
    // #pragma omp parallel num_threads(NUM_THREADS) private(count) 
    int count = 0;
    //memcpy(copy, no_visit_array, sizeof(no_visit_array));
    //int padding[15];
    #pragma omp parallel 
    {
        #pragma omp for reduction(+:count)
        for (int i=0; i < g->num_nodes; i++) {
				    //int index = no_visit_array[i];                
            if (frontier->vertices[i] == NOT_VISITED_MARKER) {
                int start_edge = g->incoming_starts[i];
                int end_edge = (i == g->num_nodes-1)? g->num_edges : g->incoming_starts[i + 1];
                for(int neighbor = start_edge; neighbor < end_edge; neighbor++) {
                    int incoming = g->incoming_edges[neighbor];
                    // if(__sync_bool_compare_and_swap(&frontier->vertices[incoming], iter, distances[node] + 1)) {
                    if(frontier->vertices[incoming] == iter) {
                        distances[i] = distances[incoming] + 1;                        
                        count ++;
                        frontier->vertices[i] = iter + 1;
                        //copy = remove(begin(copy), end(copy), index);
                        //drop.push_back(index);
                        break;
                    }
                }
            }
        }
        // #pragma omp atomic
        //     frontier->count += count;
    }    
    frontier->count = count;

}
/*
void bottom_up_step(
    graph* g,
    vertex_set* frontier,    
    int* distances,
    int iter,
		int* pbegin,
		int* pend) {
    
    // #pragma omp parallel num_threads(NUM_THREADS) private(count) 
    // #pragma omp parallel private(count)    
    // #pragma omp parallel num_threads(NUM_THREADS) private(count) 
    int count = 0;
    vector<int> drop;
    //memcpy(copy, no_visit_array, sizeof(no_visit_array));
    //int padding[15];
    #pragma omp parallel 
    {
        #pragma omp for reduction(+:count)
        for(int* i=pbegin; i < pend; i++) {
				    int index = *i;
				    //cout<<index<<endl;
            if (frontier->vertices[index] == NOT_VISITED_MARKER) {
                //cout<<"here"<<endl;
                int start_edge = g->incoming_starts[index];
                int end_edge = (index == g->num_nodes-1)? g->num_edges : g->incoming_starts[index + 1];
                for(int neighbor = start_edge; neighbor < end_edge; neighbor++) {
                    int incoming = g->incoming_edges[neighbor];
                    // if(__sync_bool_compare_and_swap(&frontier->vertices[incoming], iter, distances[node] + 1)) {
                    if(frontier->vertices[incoming] == iter) {
                        distances[index] = distances[incoming] + 1;                        
                        count ++;
                        frontier->vertices[index] = iter + 1;
                        //copy = remove(begin(copy), end(copy), index);
                        drop.push_back(index);
                        break;
                    }
                }
            }
        }
        // #pragma omp atomic
        //     frontier->count += count;
    }    
    frontier->count = count;
    for(int i=0; i<drop.size();i++)
    {
				pend = remove(pbegin, pend, drop[i]);
    }
}
*/

void bfs_bottom_up(Graph graph, solution *sol)
{
    // For PP students:
    //
    // You will need to implement the "bottom up" BFS here as
    // described in the handout.
    //
    // As a result of your code's execution, sol.distances should be
    // correctly populated for all nodes in the graph.
    //
    // As was done in the top-down case, you may wish to organize your
    // code by creating subroutine bottom_up_step() that is called in
    // each step of the BFS process.
    vertex_set list1;
    
    vertex_set_init(&list1, graph->num_nodes);
    
    int iter = 1;
    //int end = 1;
    int *no_visit_array = new int[graph->num_nodes];
    vertex_set* frontier = &list1; 
    
    memset(frontier->vertices, NOT_VISITED_MARKER, sizeof(int) * graph->num_nodes);
            
    // setup frontier with the root node    
    // just like put the root into queue
    frontier->vertices[frontier->count++] = 1;

    // set the root distance with 0
    #pragma omp parallel for
    for (int i=0; i<graph->num_nodes; i++)
    {
        sol->distances[i] = NOT_VISITED_MARKER;
        no_visit_array[i] = i;
    }
    sol->distances[ROOT_NODE_ID] = 0;
    int* pbegin = no_visit_array;                          // ^
    //cout<<malloc_usable_size(no_visit_array)<<endl;
    int* pend = no_visit_array+(malloc_usable_size(no_visit_array)-sizeof(no_visit_array))/sizeof(int);
    //cout<<pend-pbegin<<endl;
    
    //cout<<graph->num_nodes<<endl;
    /*
    int* pbegin = no_visit_array;                          // ^
    cout<<malloc_usable_size(no_visit_array)<<endl;
    int* pend = no_visit_array+(malloc_usable_size(no_visit_array)-sizeof(int))/sizeof(int);
    for(int* i=pbegin; i!=pend;++i)
        cout<<"Before: "<<*i<<endl;
    cout<<pbegin<<endl;
    cout<<pend<<endl;
    cout<<"Hi"<<pend-pbegin<<endl;
    pend = remove(pbegin, pend, 3);
    cout<<"remove 1"<<endl;
    cout<<(malloc_usable_size(no_visit_array)-sizeof(no_visit_array))/sizeof(int)<<endl;
    cout<<pend-pbegin<<endl;
    for(int* i=pbegin; i!=pend;++i)
        cout<<"After: "<<*i<<endl;
    */
    // printf("!!!!!!!!!!!!!!!!!!!!fro2: %-10d\n", frontier->count);
    // just like pop the queue
    while (frontier->count != 0) {
        //int len = no_visit_array.length();

        //bottom_up_step(graph, frontier, sol->distances, iter, pbegin, pend);
        bottom_up_step(graph, frontier, sol->distances, iter);
        iter++;

    }
    
}

void bfs_hybrid(Graph graph, solution *sol)
{
    // For PP students:
    //
    // You will need to implement the "hybrid" BFS here as
    // described in the handout.
    vertex_set list1;
    
    vertex_set_init(&list1, graph->num_nodes);
    
    int iter = 1;

    vertex_set* frontier = &list1;    
        
    // setup frontier with the root node    
    // just like put the root into queue
    memset(frontier->vertices, NOT_VISITED_MARKER, sizeof(int) * graph->num_nodes);

    frontier->vertices[frontier->count++] = 1;
    #pragma omp parallel for
    for (int i=0; i<graph->num_nodes; i++)
    {
        sol->distances[i] = NOT_VISITED_MARKER;
        //no_visit_array[i] = i;
    }
		// set the root distance with 0
    sol->distances[ROOT_NODE_ID] = 0;
    
    // just like pop the queue
    while (frontier->count != 0) {

        
        if(frontier->count >= THRESHOLD) {
            frontier->count = 0;
            bottom_up_step(graph, frontier, sol->distances, iter);
        }
        else {
            frontier->count = 0;
            top_down_step_big(graph, frontier, sol->distances, iter);
        }

        iter++;


    }     
}
