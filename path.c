#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

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
    stack_t* zero_indegree = create_stack();
    stack_t* topo_sort = create_stack();
    printf("zero_indegree = %p, topo_sort = %p\n",
           zero_indegree,
           topo_sort);
    /* assert(zero_indegree != NULL && topo_sort != NULL); */

    double kmax_delay = 0.0;
    const int count = calc_all_vertexs_arrival_time(graph,
                                                    zero_indegree,
                                                    topo_sort,
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
                                   topo_sort);
    printf("calculate all vertexs' required_time OK!\n");
    /* fifth, calculate all vertexs' behind_crit_path_through_pin */
    calc_behind_crit_path_through_pin(graph,
                                      kmax_delay);
    printf("calculate all vertexs' behind_crit_path_through_pin OK!\n");
    /* last, calculate all edge's local_crit_path_through_edge(weight). */
    calc_local_crit_path_through_edge(graph, kmax_delay);
    printf("calculate all edges's local_crit_path_through_edge OK!\n");

    /* Don't forget free the 2 stacks */
    destroy_stack(zero_indegree);
    destroy_stack(topo_sort);
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
                                   stack_t* zero_indegree,
                                   stack_t* topo_sort,
                                   double* kmax_delay)
{
    printf("Enter in calc_all_vertexs_arrival_time()...\n");
    const int num_of_primary_inputs = graph->m_num_of_primary_inputs;
    printf("num_of_primary_inputs = %d\n", num_of_primary_inputs);
    int v = 0;
    while (v < num_of_primary_inputs) {
        stack_push(zero_indegree,
                   graph->m_primary_inputs[v]);
        ++v;
    }

    /* second, use topo-sort to calculate all vertexs' arrival_time, kmax_delay *
     * and front_crit_path_through_pin. */
    int count = 0;
    while (kfalse == stack_is_empty(zero_indegree)) {
        vertex_t* top_elem = stack_pop(zero_indegree);
        printf("\ncurrent vertex: %s\n", top_elem->m_name);
        stack_push(topo_sort, top_elem);
        ++count;

        /* then calculate all vertexs' arrival_time, kmax_delay */
        edge_t* e = top_elem->m_first_edge;
        while (e != NULL) {
            vertex_t* tail = e->m_sink;
            if (--(tail->m_indegree) == 0) {
                stack_push(zero_indegree, tail);
                printf("%s push into zero_indegree\n", tail->m_name);
            }

            if (top_elem->m_arrival_time + e->m_delay > tail->m_arrival_time) {
                tail->m_arrival_time = top_elem->m_arrival_time + e->m_delay;
                printf("  %s: %s arrival_time = %f\n",
                       e->m_name,
                       tail->m_name,
                       tail->m_arrival_time);
                if (tail->m_arrival_time > *kmax_delay) {
                    *kmax_delay = tail->m_arrival_time;
                }
            }
            e = e->m_next;
        } /* end of while(e != NULL) */
    } /* end of while(kfalse == stack_is_empty(zero_indegree)) */

    printf("vertex_count = %d\n", count);
    return count;
} /* end of int calc_all_vertexs_arrival_time(graph, zero_indegree...)*/

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
            tail->m_front_crit_path_through_pin += forward_value
                                                      * source->m_front_crit_path_through_pin;
            e = e->m_next;
        }
    }
}

void calc_all_vertexs_required_time(const timing_analyze_graph* graph,
                                    const double kmax_delay,
                                    stack_t* topo_sort)
{
    /* 1.initial all vertexs' required_time as kmax_delay */
    int v = 0;
    while (v < graph->m_num_of_vertexs) {
        graph->m_vertexs[v]->m_required_time = kmax_delay;
        ++v;
    }
    /* 2.calculate all vertexs's required_time, slack and behind_crit_path_through_pin */
    while (kfalse == stack_is_empty(topo_sort)) {
        vertex_t* top_elem = stack_pop(topo_sort);
        edge_t* e = top_elem->m_first_edge;
        while (e != NULL) {
            vertex_t* tail = e->m_sink;
            if (tail->m_required_time - e->m_delay < top_elem->m_required_time) {
                top_elem->m_required_time = tail->m_required_time - e->m_delay;
            }
            e = e->m_next;
        }
    } /* end of while(stack_is_empty(topo_sort) == kfalse) */
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
            /* slack(i,j) = required_time(j) - arrival_time(i) - delay(i,j) */
            e->m_slack = tail->m_required_time - source->m_arrival_time - e->m_delay;
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

            const double slack = e->m_slack;
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

void print_all_nets_weight(const timing_analyze_graph* graph)
{
    FILE* output_file = fopen("print_all_nets_weight.log", "w");
    const int num_of_vertexs = graph->m_num_of_vertexs;
    int v = -1;
    char line[128];
    char str[8];
    for (v = 0; v < num_of_vertexs; ++v) {
        vertex_t* source = graph->m_vertexs[v];
        edge_t* edge = NULL;
        for (edge = source->m_first_edge; edge != NULL; edge = edge->m_next) {
            strcpy(line, "Net: ");
            strcat(line, edge->m_name);
            strcat(line, ", ");
            /* source vertex's arrival_time */
            strcat(line, edge->m_source->m_name);
            strcat(line, " arr_time: ");
            sprintf(str, "%f", edge->m_source->m_arrival_time);
            strcat(line, str);

            /* sink vertex's required_time */
            strcat(line, ", ");
            strcat(line, edge->m_sink->m_name);
            strcat(line, " req_time: ");
            sprintf(str, "%f", edge->m_sink->m_required_time);
            strcat(line, str);

            strcat(line, ", slack = ");
            sprintf(str, "%f", edge->m_slack);
            strcat(line, str);

            strcat(line, ", weight = ");
            sprintf(str, "%f", edge->m_local_crit_path_through_edge);
            strcat(line, str);
            strcat(line, "\n");

            fputs(line, output_file);
        }
    }
    fclose(output_file);
}

