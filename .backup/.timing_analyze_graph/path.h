#ifndef PATH_H
#define PATH_H

#include "graph.h"

void run_path_algorithm(const timing_analyze_graph* graph);

void calc_local_crit_path_through_edge(const timing_analyze_graph* graph);

double compute_discount_value(const double x, const double y);

#endif

