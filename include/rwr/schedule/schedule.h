#pragma once

#include "rwr/model/alr56.h"

/// A random range between two numbers
typedef struct randrange {
    float min;
    float max;
} randrange_t;

typedef struct rand_location {
    randrange_t bearing;
    randrange_t altitude;
    randrange_t distance;
} rand_location_t;

/// A collection of parameters used to generate a random RWR encounter
typedef struct timerprofile {
    contact_idx_t contact;
    /// Initial delay before search radar pings
    randrange_t initial;
    
    rand_location_t initial_pos;
    rand_location_t movement;

    struct {
        randrange_t total;
        randrange_t ping;
        float drop_p;
    } search;

    struct {
        randrange_t ping;
        randrange_t missile_duration;
        float missile_p;
        float drop_p;
        uint8_t max_missiles;
    } lock;
} timerprofile_t;

typedef struct schedule_timer schedule_timer_t;

/// An RWR contact schedule made up of multiple timers
typedef struct schedule {
    schedule_timer_t *root;
    alr56_t *rwr;
} schedule_t;


/// Create a new `schedule_t` with an empty list
void schedule_new(schedule_t *list);

/// Add a new contact timer to the RWR schedule
schedule_timer_t* schedule_add(schedule_t *list, const timerprofile_t *prof);

/// Remove the given timer from this list, stopping the contained timer if required
void schedule_remove(schedule_t *list, schedule_timer_t *node);

/// Free all timerlist nodes of the given timer list, stopping their timers in the process
void schedule_free(schedule_t *list);
