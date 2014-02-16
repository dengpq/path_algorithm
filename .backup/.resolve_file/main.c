#include <stdio.h>
#include "resolve_file.h"

int main(int argc, char* argv[])
{
    char* input_file = argv[1];
    printf("input_file is: %s\n", input_file);

    input_circuit_info* circuit_info = resolve_file(input_file);

    printf("circuit_info = %p, it had %d pins, %d PIs, %d POs, %d nets.\n",
           circuit_info,
           circuit_info->m_input_pins->m_num_of_pins,
           circuit_info->m_input_pins->m_num_of_primary_inputs,
           circuit_info->m_input_pins->m_num_of_primary_outputs,
           circuit_info->m_num_of_nets);

    /* Now free circuit_info pointer */
    printf("Now free memory...\n");
    destroy_circuit_info(circuit_info);
    printf("Free memory OK!\n");
    return 0;
}

