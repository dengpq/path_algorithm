#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "path.h"
#include "global.h"


void run_path_algorithm(const timing_analyze_graph* graph,
                        const char* algo_type)
{
    /* First initialize all vertexs' front_crit_path_through_pin and *
     * behind_crit_path_through_pin.                                 */
    printf("Enter in run_path_algorithm(),algo_type:%s, first initial all vertexs' value...\n",
           algo_type);
    init_all_vertexs_value(graph);

    /* Second calculate all vertexs' arrival_time, max_delay and return the *
     * number of vertexs that find by topo_sorting or breath-first search.  */
    stack_t* zero_indegree = create_stack();
    stack_t* topo_sort = create_stack();

    queue_t* queue = create_queue();
    vertex_level_t** netlist_level = (vertex_level_t**)malloc(gmax_size * sizeof(vertex_level_t*));
    int  num_of_levels = 0;
    assert(zero_indegree != NULL && topo_sort != NULL && queue != NULL
             && netlist_level!=NULL);

    double max_delay = 0.0;
    int    count = 0;
    if (strcmp(algo_type, "topo_sort") == 0) {
        printf("Use topo_sorting to calculate all vertexs' arrival_time...\n");
        count = calc_all_vertexs_arrival_time(graph,
                                              zero_indegree,
                                              topo_sort,
                                              &max_delay);
    } else {
        printf("Use breath-first searching to calculate all vertexs' arrival_time...\n");
        count = calc_all_vertexs_arrival_time_by_bfs(graph,
                                                     queue,
                                                     netlist_level,
                                                     &num_of_levels,
                                                     &max_delay);
    }
    printf("Calculate all vertexs' arr_time OK!max_delay = %f,count = %d,num_of_levels = %d\n",
           max_delay,
           count,
           num_of_levels);
    /* In this circumstance, there must be circle in graph */
    if (count < graph->m_num_of_vertexs) {
        exit(1);
    }

    /* Third, calculate all vertexs' front_crit_path_through_pin */
    calc_front_crit_path_through_pin(graph,
                                     max_delay);
    printf("calculate all vertexs' critical_path_through_pin OK!\n");
    /* Forth, use reverse top-sorting to compute all vertexs' required_time, *
     * slack and behind_crit_path_through_pin.                               */
    if (strcmp(algo_type,"topo_sort") == 0) {
        printf("Use topo-sorting to calculate all vertexs' required_time...\n");
        calc_all_vertexs_required_time(graph,
                                       max_delay,
                                       topo_sort);
    } else {
        printf("Use breath-first searching to calculate all vertexs' required_time...\n");
        calc_all_vertexs_required_time_by_bfs(graph,
                                              max_delay,
                                              num_of_levels,
                                              netlist_level);
    }
    printf("Calculate all vertexs' required_time OK!\n");
    /* Fifth, calculate all vertexs' behind_crit_path_through_pin */
    calc_behind_crit_path_through_pin(graph,
                                      max_delay);
    printf("Calculate all vertexs' behind_crit_path_through_pin OK!\n");
    /* Last, calculate all edge's local_crit_path_through_edge(weight). */
    calc_local_crit_path_through_edge(graph,
                                      max_delay);
    printf("Calculate all edges' local_crit_path_through_edge OK!\n");

    /* Don't forget free the 2 stacks */
    destroy_stack(zero_indegree);
    destroy_stack(topo_sort);

    destroy_queue(queue);
    destroy_netlist_level(netlist_level,
                          num_of_levels);
    printf("Free 2 stacks, queue and netlist_level OK!\n");
} /* end of void run_path_algorithm() */

void init_all_vertexs_value(const timing_analyze_graph* graph)
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
        graph->m_primary_outputs[v]->m_behind_crit_path_through_pin = 1.0;
    }
} /* end of void init_all_vertexs_value(graph) */

int calc_all_vertexs_arrival_time(const timing_analyze_graph* graph,
                                  stack_t* zero_indegree,
                                  stack_t* topo_sort,
                                  double*  kmax_delay)
{
    const int num_of_primary_inputs = graph->m_num_of_primary_inputs;
    int v = -1;
    for (v = 0; v < num_of_primary_inputs; ++v) {
        stack_push(zero_indegree,
                   graph->m_primary_inputs[v]);
    }

    /* second, use topo-sort to calculate all vertexs' arrival_time, kmax_delay *
     * and front_crit_path_through_pin. */
    int count = 0;
    while (kfalse == stack_is_empty(zero_indegree)) {
        vertex_t* top_elem = stack_pop(zero_indegree);
        stack_push(topo_sort,
                   top_elem);
        ++count;

        /* Then calculate all vertexs' arrival_time and max_delay */
        edge_t* e = top_elem->m_first_edge;
        while (e != NULL) {
            vertex_t* tail = e->m_sink;
            if (--(tail->m_indegree) == 0) {
                stack_push(zero_indegree,
                           tail);
            }

            if (top_elem->m_arrival_time + e->m_delay > tail->m_arrival_time) {
                tail->m_arrival_time = top_elem->m_arrival_time + e->m_delay;
                if (tail->m_arrival_time>*kmax_delay) {
                    *kmax_delay=tail->m_arrival_time;
                }
            }
            e = e->m_next;
        } /* end of while(e != NULL) */
    } /* end of while(kfalse == stack_is_empty(zero_indegree)) */
    return count;
} /* end of int calc_all_vertexs_arrival_time(graph, zero_indegree...) */

