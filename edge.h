#ifndef EDGE_H
#define EDGE_H

struct vertex_t;

typedef struct s_edge_t {
    char      m_name[8];
    double    m_delay;

    /* the following 3 variable was used for PATH algo */
    double    m_forward_local_slack; /* forward_local_slack(i,j) = Tarrival(j) - Tarrival(i) - delay(i,j)*/
    double    m_backward_local_slack; /* backward_local_slack(i,j) = Trequired(j) - Trequired(i) - delay(i,j)*/
    double    m_slack; /* for slack(i,j) = Trequired(j) - Tarrival(i) - delay(i,j) */
    double    m_local_crit_path_through_edge;
    /* Do not use vertex_t*, use struct s_vertex_t*  */
    struct s_vertex_t* m_source;
    struct s_vertex_t* m_sink;
    struct s_edge_t*   m_next; /* <source, ?>*/
} edge_t;

#endif

