#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "stack.h"
#include "vertex.h"

stack_t*  create_stack()
{
    stack_t* stack = (stack_t*)malloc(sizeof(stack_t));
    vertex_t** elems = (vertex_t**)malloc(gmax_size * sizeof(vertex_t*));
    assert(stack != NULL && elems != NULL);

    stack->m_elements = elems;
    stack->m_capacity = gmax_size;
    stack->m_top = -1;

    return stack;
}

void destroy_stack(stack_t* stack)
{
    free(stack->m_elements);
    stack->m_elements = NULL;
    free(stack);
    stack = NULL;
}

bool_t stack_is_empty(stack_t* stack)
{
    bool_t return_val = kfalse;
    if (stack->m_top < 0) {
        return_val = ktrue;
    }

    return return_val;
}

void stack_push(stack_t* stack, vertex_t* elem)
{
    assert(stack != NULL && elem != NULL);

    if (stack->m_top == stack->m_capacity - 1) {
        stack->m_elements = (vertex_t**)realloc(stack->m_elements,
                                                2 * stack->m_capacity * sizeof(vertex_t*));
        stack->m_capacity *= 2;
    }

    stack->m_elements[++(stack->m_top)] = elem;
}

vertex_t* stack_pop(stack_t* stack)
{
    vertex_t* top_elem = stack->m_elements[(stack->m_top)--];
    return top_elem;
}

int stack_length(stack_t* stack)
{
    return stack->m_top;
}