int  calc_all_vertexs_arrival_time_by_bfs(const timing_analyze_graph* graph,
                                          queue_t*  queue,
                                          vertex_level_t**  netlist_level,
                                          int*  num_of_levels,
                                          double*  max_delay)
{
    assert(graph != NULL && queue != NULL && netlist_level != NULL);

    int count = 0;
    const int num_of_primary_inputs = graph->m_num_of_primary_inputs;
    vertex_level_t* pri_input_level = init_a_vertex_level(graph->m_primary_inputs,
                                                          num_of_primary_inputs,
                                                          *num_of_levels);
    int v = -1;
    for (v = 0; v < num_of_primary_inputs; ++v) {
        queue_push(queue,
                   graph->m_primary_inputs[v]);
    }
    /* Do not for get assign primary_vertex_level to netlist_level */
    netlist_level[(*num_of_levels)++] = pri_input_level;

    int vertexs_in_cur_level = num_of_primary_inputs;
    int visited_in_cur_level = 0;
    vertex_t*  visited_vertexs_in_next_level[gmax_size];
    int visited_in_next_level = 0;

    while (kfalse == queue_is_empty(queue)) {
        vertex_t* source = queue_pop(queue);
        ++count;

        edge_t* edge = NULL;
        for (edge = source->m_first_edge; edge != NULL; edge = edge->m_next) {
            vertex_t* sink = edge->m_sink;
            const int loc = array_find_element(visited_vertexs_in_next_level,
                                               visited_in_next_level,
                                               sink);
            if (loc == visited_in_next_level) {
                visited_vertexs_in_next_level[visited_in_next_level++] = sink;
            }

            if (source->m_arrival_time + edge->m_delay > sink->m_arrival_time) {
                sink->m_arrival_time = source->m_arrival_time + edge->m_delay;
                if (sink->m_arrival_time > *max_delay) {
                    *max_delay = sink->m_arrival_time;
                }
            }
        } /* end of for(edge = source->m_first_edge; edge != NULL; edge = edge->m_next) */

        ++visited_in_cur_level;
        if (visited_in_next_level != 0
              && visited_in_cur_level == vertexs_in_cur_level) {
            /* All vertexs in current level had visisted. Now initial next *
             * vertex_level_t.                                             */
            vertex_level_t* next_level = init_a_vertex_level(visited_vertexs_in_next_level,
                                                             visited_in_next_level,
                                                             *num_of_levels);
            netlist_level[*num_of_levels] = next_level;
            ++(*num_of_levels);
            /* then set vertexs in next_level... */
            for (v = 0; v < visited_in_next_level; ++v) {
                queue_push(queue,
                           visited_vertexs_in_next_level[v]);
            }
            visited_in_cur_level = visited_in_next_level = 0;
            vertexs_in_cur_level = queue_size(queue);
        } /* end of if(visited_in_next_level != 0 && ...) */
    } /* end of while(kfalse==queue_is_empty(queue)) */
    return count;
} /* end of int calc_all_vertexs_required_time_by_bfs() */

void calc_front_crit_path_through_pin(const timing_analyze_graph* graph,
                                      const double kmax_delay)
{
    const int num_of_vertexs = graph->m_num_of_vertexs;
    int v = -1;
    for (v = 0; v < num_of_vertexs; ++v) {
        vertex_t* source = graph->m_vertexs[v];
        edge_t*   e = source->m_first_edge;
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
        } /* end of while (e != NULL) */
    } /* end of for(v = 0; v < num_of_vertexs; ++v) */
} /* end of void calc_front_crit_path_through_pin() */

void calc_all_vertexs_required_time(const timing_analyze_graph* graph,
                                    const double kmax_delay,
                                    stack_t* topo_sort)
{
    /* 1.initialallvertexs'required_timeaskmax_delay */
    int v = 0;
    while (v < graph->m_num_of_vertexs) {
        graph->m_vertexs[v]->m_required_time = kmax_delay;
        ++v;
    }
    /* 2.calculateallvertexs'srequired_time,slackandbehind_crit_path_through_pin */
    while (kfalse == stack_is_empty(topo_sort)) {
        vertex_t* top_elem = stack_pop(topo_sort);
        edge_t*  e = top_elem->m_first_edge;
        while (e != NULL) {
            vertex_t* tail = e->m_sink;
            if (tail->m_required_time - e->m_delay < top_elem->m_required_time) {
                top_elem->m_required_time = tail->m_required_time - e->m_delay;
            }
            e = e->m_next;
        } /* end of while(e != NULL) */
    } /* end of while(stack_is_empty(topo_sort)==kfalse) */
} /* end of void calc_all_vertexs_required_time(graph,...) */

