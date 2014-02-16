#ifndef GRAPH_H
#define GRAPH_H

#include "../resolve_file.h"

struct s_edge_t;

typedef struct s_vertex_t {
    char      m_name[8];
    double    m_required_time;
    double    m_arrival_time;
    s_edge_t* m_first_edge;
    /* In order to connect edges to vertex quickly, I new added last_edge pointer */
    s_edge_t* m_last_edge;
    int       m_indegree; /* used for topo-sort and critical path algorithm */
} vertex_t;

typedef struct s_edge_t {
    char      m_name[8];
    double    m_delay;
    double    m_forward_local_slack;
    double    m_backward_local_slack;
    double    m_local_crit_path_through_edge;
    vertex_t* m_source;
    vertex_t* m_sink;
    s_edge_t* m_next; /* <source, ?>*/
} edge_t;

typedef struct s_timing_analyze_graph {
    int         m_num_of_vertexs;
    vertex_t**  m_vertexs;
    vertex_t**  m_primary_inputs;
    vertex_t**  m_primary_output;
} timing_analyze_graph;

timing_analyze_graph* create_timing_analyze_graph(const input_circuit_info* circuit_info);

void graph_insert_vertexs(const input_circuit_info* circuit_info,
                          timing_analyze_graph* graph);

void graph_insert_edge(const int source,
                       const int sink,
                       const int delay,
                       timing_analyze_graph* graph);

void destroy_timing_analyze_graph(timing_analyze_graph* graph);

void graph_connect_edge_to_vertex(vertex_t* source,
                                  vertex_t* sink,
                                  edge_t* edge);

#endif

