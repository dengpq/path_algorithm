#ifndef VERTEX_H
#define VERTEX_H

struct edge_t;
struct edge_info;

typedef struct s_vertex_t {
    char    m_name[8];
    double  m_required_time;
    double  m_arrival_time;
    edge_t* m_first_edge;
    /* In order to connect edges to vertex quickly, I new added last_edge pointer */
    edge_t* m_last_edge;
    int     m_indegree; /* used for topo-sort and critical path algorithm */
} vertex_t;

void edge_connect_to_vertex(const edge_info info, vertex_t* target);

#endif

