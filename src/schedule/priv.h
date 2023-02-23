#pragma once

#include "rwr/schedule/schedule.h"


/// A single node in a `schedule` that acts out one RWR contact
typedef struct schedule_timer {
    struct schedule *schedule;
    const timerprofile_t *profile;
    contact_t *contact;
    struct schedule_timer *next;
    SDL_TimerID timer;
    /// Time that has passed since the last transition (search->lock, start->search)
    float time;
    /// A randomly generated time that will determine when the next transition occurs
    float time_goal;
} schedule_timer_t;

/// Timer callback executed for each timer list node
unsigned int schedule_timer_cb(unsigned int time, void *vnode);

/// Create a new timer list node from a profile
schedule_timer_t *schedule_timer_new(schedule_t *schedule, const timerprofile_t *prof);

/// Generate a random location from a collection of random ranges
location_t rand_location(rand_location_t loc);

/// Generate a random number in the range described by `range`
float rand_range(randrange_t range);