void calc_all_vertexs_required_time_by_bfs(const timing_analyze_graph* graph,
                                           const double  kmax_delay,
                                           const int  knum_of_levels,
                                           vertex_level_t** netlist_level)
{
    int num_of_vertexs = graph->m_num_of_vertexs;
    int v = -1;
    for (v = 0; v < num_of_vertexs; ++v) {
        graph->m_vertexs[v]->m_required_time = kmax_delay;
    }

    int l = -1;
    for (l = knum_of_levels - 1; l >= 0; --l) {
        vertex_level_t* vertex_level = netlist_level[l];
        num_of_vertexs = vertex_level->m_num_of_vertexs;

        for (v = 0; v < num_of_vertexs; ++v) {
            vertex_t* source = vertex_level->m_vertexs[v];
            edge_t*   e = NULL;
            for (e = source->m_first_edge; e != NULL; e = e->m_next) {
                vertex_t*  sink = e->m_sink;
                if (sink->m_required_time - e->m_delay < source->m_required_time) {
                    source->m_required_time = sink->m_required_time - e->m_delay;
                }
            }
        } /* end of for (v = 0; v < num_of_vertexs; ++v) */
    } /* end of for(l = knum_of_levels-1; l >= 0; --l) */
} /* end of void calc_all_vertexs_required_time_by_bfs() */

void calc_behind_crit_path_through_pin(const timing_analyze_graph* graph,
                                       const double kmax_delay)
{
    const int num_of_vertexs = graph->m_num_of_vertexs;
    int v = -1;
    for (v = 0; v < num_of_vertexs; ++v) {
        vertex_t* source = graph->m_vertexs[v];
        edge_t*   e = source->m_first_edge;
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
        } /* end of while(e != NULL) */
    } /* end of for(v = 0; v < num_of_vertexs; ++v) */
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

            e->m_local_crit_path_through_edge = front_crit_path_through_pin
                                                  * behind_crit_path_through_pin
                                                  * discount_value;

            e = e->m_next; /* then, calculate next edge */
        } /* end of while(e != NULL) */
        ++v;
    } /* end of while(v < num_of_vertexs) */
} /* end of void calc_local_crit_path_through_edge(graph) */

double compute_discount_value(const double x,
                              const double y)
{
    return exp(-x / y); /* e^(-x/y) */
}

/*void find_all_crit_paths_in_graph(const timing_analyze_graph* graph)
{
}*/

void print_all_nets_weight(const timing_analyze_graph* graph)
{
    FILE* output_file = fopen("print_all_nets_weight.log", "w");
    char  line[128];
    char  str[8];
    const int num_of_vertexs = graph->m_num_of_vertexs;

    int v = -1;
    for (v = 0; v < num_of_vertexs; ++v) {
        vertex_t* source = graph->m_vertexs[v];
        edge_t*   edge = NULL;
        for (edge = source->m_first_edge; edge != NULL; edge = edge->m_next) {
            strcpy(line, "Net: ");
            strcat(line, edge->m_name);
            strcat(line, ", ");

            /*source vertex's arrival_time*/
            strcat(line, edge->m_source->m_name);
            strcat(line, " arr_time:");
            sprintf(str, "%f", edge->m_source->m_arrival_time);
            strcat(line, str);

            /*sinkvertex'srequired_time*/
            strcat(line, ", ");
            strcat(line, edge->m_sink->m_name);
            strcat(line, " req_time:");
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
} /* end of void print_all_nets_weight(graph) */

vertex_level_t* init_a_vertex_level(vertex_t* vertexs[gmax_size],
                                    const int num_of_vertexs,
                                    const int level_num)
{
    vertex_level_t* level = (vertex_level_t*)malloc(sizeof(vertex_level_t));
    level->m_num_of_vertexs = num_of_vertexs;
    level->m_vertexs = (vertex_t**)malloc(num_of_vertexs*sizeof(vertex_t*));

    int v = -1;
    for (v = 0; v < num_of_vertexs; ++v) {
        level->m_vertexs[v] = vertexs[v];
    }
    level->m_level_num = level_num;

    return level;
}

void destroy_a_vertex_level(vertex_level_t* vertex_level)
{
    free(vertex_level->m_vertexs);
    vertex_level->m_vertexs = NULL;

    free(vertex_level);
    vertex_level = NULL;
}

int array_find_element(vertex_t* vertexs[gmax_size],
                       const int  klength,
                       const vertex_t*elem)
{
    assert(klength < gmax_size);

    int loc = -1;
    for (loc = 0; loc < klength; ++loc) {
        if (vertexs[loc] == elem) {
            break;
        }
    }
    return loc;
}

void destroy_netlist_level(vertex_level_t** netlist_level,
                           const int knum_of_levels)
{
    int l = -1;
    for (l = 0; l < knum_of_levels; ++l) {
        destroy_a_vertex_level(netlist_level[l]);
    }
    free(netlist_level);
    netlist_level = NULL;
}

