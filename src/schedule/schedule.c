#include "rwr/schedule/schedule.h"
#include "priv.h"
#include "rwr/model/alr56.h"
#include <stdlib.h>

schedule_timer_t *schedule_timer_new(schedule_t *schedule, const timerprofile_t *profile) {
    schedule_timer_t *node = malloc(sizeof(*node));
    node->schedule = schedule;
    node->next = NULL;
    node->contact = NULL;
    node->profile = profile;
    node->timer = SDL_AddTimer(0, schedule_timer_cb, node);
    node->time = 0.f;
    node->time_goal = 0.f;
    return node;
}

static float frand(void) { return (float)rand() / (float)RAND_MAX; }

static location_t add_location(location_t a, location_t b) {
    return (location_t){
        .altitude = a.altitude + b.altitude,
        .distance = a.distance + b.distance,
        .bearing = a.bearing + b.bearing
    };
}

unsigned int schedule_timer_cb(unsigned int time, void *vnode) {
    schedule_timer_t *timer = (schedule_timer_t*)vnode;
    if(timer->contact == NULL) {
        if(timer->time == 0.f) {
            timer->time += (float)time / 1000.f;
            return rand_range(timer->profile->search.ping) * 1000.f;
        } else {
            timer->contact = alr56_newguy(
                timer->schedule->rwr,
                &SOURCES[timer->profile->contact],
                rand_location(timer->profile->initial_pos)
            );
            
            timer->time = 0.f;
            timer->time_goal = rand_range(timer->profile->search.total);
        }
    }

    timer->time += (float)time / 1000.f;

    if(timer->contact->status == CONTACT_SEARCH) {
        if(timer->time >= timer->time_goal) {
            alr56_lock(timer->schedule->rwr, timer->contact);
        } else if(frand() > timer->profile->search.drop_p) {
            float time_to_ping = rand_range(timer->profile->search.ping);
            alr56_ping(
                timer->schedule->rwr,
                timer->contact,
                add_location(
                    timer->contact->location,
                    rand_location(timer->profile->movement)
                )
            );

            return time_to_ping * 1000.f;
        } else {
            schedule_remove(timer->schedule, timer);
            return 0;
        }
    }

    if(timer->contact->status == CONTACT_LOCK) {
        if(frand() <= timer->profile->lock.drop_p) {
            alr56_drop_lock(timer->schedule->rwr, timer->contact);
            timer->time = 0.f;
            timer->time_goal = rand_range(timer->profile->search.total);
            return rand_range(timer->profile->search.ping) * 1000.f;
        }

        if(
            frand() <= timer->profile->lock.missile_p &&
            contact_missiles_count(timer->contact) < timer->profile->lock.max_missiles
        ) {
            alr56_missile(
                timer->schedule->rwr,
                timer->contact,
                rand_range(timer->profile->lock.missile_duration)
            ); 
        }

        alr56_ping(
            timer->schedule->rwr,
            timer->contact,
            add_location(
                timer->contact->location,
                rand_location(timer->profile->movement)
            )
        );

        return rand_range(timer->profile->lock.ping) * 1000.f;
    }

    return 0;
}

location_t rand_location(rand_location_t loc) {
    return (location_t) {
        .altitude = rand_range(loc.altitude),
        .bearing = rand_range(loc.bearing),
        .distance = rand_range(loc.distance)
    };
}

float rand_range(randrange_t range) { return frand() * (range.max - range.min) + range.min; }

const timerprofile_t SA10_PROF = (timerprofile_t){
    .contact = SOURCE_SA10,
    .initial_delay = {5.f, 7.f},
    .initial_pos = {
        .bearing = {0.f, (float)M_PI},
        .distance = {0.2f, 35.f},
        .altitude = {0.f, 750.f},
    },
    .movement = {
        .bearing = {10.f, 10.f},
        .altitude = {0.f, 0.f},
        .distance = {0.f, 0.f}
    },
    .search = {
        .total = {2.5f, 15.f},
        .ping = {1.5f, 2.f},
        .drop_p = 0.05f
    },
    .lock = {
        .drop_p = 0.07f,
        .missile_p = 0.2f,
        .ping = {0.2f, 1.f},
        .missile_duration = {1.7f, 5.f},
        .max_missiles = 1
    }
};
