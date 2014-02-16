#ifndef EDGE_H
#define EDGE_H

struct vertex_t;

typedef struct s_edge_t {
    char      m_name[8];
    double    m_delay;
    double    m_forward_local_slack;
    double    m_backward_local_slack;
    double    m_local_crit_path_through_edge;
    vertex_t* m_source;
    vertex_t* m_sink;
    s_edge_t* m_next;
} edge_t;

#endif

