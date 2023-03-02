#pragma once

#include <rwr/schedule/schedule.h>
#include <rwr/schedule/builder.h>
#include <stdlib.h>

#define S_TO_MS(time) (uint32_t)((float)(time) * 1000.f)

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
        /// Array mapping `rwr_scheduled_contact_t` values to contact instances
        contact_id_t *contacts;
        /// Array mapping `rwr_scheduled_missile_t` values to fired missiles
        fired_missile_t **missiles;
        /// Timer that is running through the schedule events
        SDL_TimerID timer;
        /// The current time in milliseconds
        size_t time;
        size_t event;
    } run;
} rwr_schedule_t;

typedef struct rwr_encounter_builder {
    rwr_schedule_t *schedule;
    rwr_scheduled_contact_t contact;
    
    /// Current time offset to scedule events at
    uint32_t t_offset;
    uint32_t max_time;
} rwr_encounter_builder_t;

/// Used to sort the events list by the time the event occurs
int rwr_schedule_cmp_events(const void *v1, const void *v2);

/// Double the capacity of the events buffer
void rwr_schedule_expand_events(rwr_schedule_t *schedule);

/// Add a new event to the schedule
void rwr_schedule_add_event(rwr_schedule_t *schedule, rwr_schedule_event_t event);

/// Create a new contact to be filled by a new contact event
rwr_scheduled_contact_t rwr_schedule_new_contact(rwr_schedule_t *schedule);

/// Create a new missile entry to be filled by a fired missile event
rwr_scheduled_missile_t rwr_schedule_new_missile(rwr_schedule_t *schedule);
