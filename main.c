#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "graph.h"
#include "path.h"

/* timing_analyze_graph*  initial_graph(); */

int main(int argc, char* argv[])
{
    int return_val = 0;
    /* if (argc == 1 || argc == 2 || (strcmp(argv[1], "-h") == 0
         || strcmp(argv[1], "--help") == 0
         || strcmp(argv[1], "-H") == 0)) {
        printf("Usage: ./path a input_file output_file\n");
        return_val = 0;
    } else { */
        /* first resolve the input file */
        input_circuit_info* circuit_info = resolve_file(argv[1]);
        assert(circuit_info != NULL);
        printf("resolve input circuit file OK!\n");

        /* second initial the DAG and free input_circuit_info */
        timing_analyze_graph* timing_graph = create_timing_analyze_graph(circuit_info);
        printf("Create timing_analyze_graph, timing_graph = %p!\n",
               timing_graph);

        printf("then free circuit_info pointer...\n");
        destroy_circuit_info(circuit_info);
        printf("free circuit_info OK!\n\n");

        /* third find all critical path in DAG, especially the edges that shared *
         * by many critical path. */
        run_path_algorithm(timing_graph);
        printf("Run actual PATH algorithm OK! Then write all net weights...\n");

        print_all_nets_weight(timing_graph);
        printf("Print all nets' weight OK!, then free timing_graph pointer...\n");
        /* last, write the result to output file */
        return_val = 1;
#ifdef DEBUG
        printf("Write circuit_output_file OK! Exit program.\n");
#endif
    /* } */
        printf("Last, don't forget free timing_graph......\n");
        destroy_timing_analyze_graph(timing_graph);
        printf("free timing_graph OK! PATH algorithm finished!\n");
    return return_val;
}

