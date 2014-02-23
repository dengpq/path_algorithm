#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "vertex.h"
#include "queue.h"

queue_t* create_queue()
{
    queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
    vertex_t** elems = (vertex_t**)malloc(gmax_size * sizeof(vertex_t*));
    assert(queue != NULL && elems != NULL);

    queue->m_elements = elems;
    queue->m_capacity = gmax_size;
    queue->m_size = 0;
    queue->m_front = queue->m_end = NULL;

    return queue;
}

void destroy_queue(queue_t* queue)
{
    assert(queue != NULL);
    free(queue->m_elements);
    queue->m_elements = NULL;
    queue->m_front = queue->m_end = NULL;
}

int queue_size(const queue_t* queue)
{
    assert(queue != NULL);
    return queue->m_size;
}

bool_t queue_is_empty(const queue_t* queue)
{
    assert(queue != NULL);
    bool_t return_val = ktrue;
    if (queue_size(queue) > 0) {
        return_val = kfalse;
    } else {
        return_val = ktrue;
    }
    return return_val;
}

void queue_push(queue_t* queue,
                vertex_t* elem)
{
    assert(queue != NULL && elem != NULL);
    if (queue->m_size == queue->m_capacity - 1) {
        queue->m_elements = (vertex_t**)realloc(queue->m_elements,
                                                2 * queue->m_capacity * sizeof(vertex_t*));
        queue->m_capacity *= 2;
    }

    queue->m_elements[queue->m_size++] = elem;
    queue->m_end = elem;
    if (queue->m_size == 1) {
        queue->m_front = queue->m_end;
    }
}

vertex_t* queue_pop(queue_t* queue)
{
    vertex_t* elem = NULL;
    const int klength = queue->m_size;
    if (klength > 0) {
        elem = queue->m_front;
        int e = 0;
        for (e = 1; e <= klength - 1; ++e) {
            queue->m_elements[e-1] = queue->m_elements[e];
        }
        queue->m_front = queue->m_elements[0];
        queue->m_elements[klength-1] = NULL;
        --queue->m_size;
    }
    return elem;
}

vertex_t* queue_front(const queue_t* queue)
{
    assert(queue!= NULL && queue->m_front != NULL);
    return queue->m_front;
}

vertex_t* queue_end(const queue_t* queue)
{
    assert(queue != NULL && queue->m_end != NULL);
    return queue->m_end;
}

int queue_find_elem(const queue_t*  queue,
                    const vertex_t* v)
{
    int loc = -1;
    for (loc = 0; loc < queue->m_size; ++loc) {
        if (v == queue->m_elements[loc]) {
            break;
        }
    }
    return loc;
}

