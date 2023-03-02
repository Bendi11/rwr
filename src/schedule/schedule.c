#include "rwr/schedule/schedule.h"
#include "rwr.h"
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

    rwr_schedule_event_t *ev = &schedule->events.array[schedule->run.event];
    schedule->run.time += time;
    
    switch(ev->tag) {
        case RWR_SCHEDULE_EVENT_NEWGUY: {
            if(schedule->run.contacts[ev->contact] != INVALID_CONTACT_ID) {
                fputs("Attempt to create a new contact in schedule when contact is already initialized", stderr);
                return 0;
            }

            schedule->run.contacts[ev->contact] = alr56_newguy(
                schedule->run.rwr,
                &SOURCES[ev->newguy.source],
                ev->newguy.loc
            );
        } break;

        case RWR_SCHEDULE_EVENT_PAINT: {
            contact_t *contact = alr56_lookup_contact(schedule->run.rwr, schedule->run.contacts[ev->contact]);
            location_t newloc = (location_t){
                .altitude = contact->location.altitude + ev->paint.loc_diff.altitude,
                .bearing = contact->location.bearing + ev->paint.loc_diff.bearing,
                .distance = contact->location.distance + ev->paint.loc_diff.distance,
            };
            alr56_ping(
                schedule->run.rwr,
                schedule->run.contacts[ev->contact],
                newloc
            );
        } break;

        case RWR_SCHEDULE_EVENT_DROP: {
            alr56_drop(schedule->run.rwr, schedule->run.contacts[ev->contact]);
            schedule->run.contacts[ev->contact] = INVALID_CONTACT_ID;
        } break;

        case RWR_SCHEDULE_EVENT_DROP_LOCK: {
            alr56_drop_lock(schedule->run.rwr, schedule->run.contacts[ev->contact]);
        } break;

        case RWR_SCHEDULE_EVENT_FIRE_MISSILE: {
            schedule->run.missiles[ev->fire_missile.missile] = alr56_missile(schedule->run.rwr, schedule->run.contacts[ev->contact]);
        } break;

        case RWR_SCHEDULE_EVENT_MISSILE_PING: {
            schedule->run.missiles[ev->missile_ping.missile]->location = ev->missile_ping.loc;
        } break;

        case RWR_SCHEDULE_EVENT_DROP_MISSILE: {
            contact_t *contact = alr56_lookup_contact(schedule->run.rwr, ev->contact);
            fired_missile_t *missile = schedule->run.missiles[ev->drop_missile.missile];
            contact_remove_missile(contact, missile);
        } break;
    }
    
    schedule->run.event += 1;
    if(schedule->run.event >= schedule->events.len) { return 0; }
    uint32_t next = schedule->events.array[schedule->run.event].time_ms - (uint32_t)schedule->run.time;
    next = next == 0 ? 1 : next;
    return next;
}

void rwr_schedule_run(rwr_schedule_t *schedule, alr56_t *rwr) {
    rwr_schedule_stop(schedule);
    schedule->run.rwr = rwr;
    schedule->run.event = 0;
    schedule->run.contacts = calloc(schedule->contacts, sizeof(contact_t*));
    schedule->run.missiles = calloc(schedule->missiles, sizeof(fired_missile_t*));
    qsort(
        schedule->events.array,
        schedule->events.len,
        sizeof(*schedule->events.array),
        rwr_schedule_cmp_events
    );
    
    schedule->run.time = 0;
    schedule->run.timer = SDL_AddTimer(1, rwr_schedule_timer_cb, schedule);
}

void rwr_schedule_stop(rwr_schedule_t *schedule) {
    if(schedule->run.rwr == NULL) { return; }
    
    SDL_RemoveTimer(schedule->run.timer);
    schedule->run.rwr = NULL;
    free(schedule->run.contacts);
    free(schedule->run.missiles);
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

int rwr_schedule_cmp_events(const void *v1, const void *v2) {
    const rwr_schedule_event_t *e1 = v1;
    const rwr_schedule_event_t *e2 = v2;

    return (int)e1->time_ms - (int)e2->time_ms;
}

void rwr_schedule_free(rwr_schedule_t *schedule) {
    rwr_schedule_stop(schedule);

    free(schedule->events.array);
    free(schedule);
}
