#include <stdlib.h>
#include <assert.h>

#include "graph.h"
#include "../resolve_file.h"

timing_analyze_graph*  create_timing_analyze_graph(const input_circuit_info* circuit_info)
{
    assert(circuit_info != NULL);

    timing_analyze_graph* graph = (timing_analyze_graph*)malloc(sizeof(timing_analyze_graph));
    assert(graph != NULL);
    /* first insert vertexs */
    graph_insert_vertexs(circuit_info, graph);
    /* then insert edge */
    const int num_of_edges = circuit_info->m_num_of_edges;
    int e = 0;
    while (e < num_of_edges) {
        int source = circuit_info->m_input_edges[e]->m_from;
        int sink = circuit_info->m_input_edges[e]->m_to;
        double delay = circuit_info->m_input_edges[e]->m_delay;
        graph_insert_edge(source,
                          sink,
                          delay,
                          graph);
        ++e;
    }

    return graph;
}

void graph_insert_vertexs(const input_circuit_info* circuit_info,
                          timing_analyze_graph* graph)
{
    /* first create vertexs, primary_inputs and primary_outputs arrays */
    const int num_of_vertexs = circuit_info->m_input_pins->m_num_of_pins;
    const int num_of_primary_inputs = circuit_info->m_input_pins->m_num_of_primary_inputs;
    const int num_of_primary_outputs = circuit_info->m_input_pins->m_num_of_primary_outputs;
    graph->m_num_of_vertexs = num_of_vertexs;
    graph->m_vertexs = (vertex_t**)malloc(num_of_vertexs * sizeof(vertex_t*));
    graph->m_primary_inputs = (vertex_t**)malloc(num_of_primary_inputs
                                                 * sizeof(vertex_t*));
    graph->m_primary_output = (vertex_t**)malloc(num_of_primary_outputs
                                                 * sizeof(vertex_t*));
    /* second insert vertexs into array */
    int v = 0;
    while (v < num_of_vertexs) {
        vertex_t* vertex = (vertex_t*)malloc(sizeof(vertex_t));
        char name[8];
        vertex->m_arrival_time = 0.0;
        vertex->m_required_time = 0.0;
        vertex->m_first_edge = NULL;
        vertex->m_last_edge = NULL;
        vertex->m_indegree = 0;
        ++v;
        graph->m_vertexs[v] = vertex;
    }
    /* last insert primary inputs and outputs */
    v = 0;
    while (v < num_of_primary_inputs) {
        const int serial_num = circuit_info->m_input_pins->m_primary_inputs[v];
        graph->m_primary_inputs[v] = graph->m_vertexs[serial_num];
        ++v;
    }
    v = 0;
    while (v < num_of_primary_outputs) {
        const int serial_num = circuit_info->m_input_pins->m_primary_outputs[v];
        graph->m_primary_output[v] = graph->m_vertexs[serial_num];
        ++v;
    }
}

void graph_insert_edge(const int source,
                       const int sink,
                       const double delay,
                       timing_analyze_graph* graph)
{
    edge_t* edge = (edge_t*)malloc(sizeof(edge_t));
    char name[16];
    edge->m_source = graph->m_vertexs[source];
    edge->m_sink = graph->m_vertexs[sink];
    edge->m_delay = delay;
    edge->m_forward_local_slack = 0.0;
    edge->m_backward_local_slack = 0.0;
    edge->m_local_crit_path_through_edge = 0.0;
    edge->m_next = NULL;
    graph_connect_edge_to_vertex(graph->m_vertexs[source],
                                 graph->m_vertexs[sink],
                                 edge);
}

void graph_connect_edge_to_vertex(vertex_t* source,
                                  vertex_t* sink,
                                  edge_t* edge)
{
    if (source->m_first_edge == NULL) {
        source->m_first_edge = edge;
        source->m_last_edge = edge;
    } else {
        source->m_last_edge->m_next = edge;
        source->m_last_edge = edge;
    }
    /* Attention, it should increase sink vertex's indgree */
    ++(sink->m_indegree);
}

void destroy_timing_analyze_graph(timing_analyze_graph* graph)
{
    const int num_of_vertexs = graph->m_num_of_vertexs;
    int v = 0;
    for (; v < num_of_vertexs; ++v) {
        vertex_t* vertex = graph->m_vertexs[v];
        /* fist delete all edges connect to the vertex */
        edge_t* edge = vertex->m_first_edge;
        while (edge != NULL) {
            edge_t* next = edge->m_next;

            free(edge);
            edge = NULL;

            edge = next;
        }
        /* then delete this vertex! */
        free(vertex);
        vertex = NULL;
    }
}

