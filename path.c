#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "path.h"
#include "global.h"

void run_path_algorithm(const timing_analyze_graph* graph)
{
    /* first initialize all vertexs' front_crit_path_through_pin *
     * and behind_crit_path_through_pin.                         */
    init_all_vertexs_front_and_behind_value(graph);
    printf("initial all vertexs' front and behind value OK!, then calculate all vertexs' arrival_time, max_delay.\n");

    /* second calculate all vertexs' arrival_time, max_delay and return *
     * the number of vertexs that find in topo sorting.                 */ 
    stack_t* topo_sort = create_stack();
    stack_t* reverse_topo_sort = create_stack();
    printf("topo_sort = %p, reverse_topo_sort = %p\n",
           topo_sort,
           reverse_topo_sort);
    /* assert(topo_sort != NULL && reverse_topo_sort != NULL); */

    double kmax_delay = 0.0;
    const int count = calc_all_vertexs_arrival_time(graph,
                                                    topo_sort,
                                                    reverse_topo_sort,
                                                    &kmax_delay);
    printf("calculate all vertexs' s arrival_time OK! kmax_delay = %f\n",
           kmax_delay);
    /* in this circumstance, there must be circle in graph */
    if (count < graph->m_num_of_vertexs) {
         exit(1);
    }

    /* third calculate all vertexs' front_crit_path_through_pin */
    calc_front_crit_path_through_pin(graph,
                                     kmax_delay);
    printf("calculate all vertexs' critical_path_through_pin OK!\n");
    /* forth, use reverse top-sort to compute all vertexs' required_time, slack *
     * behind_crit_path_through_pin.                                            */
    calc_all_vertexs_required_time(graph,
                                   kmax_delay,
                                   reverse_topo_sort);
    printf("calculate all vertexs' required_time OK!\n");
    /* fifth, calculate all vertexs' behind_crit_path_through_pin */
    calc_behind_crit_path_through_pin(graph,
                                      kmax_delay);
    printf("calculate all vertexs' behind_crit_path_through_pin OK!\n");
    /* last, calculate all edge's local_crit_path_through_edge(weight). */
    calc_local_crit_path_through_edge(graph, kmax_delay);
    printf("calculate all edges's local_crit_path_through_edge OK!\n");

    /* Don't forget free the 2 stacks */
    destroy_stack(topo_sort);
    destroy_stack(reverse_topo_sort);
    printf("free 2 stacks OK!\n");
} /* end of void run_path_algo()  */

void init_all_vertexs_front_and_behind_value(const timing_analyze_graph* graph)
{
    const int num_of_vertexs = graph->m_num_of_vertexs;
    int v = -1;
    for (v = 0; v < num_of_vertexs; ++v) {
        graph->m_vertexs[v]->m_front_crit_path_through_pin = 0.0;
        graph->m_vertexs[v]->m_behind_crit_path_through_pin = 0.0;
    }

    const int num_of_primary_inputs = graph->m_num_of_primary_inputs;
    for (v = 0; v < num_of_primary_inputs; ++v) {
        graph->m_primary_inputs[v]->m_front_crit_path_through_pin = 1.0;
    }

    const int num_of_primary_outputs = graph->m_num_of_primary_outputs;
    for (v = 0; v < num_of_primary_outputs; ++v) {
        graph->m_primary_output[v]->m_behind_crit_path_through_pin = 1.0;
    }
} /* end of void init_all_vertexs_front_and_behind_value(graph) */

int  calc_all_vertexs_arrival_time(const timing_analyze_graph* graph,
                                   stack_t* topo_sort,
                                   stack_t* reverse_topo_sort,
                                   double* kmax_delay)
{
    printf("Enter in calc_all_vertexs_arrival_time()...\n");
    const int num_of_primary_inputs = graph->m_num_of_primary_inputs;
    printf("num_of_primary_inputs = %d\n", num_of_primary_inputs);
    int v = 0;
    while (v < num_of_primary_inputs) {
        stack_push(topo_sort,
                   graph->m_primary_inputs[v]);
        ++v;
    }

    /* second, use topo-sort to calculate all vertexs' arrival_time, kmax_delay *
     * and front_crit_path_through_pin. */
    int count = 0;
    while (stack_is_empty(topo_sort) == kfalse) {
        vertex_t* top_elem = stack_pop(topo_sort);
        ++count;
        stack_push(reverse_topo_sort, top_elem);

        /* then calculate all vertexs' arrival_time, kmax_delay */
        edge_t* e = top_elem->m_first_edge;
        while (e != NULL) {
            vertex_t* tail = e->m_sink;
            if (--(tail->m_indegree) == 0) {
                stack_push(topo_sort, tail);
            }

            if (top_elem->m_arrival_time + e->m_delay > tail->m_arrival_time) {
                tail->m_arrival_time = top_elem->m_arrival_time + e->m_delay;
                if (tail->m_arrival_time > *kmax_delay) {
                    *kmax_delay = tail->m_arrival_time;
                }
            }
            e = e->m_next;
        } /* end of while(e != NULL) */
    } /* end of while(stack_is_empty(topo_sort) == kfalse) */

    printf("vertex_count = %d\n", count);
    return count;
} /* end of int calc_all_vertexs_arrival_time(graph, topo_sort...)*/

