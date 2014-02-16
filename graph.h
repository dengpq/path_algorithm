#ifndef GRAPH_H
#define GRAPH_H

#include "resolve_file.h"
#include "vertex.h"
#include "edge.h"

typedef struct s_timing_analyze_graph {
    int         m_num_of_vertexs;
    int         m_num_of_primary_inputs;
    int         m_num_of_primary_outputs;

    vertex_t**  m_vertexs;
    /* the 2 following arrays were used for topo-sort and path algorithm. */
    vertex_t**  m_primary_inputs;
    vertex_t**  m_primary_output;
} timing_analyze_graph;

timing_analyze_graph* create_timing_analyze_graph(const input_circuit_info* circuit_info);

void graph_insert_vertexs(const input_circuit_info* circuit_info,
                          timing_analyze_graph* graph);

void graph_set_vertex_name(vertex_t* vertex,
                           const int serial_num);

void graph_insert_edge(const int source,
                       const int sink,
                       const double delay,
                       timing_analyze_graph* graph);

void graph_set_edge_name(edge_t* edge,
                         const vertex_t* source,
                         const vertex_t* sink);

void graph_connect_edge_to_vertex(vertex_t* source,
                                  vertex_t* sink,
                                  edge_t* edge);

void destroy_timing_analyze_graph(timing_analyze_graph* graph);

#endif

