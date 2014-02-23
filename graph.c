#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "graph.h"

timing_analyze_graph*  create_timing_analyze_graph(const input_circuit_info* circuit_info)
{
    assert(circuit_info != NULL);

    timing_analyze_graph* graph = (timing_analyze_graph*)malloc(sizeof(timing_analyze_graph));
    assert(graph != NULL);
    /* first insert all vertexs */
    graph_insert_vertexs(circuit_info, graph);
    printf("graph insert all vertexs OK!\n");

    /* then insert all edges */
    const int num_of_nets = circuit_info->m_num_of_nets;
    int e = -1;
    for (e = 0; e < num_of_nets; ++e) {
        graph_insert_edge(circuit_info->m_input_nets[e]->m_from,
                          circuit_info->m_input_nets[e]->m_to,
                          circuit_info->m_input_nets[e]->m_delay,
                          graph);
    }
    printf("graph insert all edges OK!\n");
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
    graph->m_num_of_primary_inputs = num_of_primary_inputs;
    graph->m_num_of_primary_outputs = num_of_primary_outputs;
    graph->m_vertexs = (vertex_t**)malloc(num_of_vertexs * sizeof(vertex_t*));
    graph->m_primary_inputs = (vertex_t**)malloc(num_of_primary_inputs
                                                   * sizeof(vertex_t*));
    graph->m_primary_outputs = (vertex_t**)malloc(num_of_primary_outputs
                                                    * sizeof(vertex_t*));
    /* second insert vertexs into array */
    int v = -1;
    for (v = 0; v < num_of_vertexs; ++v) {
        vertex_t* vertex = (vertex_t*)malloc(sizeof(vertex_t));

        graph_set_vertex_name(vertex, v);
        vertex->m_arrival_time = 0.0;
        vertex->m_required_time = 0.0;
        vertex->m_front_crit_path_through_pin = 0.0;
        vertex->m_behind_crit_path_through_pin = 0.0;
        vertex->m_first_edge = NULL;
        vertex->m_last_edge = NULL;
        vertex->m_indegree = 0;

        graph->m_vertexs[v] = vertex;
    }
    /* last insert primary inputs and outputs */
    int serial_num = -1;
    for (v = 0; v < num_of_primary_inputs; ++v) {
        serial_num = circuit_info->m_input_pins->m_primary_inputs[v];
        graph->m_primary_inputs[v] = graph->m_vertexs[serial_num];
    }
    for (v = 0; v < num_of_primary_outputs; ++v) {
        serial_num = circuit_info->m_input_pins->m_primary_outputs[v];
        graph->m_primary_outputs[v] = graph->m_vertexs[serial_num];
    }
} /* end of void graph_insert_vertexs() */

void graph_set_vertex_name(vertex_t* vertex,
                           const int serial_num)
{
    char name[8] = "";
    strcpy(name, "v");
    char str[8];
    sprintf(str, "%d", serial_num);
    strcat(name, str); /* example: v10 */
    strcpy(vertex->m_name, name);
}

void graph_insert_edge(const int source,
                       const int sink,
                       const double delay,
                       timing_analyze_graph* graph)
{
    edge_t* edge = (edge_t*)malloc(sizeof(edge_t));
    edge->m_source = graph->m_vertexs[source];
    edge->m_sink = graph->m_vertexs[sink];
    graph_set_edge_name(edge,
                        edge->m_source,
                        edge->m_sink);
    edge->m_delay = delay;
    edge->m_forward_local_slack = 0.0;
    edge->m_backward_local_slack = 0.0;
    edge->m_slack = -1.0;
    edge->m_local_crit_path_through_edge = 0.0;
    edge->m_next = NULL;

    graph_connect_edge_to_vertex(graph->m_vertexs[source],
                                 graph->m_vertexs[sink],
                                 edge);
} /* end of void graph_insert_edge() */

void graph_set_edge_name(edge_t* edge,
                         const vertex_t* source,
                         const vertex_t* sink)
{
    char edge_name[16] = "";
    strcpy(edge_name, source->m_name);
    strcat(edge_name, "->");
    strcat(edge_name, sink->m_name);
    strcpy(edge->m_name, edge_name);
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
} /* end of void graph_connect_edge_to_vertex() */

void destroy_timing_analyze_graph(timing_analyze_graph* graph)
{
    const int num_of_vertexs = graph->m_num_of_vertexs;
    int v = 0;
    for (; v < num_of_vertexs; ++v) {
        vertex_t* vertex = graph->m_vertexs[v];
        /* first delete all edges connect to the vertex */
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
} /* end of void destroy_timing_analyze_graph() */

