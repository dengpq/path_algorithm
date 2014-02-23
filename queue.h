#ifndef QUEUE_H
#define QUEUE_H

#include "global.h"

typedef struct s_vertex_t vertex_t;

typedef struct s_queue_t {
    struct s_vertex_t**  m_elements;
    int    m_capacity;
    int    m_size;
    struct s_vertex_t* m_front;
    struct s_vertex_t* m_end;
} queue_t;

queue_t* create_queue();

void destroy_queue(queue_t* queue);

int  queue_size(const queue_t*  queue);

bool_t queue_is_empty(const queue_t* queue);

void queue_push(queue_t* queue,
                vertex_t* v);

vertex_t* queue_pop(queue_t* queue);

vertex_t* queue_front(const queue_t* queue);

vertex_t* queue_back(const queue_t* queue);

int queue_find_elem(const queue_t* queue,
                    const vertex_t* v);

#endif

