#pragma once

#include "rwr.h"
#include "rwr/source.h"
#include <SDL_timer.h>

typedef struct randrange {
    float min;
    float rlen;
} randrange_t;

typedef struct timerprofile {
    contact_idx_t contact;

    struct {
        randrange_t total;
        randrange_t ping;
        float drop_p;
    } search;

    struct {
        randrange_t total;
        randrange_t ping;
        float missile_p;
        float drop_p;
    } lock;
} timerprofile_t;

/// A single node in a `timerlist`
typedef struct timerlist_node {
    timerprofile_t *profile;
    contact_t *contact;
    struct timerlist_node *next;
    SDL_TimerID timer;
} timerlist_node_t;

/// A linked list of contact timers, contained in a schedule
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
