#pragma once

#include "rwr/model/alr56.h"
#include <math.h>

enum {
    RWR_SCHEDULE_EVENT_PAINT,
    RWR_SCHEDULE_EVENT_LOCK,
    RWR_SCHEDULE_EVENT_DROP_LOCK,
    RWR_SCHEDULE_EVENT_DROP,
    RWR_SCHEDULE_EVENT_FIRE_MISSILE,
    RWR_SCHEDULE_EVENT_DROP_MISSILE,
    RWR_SCHEDULE_EVENT_MISSILE_PING,
};

typedef uint8_t rwr_schedule_event_tag_t;

typedef uint8_t rwr_scheduled_contact_t;
typedef uint8_t rwr_scheduled_missile_t;

/// An event that is scheduled to happen at a specific time in an `rwr_schedule_t`
typedef struct rwr_schedule_event {
    rwr_schedule_event_tag_t tag;
    rwr_scheduled_contact_t contact;

    union {
        struct {
            location_t loc;
        } paint;
        
        struct {} lock;
        struct {} drop_lock;
        struct {} drop;

        struct {
            rwr_scheduled_missile_t missile;
        } fire_missile;

        struct {
            rwr_scheduled_missile_t missile;
        } drop_missile;

        struct {
            rwr_scheduled_missile_t missile;
            location_t loc;
        } missile_ping;
    };
} rwr_schedule_event_t;

typedef struct rwr_schedule {
    alr56_t *rwr;

} rwr_schedule_t;


