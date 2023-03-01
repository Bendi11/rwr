#include "rwr/schedule/builder.h"
#include "rwr/schedule/schedule.h"
#include "private.h"


rwr_encounter_builder_t* rwr_schedule_encounter(rwr_schedule_t *schedule, float time, location_t loc, source_idx_t source) {
    rwr_encounter_builder_t *builder = malloc(sizeof(*builder));
    builder->schedule = schedule;
    builder->location = loc;
    builder->t_offset = S_TO_MS(time);
    builder->contact = rwr_schedule_new_contact(schedule);

    rwr_schedule_add_event(
        schedule,
        (rwr_schedule_event_t) {
            .tag = RWR_SCHEDULE_EVENT_NEWGUY,
            .contact = builder->contact,
            .time_ms = builder->t_offset,
            .newguy = {
                .loc = builder->location,
                .source = source,
            }
        }
    );

    builder->t_offset += 1;

    return builder;
}

void rwr_encounter_delay(rwr_encounter_builder_t *builder, float seconds) {
    builder->t_offset += S_TO_MS(seconds);
    builder->max_time = builder->t_offset > builder->max_time ? builder->t_offset : builder->max_time;
}

void rwr_encounter_paint(rwr_encounter_builder_t *builder) {
    rwr_schedule_add_event(
        builder->schedule,
        (rwr_schedule_event_t) {
            .tag = RWR_SCHEDULE_EVENT_PAINT,
            .contact = builder->contact,
            .time_ms = builder->t_offset,
            .paint = {
                .loc = builder->location,
                
            }
        }
    );
}

void rwr_encounter_paint_periodic(rwr_encounter_builder_t *builder, rand_range_t ping_interval, float time) {
    uint32_t end_time = builder->t_offset + S_TO_MS(time);
    uint32_t t = builder->t_offset;
    while(builder->t_offset < end_time) {
        float delay = rwr_encounter_rand(ping_interval);
        rwr_encounter_delay(builder, delay);
        rwr_encounter_paint(builder);
    }
    builder->t_offset = t;
}

void rwr_encounter_move_abs(rwr_encounter_builder_t *builder, location_t loc) {
    builder->location = loc;
}

void rwr_encounter_move(rwr_encounter_builder_t *builder, location_t diff) {
    builder->location.altitude += diff.altitude;
    builder->location.bearing += diff.bearing;
    builder->location.distance += diff.distance;
}

void rwr_encounter_move_periodic(rwr_encounter_builder_t *builder, rand_range_t interval, float time, rand_location_t diff) {
    uint32_t end_time = builder->t_offset + S_TO_MS(time);
    uint32_t t = builder->t_offset;
    while(builder->t_offset < end_time) {
        float delay = rwr_encounter_rand(interval);
        rwr_encounter_delay(builder, delay);
        rwr_encounter_move(builder, rwr_encounter_rand_location(diff));
    }
    builder->t_offset = t;
}

float rwr_encounter_rand(rand_range_t range) {
    float r = (float)rand() / (float)RAND_MAX;
    r = (r * (range.max - range.min) + range.min);
    return r;
}

location_t rwr_encounter_rand_location(rand_location_t loc) {
    return (location_t) {
        .distance = rwr_encounter_rand(loc.distance),
        .bearing = rwr_encounter_rand(loc.bearing),
        .altitude = rwr_encounter_rand(loc.altitude),
    };
}

rwr_scheduled_missile_t rwr_encounter_missile(rwr_encounter_builder_t *builder) {
    rwr_scheduled_missile_t missile = rwr_schedule_new_missile(builder->schedule);

    rwr_schedule_add_event(
        builder->schedule,
        (rwr_schedule_event_t) {
            .tag = RWR_SCHEDULE_EVENT_FIRE_MISSILE,
            .contact = builder->contact,
            .time_ms = builder->t_offset,
            .fire_missile = {
                .missile = missile
            }
        }
    );

    return missile;
}

void rwr_encounter_complete(rwr_encounter_builder_t *builder) {
    rwr_schedule_add_event(
        builder->schedule,
        (rwr_schedule_event_t) {
            .tag = RWR_SCHEDULE_EVENT_DROP,
            .contact = builder->contact,
            .time_ms = builder->max_time,
            .drop = {}
        }
    );

    free(builder);
}
