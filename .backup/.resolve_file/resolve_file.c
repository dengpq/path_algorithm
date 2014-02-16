#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "resolve_file.h"

void circuit_create_pins(input_circuit_info* circuit_info,
                          const int num_of_pins)
{
    assert(circuit_info != NULL);

    input_pin_info* input_pins = (input_pin_info*)malloc(sizeof(input_pin_info));
    assert(input_pins != NULL);

    input_pins->m_num_of_pins = num_of_pins;
    circuit_info->m_input_pins = input_pins;
}

void circuit_create_primary_inputs(input_circuit_info* circuit_info,
                                   const int num_of_primary_inputs,
                                   char a_line_words[][8])
{
    assert(circuit_info != NULL);

    int* primary_inputs = (int*)malloc(num_of_primary_inputs * sizeof(int));
    assert(primary_inputs != NULL);

    circuit_info->m_input_pins->m_num_of_primary_inputs = num_of_primary_inputs;
    int i = 0;
    while (i < num_of_primary_inputs) {
        int serial_num = atoi(a_line_words[2+i]);
        primary_inputs[i] = serial_num;
        ++i;
    }

    circuit_info->m_input_pins->m_primary_inputs = primary_inputs;
}

void circuit_create_primary_outputs(input_circuit_info* circuit_info,
                                    const int num_of_primary_outputs,
                                    char a_line_words[][8])
{
    assert(circuit_info != NULL);

    int* primary_outputs = (int*)malloc(num_of_primary_outputs * sizeof(int));
    assert(primary_outputs != NULL);

    circuit_info->m_input_pins->m_num_of_primary_outputs = num_of_primary_outputs;
    int i = 0;
    while (i < num_of_primary_outputs) {
        int serial_num = atoi(a_line_words[2+i]);
        primary_outputs[i] = serial_num;
        ++i;
    }

    circuit_info->m_input_pins->m_primary_outputs = primary_outputs;
}

void circuit_create_nets(input_circuit_info* circuit_info,
                          const int num_of_nets)
{
    assert(circuit_info != NULL);

    input_net_info** input_nets = (input_net_info**)malloc(num_of_nets * sizeof(input_net_info*));
    assert(input_nets!= NULL);

    circuit_info->m_input_nets = input_nets;
    circuit_info->m_num_of_nets = num_of_nets;
}

void circuit_insert_net(input_circuit_info* circuit_info,
                        const int source,
                        const int sink,
                        const int delay,
                        int edge_count)
{
    assert(circuit_info != NULL);

    input_net_info* edge_info = (input_net_info*)malloc(sizeof(input_net_info));
    assert(edge_info != NULL);
    edge_info->m_from = source;
    edge_info->m_to = sink;
    edge_info->m_delay = delay;

    printf("edge_count = %d,edge_info = %p,source = %d,sink = %d,delay = %d\n",
           edge_count,
           edge_info,
           edge_info->m_from,
           edge_info->m_to,
           edge_info->m_delay);
    circuit_info->m_input_nets[edge_count] = edge_info;
}

void destroy_circuit_info(input_circuit_info* circuit_info)
{
    assert(circuit_info != NULL);
    /* first free m_input_pins */
    free(circuit_info->m_input_pins->m_primary_inputs);
    circuit_info->m_input_pins->m_primary_inputs = NULL;

    free(circuit_info->m_input_pins->m_primary_outputs);
    circuit_info->m_input_pins->m_primary_outputs = NULL;

    free(circuit_info->m_input_pins);
    circuit_info->m_input_pins = NULL;

    /* then free m_input_edges */
    printf("Then free all input_nets......\n");
    const int num_of_nets = circuit_info->m_num_of_nets;
    int e = 0;
    while (e < num_of_nets) {
        printf("Now free Net[%d] = %p, source: %d, sink: %d, delay: %d\n", e,
               circuit_info->m_input_nets[e],
               circuit_info->m_input_nets[e]->m_from,
               circuit_info->m_input_nets[e]->m_to,
               circuit_info->m_input_nets[e]->m_delay);
        free(circuit_info->m_input_nets[e]);
        circuit_info->m_input_nets[e] = NULL;
        ++e;
    }

    free(circuit_info->m_input_nets);
    circuit_info->m_input_nets = NULL;
}

