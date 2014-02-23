#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "graph.h"
#include "path.h"

int main(int argc, char* argv[])
{
    int return_val = 0;
    if (argc == 1 || (argc == 2 && (strcmp(argv[1], "-h") == 0
         || strcmp(argv[1], "--help") == 0
         || strcmp(argv[1], "-H") == 0))) {
        printf("Usage: ./path input_file algo_type. algo_type: -topo_sort or -bfs\n");
        return_val = 0;
    } else {
        /* first resolve the input file */
        clock_t start = clock();
        input_circuit_info* circuit_info = resolve_file(argv[1]);
        assert(circuit_info != NULL);
        clock_t finish_resolve_file = clock();
        double  cost_time = (finish_resolve_file - start) / CLOCKS_PER_SEC * 1000;
        printf("resolve input circuit file OK! It cost %f ms.\n",
               cost_time);

        /* second initial the DAG and free input_circuit_info */
        timing_analyze_graph* timing_graph = create_timing_analyze_graph(circuit_info);
        clock_t finish_create_graph = clock();
        cost_time = (finish_create_graph - finish_resolve_file) / CLOCKS_PER_SEC * 1000;
        printf("Create timing_analyze_graph OK! It cost %f ms.\n",
               cost_time);

        destroy_circuit_info(circuit_info);
        printf("free circuit_info OK!\n");

        /* third find all critical path in DAG, especially the edges that shared *
         * by many critical path. */
        char algo_type[12];
        if (argc == 2 || (argc == 3 && (strcmp(argv[2], "-topo_sort") == 0
                           || strcmp(argv[2], "topo_sort") == 0))) {
            strcpy(algo_type, "topo_sort");
        } else if (argc == 3 && (strcmp(argv[2], "-bfs") == 0
                    || strcmp(argv[2], "bfs") == 0)) {
            strcpy(algo_type, "bfs");
        }

        run_path_algorithm(timing_graph, algo_type);
        clock_t finish_run_path = clock();
        cost_time = (finish_run_path - finish_create_graph) / CLOCKS_PER_SEC * 1000;
        printf("Run actual PATH algorithm OK! It cost %f ms. Then write all net weights...\n",
               cost_time);

        print_all_nets_weight(timing_graph);
        printf("Print all nets' weight OK!, then free timing_graph pointer...\n");

        destroy_timing_analyze_graph(timing_graph);
        printf("free timing_graph OK! PATH algorithm finished!\n");
        /* last, write the result to output file */
        return_val = 1;
        clock_t  finish = clock();
        cost_time = (finish - start) / CLOCKS_PER_SEC * 1000;
        printf("Program finished! It totally cost %f ms.\n",
               cost_time);
    }
    return return_val;
}

