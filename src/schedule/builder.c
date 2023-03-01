#include "rwr/schedule/builder.h"
#include "rwr/schedule/schedule.h"
#include "private.h"


rwr_encounter_builder_t* rwr_schedule_encounter(rwr_schedule_t *schedule, location_t loc, source_idx_t source) {
    rwr_encounter_builder_t *builder = malloc(sizeof(*builder));
    builder->schedule = schedule;
    builder->location = loc;
    builder->contact = rwr_schedule_new_contact(schedule);

    rwr_schedule_add_event(
        schedule,
        (rwr_schedule_event_t) {
            .tag = RWR_SCHEDULE_EVENT_NEWGUY,
            .contact = builder->contact,
            .newguy = {
                .loc = builder->location,
                .source = source,
            }
        }
    );

    return builder;
}

void rwr_encounter_delay(rwr_encounter_builder_t *builder, float seconds) { builder->t_offset += (uint32_t)(seconds * 1000.f); }

void rwr_encounter_paint(rwr_encounter_builder_t *builder) {
    rwr_schedule_add_event(
        builder->schedule,
        (rwr_schedule_event_t) {
            .tag = RWR_SCHEDULE_EVENT_PAINT,
            .contact = builder->contact,
            .paint = {
                .loc = builder->location,
                
            }
        }
    );
}

void rwr_encounter_paint_periodic(rwr_encounter_builder_t *builder, rand_range_t ping_interval, float time) {
    uint32_t end_time = builder->t_offset + S_TO_MS(time);
    while(builder->t_offset < end_time) {
        uint32_t delay = S_TO_MS(rwr_encounter_rand(ping_interval));
        rwr_encounter_delay(builder, delay);
        rwr_encounter_paint(builder);
    }
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
    while(builder->t_offset < end_time) {
        uint32_t delay = S_TO_MS(rwr_encounter_rand(interval));
        rwr_encounter_delay(builder, delay);
        rwr_encounter_move(builder, rwr_encounter_rand_location(diff));
    }
}

float rwr_encounter_rand(rand_range_t range) {
    float r = (float)rand() / (float)RAND_MAX;
    return (r * (range.max - range.min) + range.min);
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
            .fire_missile = {
                .missile = missile
            }
        }
    );

    return missile;
}
