#ifndef STACK_H
#define STACK_H

struct vertex_t;

const int gmax_size = 1024;

typedef enum {
    kfalse = 0,
    ktrue
} kbool;

/* this stack's capacity can double when it was full. */
typedef struct s_stack_t {
    vertex_t** m_elements; /* vertex_t* elements[gmax_size] */
    int        m_capacity; /* when stack was full, call realloc() to double its capacity */
    int        m_top; /* stack top pointer, it initial as -1 */
} stack_t;

stack_t* create_stack();

void  destroy_stack(stack_t* s);

kbool stack_is_empty(stack_t* s);

void stack_push(stack_t* s,
                vertex_t* element);

vertex_t* stack_pop(stack_t* s);

int  stack_length(stack_t* s);

#endif

