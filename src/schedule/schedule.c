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
