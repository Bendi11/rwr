#pragma once

#include "rwr/model/alr56.h"
#include <math.h>

enum {
    RWR_SCHEDULE_EVENT_NEWGUY,
    RWR_SCHEDULE_EVENT_PAINT,
    RWR_SCHEDULE_EVENT_LOCK,
    RWR_SCHEDULE_EVENT_DROP_LOCK,
    RWR_SCHEDULE_EVENT_DROP,
    RWR_SCHEDULE_EVENT_FIRE_MISSILE,
    RWR_SCHEDULE_EVENT_DROP_MISSILE,
    RWR_SCHEDULE_EVENT_MISSILE_PING,
};

typedef uint8_t rwr_schedule_event_tag_t;

typedef uint16_t rwr_scheduled_contact_t;
typedef uint16_t rwr_scheduled_missile_t;

#define RWR_SCHEDULE_EVENT_UNION_SIZE (16)

/// An event that is scheduled to happen at a specific time in an `rwr_schedule_t`
typedef struct rwr_schedule_event {
    rwr_scheduled_contact_t contact;
    rwr_schedule_event_tag_t tag;
    uint32_t time_ms;

    union {
        struct {
            location_t loc;
            source_idx_t source;
        } newguy;

        struct {
            location_t loc_diff;
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

typedef struct rwr_schedule rwr_schedule_t;

/// Create a new empty rwr schedule
rwr_schedule_t *rwr_schedule_new(void);

/// Start running the given schedule, dispatching events to the RWR
void rwr_schedule_run(rwr_schedule_t *schedule, alr56_t *rwr);

/// Check if the given RWR schedule is currently running on a separate timer thread
bool rwr_schedule_running(rwr_schedule_t *schedule);

/// Stop running the given schedule
void rwr_schedule_stop(rwr_schedule_t *schedule);

/// Free all resources associated with the given schedule, killing all timers that have been started using it
void rwr_schedule_free(rwr_schedule_t *schedule);

/// Deserialize an RWR schedule from the given buffer of size `len` bytes
rwr_schedule_t* rwr_schedule_deserialize(void *buf, size_t len);

/// Get the size in bytes that a buffer must be in order to serialize this schedule
size_t rwr_schedule_serialized_len(rwr_schedule_t *schedule);

/// Serialize the given schedule to `buf`, which must be at least `rwr_schedule_serialized_len` bytes
void rwr_schedule_serialize(rwr_schedule_t *schedule, void *buf);