input_circuit_info* resolve_file(const char* input_file_name)
{
    FILE* fp = fopen(input_file_name, "r");
    input_circuit_info* circuit_info = (input_circuit_info*)malloc(sizeof(input_circuit_info));
    if (fp != NULL && circuit_info != NULL) {
        int edge_count = 0;
        while (!feof(fp)) {
            const int kmax_len = 100;
            char a_line_string[kmax_len];
            fgets(a_line_string, kmax_len, fp);
            /* then resolve the charactors in a_line_string */
            kbool find_net = resolve_a_line_string(a_line_string,
                                                   circuit_info,
                                                   edge_count);
            if (ktrue == find_net) {
                ++edge_count;
            }
        }
    }

    int status = fclose(fp);
    if (0 == status) {
        printf("Resolve File OK!");
    } else {
        printf("Close input file failure!");
    }

    return circuit_info;
} /* end of resolve_file() */

kbool resolve_a_line_string(const char* a_line_string,
                            input_circuit_info* circuit_info,
                            int edge_count)
{
    const int kmax_words = 65536;
    /*char** a_line_words = (char**)malloc(kmax_size * sizeof(char*));*/
    char a_line_words[kmax_words][8];
    const int klen = strlen(a_line_string);
    const int words = resolve_words_by_dfa(a_line_string,
                                           klen,
                                           a_line_words);

    /* After get all words in a a_line_string, now create input_circuit_info. */
    kbool find_net = kfalse;
    int num_of_nets = -1;
    if (words > 0 && strcmp(a_line_words[0], "PINS") == 0) {
        int num_of_pinss = atoi(a_line_words[1]);
        circuit_create_pins(circuit_info, num_of_pinss);
    } else if (words > 0 && strcmp(a_line_words[0], "PI") == 0) {
        const int num_of_primary_inputs = atoi(a_line_words[1]);
        circuit_create_primary_inputs(circuit_info,
                                      num_of_primary_inputs,
                                      a_line_words);
    } else if (words > 0 && strcmp(a_line_words[0], "PO") == 0) {
        const int num_of_primary_outputs = atoi(a_line_words[1]);
        circuit_create_primary_outputs(circuit_info,
                                       num_of_primary_outputs,
                                       a_line_words);
    } else if (words > 0 && strcmp(a_line_words[0], "NETS") == 0) {
        num_of_nets = atoi(a_line_words[1]);
        circuit_create_nets(circuit_info,
                             num_of_nets);
    } else if (words > 0 && strcmp(a_line_words[0], "N") == 0 && edge_count < num_of_nets) {
        int source = atoi(a_line_words[1]);
        int sink = atoi(a_line_words[2]);
        int delay = atoi(a_line_words[3]);
        circuit_insert_net(circuit_info,
                           source,
                           sink,
                           delay,
                           edge_count);
        find_net = ktrue;
    }
    return find_net;
} /* end of resolve_a_line_string() */

int resolve_words_by_dfa(const char* a_line_string,
                         const int klen,
                         char a_line_words[][8])
{
    state_t state = kbegin_st;
    char input_str[8];
    input_str[0] = '\0';

    int words = 0;
    int i = -1;
    kbool break_loop = kfalse;
    int idx = 0;
    for (i = 0; i < klen; ++i) {
        if (ktrue == break_loop) {
            break;
        }

        char c = a_line_string[i];
        switch (state) {
            case kbegin_st:
                if (isdigit(c) != 0 || isalpha(c) != 0) {
                    input_str[idx++] = c;
                    state = kaccept_st;
                } else if (c == '\n' || c == '#') { /* empty or comment line */
                    state = kfinish_st;
                    break_loop = ktrue;
                    idx = 0;
                } else if (c == ' ') {
                    state = kbegin_st;
                }
                break;
 
            case kaccept_st:
                if (isdigit(c) != 0 || isalpha(c) != 0) {
                    input_str[idx++] = c; /* strcat(input_str, &c); */
                    state = kaccept_st;
                } else if (c == ' ') {
                    input_str[idx] = '\0';
                    strcpy(a_line_words[words++], input_str);
                    state = kbegin_st;
                    input_str[0] = '\0';
                    idx = 0;
                } else if (c == '\n' || c == '#') {
                    input_str[idx++] = '\0';
                    strcpy(a_line_words[words++], input_str);
                    state = kfinish_st;
                    input_str[0] = '\0';
                    idx = 0;
                }
                break;

            case kfinish_st:
                break_loop = ktrue;
                break;
        } /* end of switch(state) */
    } /* end of for(int i = 0; i < klen; ++i) */
    return words;
} /* end of resolve_a_line_words() */

