#ifndef TREE_H
#define TREE_H

typedef struct s_vertex_t vertex_t;

typedef struct s_tree_t {
    struct s_tree_node_t*  m_root;
    struct s_tree_node_t** m_elems; /* tree_node_t* elems[num_of_nodes]; */
    int    m_num_of_nodes;
} tree_t;

typedef struct s_tree_node_t {
    struct s_vertex_t*      m_from_vertex;
    struct s_tree_node_t**  m_target_nodes;
    int    m_num_of_nodes;
} tree_node_t;

#endif

