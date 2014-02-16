#ifndef VERTEX_H
#define VERTEX_H

typedef struct s_vertex_t {
    char      m_name[8];
    /* the following 4 variables were used for PATH algo */
    double    m_required_time;
    double    m_arrival_time;
    double    m_front_crit_path_through_pin;
    double    m_behind_crit_path_through_pin;
    /* Do not use edge_t*, use struct s_edge_t*  */
    struct s_edge_t* m_first_edge;
    /* In order to connect edges to vertex quickly, I new added last_edge pointer */
    struct s_edge_t* m_last_edge;
    int       m_indegree; /* used for topo-sort and critical path algorithm */
} vertex_t;

#endif

