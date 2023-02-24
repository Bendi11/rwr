#pragma once

#include "rwr/model/alr56.h"
#include <math.h>

/// A random range between two numbers
typedef struct randrange {
    float min;
    float max;
} randrange_t;

/// A collection of random bounds used to generate a random location
typedef struct rand_location {
    randrange_t bearing;
    randrange_t altitude;
    randrange_t distance;
} rand_location_t;

/// A collection of parameters used to generate a random RWR encounter
typedef struct timerprofile {
    contact_idx_t contact;
    /// Initial delay before search radar pings
    randrange_t initial_delay;
    rand_location_t initial_pos;
    /// By how much the location may change for each ping
    rand_location_t movement;
    
    /// All random parameters relating to the search phase of the encounter
    struct {
        randrange_t total;
        randrange_t ping;
        float drop_p;
    } search;
    
    /// All random parameters relating to the locking phase of the encounter
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
void schedule_new(schedule_t *list, alr56_t *rwr);

/// Add a new contact timer to the RWR schedule
schedule_timer_t* schedule_add(schedule_t *list, const timerprofile_t *prof);

/// Remove the given timer from this list, stopping the contained timer if required
void schedule_remove(schedule_t *list, schedule_timer_t *node);

/// Free all timerlist nodes of the given timer list, stopping their timers in the process
void schedule_free(schedule_t *list);

const extern timerprofile_t SA10_PROF;
const extern timerprofile_t F16_PROF;
