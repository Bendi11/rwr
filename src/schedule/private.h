#pragma once

#include <rwr/schedule/schedule.h>
#include <stdlib.h>

/// A schedule of events that happen to an RWR on a given timespan
typedef struct rwr_schedule {
    /// Dynamic array of events, unsorted until the schedule runs
    struct {
        rwr_schedule_event_t *array;
        size_t cap;
        size_t len;
    } events;

    rwr_scheduled_contact_t contacts;
    rwr_scheduled_missile_t missiles;
    
    /// All state that is populated when the schedule is run on an RWR model
    struct {
        alr56_t *rwr;

        size_t event;
    } run;
} rwr_schedule_t;


/// Double the capacity of the events buffer
void rwr_schedule_expand_events(rwr_schedule_t *schedule);
