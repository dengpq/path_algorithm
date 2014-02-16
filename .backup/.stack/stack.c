#include <stdlib.h>
#include <assert.h>

#include "stack.h"

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
}

kbool stack_is_empty(stack_t* stack)
{
    kbool return_val = kfalse;
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

kbool stack_pop(stack_t* stack, vertex_t* elem)
{
    kbool return_val = ktrue;
    if (stack->m_top < 0) { /* stack is empty */
        return_val = kfalse;
        elem = NULL;
    } else {
        elem = stack->m_elements[(stack->m_top)--];
    }
    return return_val;
}

int stack_length(stack_t* stack)
{
    return stack->m_top;
}
