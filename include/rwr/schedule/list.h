#pragma once

#include <SDL_timer.h>

/// A single node in a `timerlist`
typedef struct timerlist_node {
    SDL_TimerID timer;
    struct timerlist_node *prev;
    struct timerlist_node *next;
} timerlist_node_t;

/// A doubly linked list of SDL_TimerID's
typedef struct timerlist {
    timerlist_node_t *root;
} timerlist_t;

/// Create a new `timerlist_t` with an empty list
void timerlist_new(timerlist_t *list);

/// Add a new timer to the linked list
timerlist_node_t* timerlist_add(timerlist_t *list, SDL_TimerID id);

/// Remove the given timer from this list, stopping the contained timer if required
void timerlist_remove(timerlist_t *list, timerlist_node_t *node);

/// Free all timerlist nodes of the given timer list, stopping their timers in the process
void timerlist_free(timerlist_t *list);