void calc_front_crit_path_through_pin(const timing_analyze_graph* graph,
                                      const double kmax_delay)
{
    const int num_of_vertexs = graph->m_num_of_vertexs;
    int v = -1;
    for (v = 0; v < num_of_vertexs; ++v) {
        vertex_t* source = graph->m_vertexs[v];
        edge_t* e = source->m_first_edge;
        while (e != NULL) {
            vertex_t* tail = e->m_sink;
            /* forward_local_slack(i,j) = arrival_time(j) - arrival_time(i) - delay(i,j) */
            e->m_forward_local_slack = tail->m_arrival_time - source->m_arrival_time
                                         - e->m_delay;
            double forward_value = compute_discount_value(e->m_forward_local_slack,
                                                          kmax_delay);
            tail->m_front_crit_path_through_pin += (forward_value
                                                      * source->m_front_crit_path_through_pin); 
            e = e->m_next;
        }
    }
}

void calc_all_vertexs_required_time(const timing_analyze_graph* graph,
                                    const double kmax_delay,
                                    stack_t* reverse_topo_sort)
{
    /* 1.initial all vertexs' required_time as kmax_delay */
    int v = 0;
    while (v < graph->m_num_of_vertexs) {
        graph->m_vertexs[v]->m_required_time = kmax_delay;
        ++v;
    }
    /* 2.calculate all vertexs's required_time, slack and behind_crit_path_through_pin */
    while (kfalse == stack_is_empty(reverse_topo_sort)) {
        vertex_t* top_elem = stack_pop(reverse_topo_sort);
        edge_t* e = top_elem->m_first_edge;
        while (e != NULL) {
            vertex_t* tail = e->m_sink;
            if (tail->m_required_time - e->m_delay < top_elem->m_required_time) {
                top_elem->m_required_time = tail->m_required_time - e->m_delay;
                /* slack(i,j) = required_time(j) - arrival_time(i) - delay(i,j) */
                e->m_slack = tail->m_required_time - top_elem->m_arrival_time - e->m_delay;
            }
            e = e->m_next;
        }
    } /* end of while(stack_is_empty(reverse_topo_sort) == kfalse) */
} /* end of void calc_all_vertexs_required_time(graph,...) */

void calc_behind_crit_path_through_pin(const timing_analyze_graph* graph,
                                       const double kmax_delay)
{
    const int num_of_vertexs = graph->m_num_of_vertexs;
    int v = -1;
    for (v = 0; v < num_of_vertexs; ++v) {
        vertex_t* source = graph->m_vertexs[v];
        edge_t* e = source->m_first_edge;
        while (e != NULL) {
            vertex_t* tail = e->m_sink;
            e->m_backward_local_slack = tail->m_required_time - source->m_required_time
                                          - e->m_delay;
            double backward_value = compute_discount_value(e->m_backward_local_slack,
                                                           kmax_delay);
            source->m_behind_crit_path_through_pin += backward_value
                                                        * tail->m_behind_crit_path_through_pin;
            e = e->m_next;
        }
    }
} /* end of void calc_behind_crit_path_through_pin() */

void calc_local_crit_path_through_edge(const timing_analyze_graph* graph,
                                       const double kmax_delay)
{
    const int num_of_vertexs = graph->m_num_of_vertexs;
    int v = 0;
    while (v < num_of_vertexs) {
        edge_t* e = graph->m_vertexs[v]->m_first_edge;
        while (e != NULL) {
            const vertex_t* source = e->m_source;
            const vertex_t* tail = e->m_sink;
            const double front_crit_path_through_pin = source->m_front_crit_path_through_pin;
            const double behind_crit_path_through_pin = tail->m_behind_crit_path_through_pin;

            const double slack = e->m_delay;
            const double discount_value = compute_discount_value(slack,
                                                                 kmax_delay);

            e->m_local_crit_path_through_edge = (front_crit_path_through_pin
                                                  * behind_crit_path_through_pin
                                                  * discount_value);

            e = e->m_next; /* then, calculate next edge */
        } /* end of while(e != NULL) */
        ++v;
    } /* end of while(v < num_of_vertexs) */
} /* end of void calc_local_crit_path_through_edge(graph) */

double compute_discount_value(const double x, const double y)
{
    return exp(-x / y);
}

/* void find_all_crit_paths_in_graph(const timing_analyze_graph* graph)
{
} */
