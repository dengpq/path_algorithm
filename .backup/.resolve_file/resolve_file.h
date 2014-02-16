#ifndef RESOLVE_FILE_H
#define RESOLVE_FILE_H

struct FILE;

typedef enum {
    kfalse = 0,
    ktrue 
} kbool;

typedef enum e_state_t {
    /* state was initial was kbegin_state, when program read " ", it keep kbegin_state */
    kbegin_st = 1,

    /* when read alpha or digit, the state change to kaccept_state */
    kaccept_st,

    /* when read the '\n' or '#', the state change to kfinish_state */
    kfinish_st
} state_t;

typedef struct s_pin_info {
    int  m_num_of_pins;
    int  m_num_of_primary_inputs;
    int  m_num_of_primary_outputs;
    int* m_primary_inputs;
    int* m_primary_outputs;
} input_pin_info;

typedef struct s_edge_info {
    int m_from; /* source vertex */
    int m_to;   /* sink vertex */
    int m_delay;
} input_net_info;

typedef struct s_circuit_info {
    input_pin_info*   m_input_pins;
    input_net_info**  m_input_nets; /* input_net_info* m_input_nets[m_num_of_nets] */
    int               m_num_of_nets;
} input_circuit_info;

/* create input_vertex, and set num_of_vertexs */
void circuit_create_pins(input_circuit_info* circuit_info,
                         const int num_of_vertexs);

void circuit_create_primary_inputs(input_circuit_info* circuit_info,
                                   const int num_of_primary_inputs,
                                   char a_line_words[][8]);

void circuit_create_primary_outputs(input_circuit_info* circuit_info,
                                    const int num_of_primary_outputs,
                                    char a_line_words[][8]);

/* initial the size of m_input_edges */
void circuit_create_nets(input_circuit_info* circuit_info,
                          const int num_of_edges);

/* insert a m_input_info to m_input_edges */
void circuit_insert_net(input_circuit_info* circuit_info,
                         const int source,
                         const int sink,
                         const int delay,
                         int edge_count);

void destroy_circuit_info(input_circuit_info* circuit_info);

/*----------------------------------------------------------*/
/* resolve the input file, and return circuit_info pointer*/
input_circuit_info* resolve_file(const char* input_file_name);

/* resolve a line string in file, and insert info into graph */
kbool resolve_a_line_string(const char* line,
                           input_circuit_info* circuit_info,
                           int edge_count);

/* char a_line_words[kmax_words][kmax_len], it will return the number of  words that had found. */
int resolve_words_by_dfa(const char* line,
                         const int klen,
                         char a_line_words[][8]);

#endif

