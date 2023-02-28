#include "rwr/schedule/schedule.h"
#include "rwr/model/alr56.h"
#include "private.h"
#include <stdlib.h>

rwr_schedule_t* rwr_schedule_new(void) {
    rwr_schedule_t *schedule = malloc(sizeof(*schedule));
    schedule->missiles = 0;
    schedule->contacts = 0;

    schedule->events.cap = 8;
    schedule->events.len = 0;
    schedule->events.array = calloc(schedule->events.cap, sizeof(*schedule->events.array));

    schedule->run.rwr = NULL;
    return schedule;
}

rwr_scheduled_contact_t rwr_schedule_new_contact(rwr_schedule_t *schedule) { return schedule->contacts++; }
rwr_scheduled_missile_t rwr_schedule_new_missile(rwr_schedule_t *schedule) { return schedule->missiles++; }

unsigned int rwr_schedule_timer_cb(unsigned int time, void *vparam) {
    rwr_schedule_t *schedule = (rwr_schedule_t*)vparam;
    if(schedule->run.event == schedule->events.len) { return 0; }

    rwr_schedule_event_t *ev = &schedule->events.array[schedule->run.event];
    schedule->run.event += 1;
    schedule->run.time += time;
    
    switch(ev->tag) {
        case RWR_SCHEDULE_EVENT_NEWGUY: {
            if(schedule->run.contacts[ev->contact] != NULL) {
                fputs("Attempt to create a new contact in schedule when contact is already initialized", stderr);
                return 0;
            }

            schedule->run.contacts[ev->contact] = alr56_newguy(
                schedule->run.rwr,
                ev->newguy.source,
                ev->newguy.loc
            );
        } break;

        case RWR_SCHEDULE_EVENT_PAINT: {
            schedule->run.contacts[ev->contact] = alr56_ping(
                schedule->run.rwr,
                schedule->run.contacts[ev->contact],
                ev->paint.loc
            );
        } break;

        case RWR_SCHEDULE_EVENT_DROP: {
            alr56_drop(schedule->run.rwr, schedule->run.contacts[ev->contact]);
            schedule->run.contacts[ev->contact] = NULL;
        }
    }

    return schedule->events.array[schedule->run.event].time_ms - schedule->run.time;
}

void rwr_schedule_run(rwr_schedule_t *schedule, alr56_t *rwr) {
    rwr_schedule_stop(schedule);
    schedule->run.rwr = rwr;
    schedule->run.event = 0;
     
}

void rwr_schedule_stop(rwr_schedule_t *schedule) {
    if(schedule->run.rwr != NULL) { return; }
    
    SDL_RemoveTimer(schedule->run.timer);
    schedule->run.rwr = NULL;
    for(size_t i = 0; i < schedule->events.len; ++i) {
    
    }
}

void rwr_schedule_add_event(rwr_schedule_t *schedule, rwr_schedule_event_t event) {
    if(schedule->events.cap == schedule->events.len) {
        rwr_schedule_expand_events(schedule);
    }

    schedule->events.array[schedule->events.len] = event;
    schedule->events.len += 1;
}

void rwr_schedule_expand_events(rwr_schedule_t *schedule) {
    schedule->events.cap <<= 1;
    schedule->events.array = reallocarray(
        schedule->events.array,
        schedule->events.cap,
        sizeof(*schedule->events.array)
    );
}
