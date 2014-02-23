#ifndef PATH_H
#define PATH_H

#include "graph.h"
#include "stack.h"
#include "queue.h"

typedef struct s_vertex_level_t {
    struct s_vertex_t**  m_vertexs;
    int    m_num_of_vertexs;
    int    m_level_num;
} vertex_level_t;

void run_path_algorithm(const timing_analyze_graph* graph,
                        const char* algo_type);

/* 1.this function will initialize all vertexs' front_crit_path_through_pin *
 * and behind_crit_path_through_pin.                                        */
void init_all_vertexs_value(const timing_analyze_graph* graph);

/* 2.calculate all vertexs' arrival_time, and return the number of vertex *
 * after topo sorting. If the count was less than num_of_vertexs, it must *
 * have circle in graph, so exit the function.                            */
int  calc_all_vertexs_arrival_time(const timing_analyze_graph* graph,
                                   stack_t* zero_indegree,
                                   stack_t* topo_sort,
                                   double* max_delay);

int  calc_all_vertexs_arrival_time_by_bfs(const timing_analyze_graph* graph,
                                          queue_t*  queue,
                                          vertex_level_t** netlist_level,
                                          int*  num_of_levels,
                                          double* max_delay);

/* 3.calculate all vertexs' front_crit_path_through_pin */
void calc_front_crit_path_through_pin(const timing_analyze_graph* graph,
                                      const double kmax_delay);

/* 4.calcualte all vertexs' required_time by reverse topo order */
void calc_all_vertexs_required_time(const timing_analyze_graph* graph,
                                    const double kmax_delay,
                                    stack_t* topo_sort);

void calc_all_vertexs_required_time_by_bfs(const timing_analyze_graph* graph,
                                           const double kmax_delay,
                                           const int  knum_of_levels,
                                           vertex_level_t**  netlist_level);

/* 5.calcuate all vertexs' behind_crit_path_through_pin */
void calc_behind_crit_path_through_pin(const timing_analyze_graph* graph,
                                       const double kmax_delay);

/* 6. */
void calc_local_crit_path_through_edge(const timing_analyze_graph* graph,
                                       const double kmax_delay);

double compute_discount_value(const double x,
                              const double y);

void  print_all_nets_weight(const timing_analyze_graph* graph);

void  find_all_crit_paths_in_graph(const timing_analyze_graph* graph);

vertex_level_t*  init_a_vertex_level(vertex_t* vertexs[gmax_size],
                                     const int num_of_vertexs,
                                     const int level_num);

void  destroy_a_vertex_level(vertex_level_t* vertex_level);

int  array_find_element(vertex_t* vertexs[gmax_size],
                        const int length,
                        const vertex_t*  elem);

void  destroy_netlist_level(vertex_level_t** netlist_level,
                            const int knum_of_levels);

#endif

