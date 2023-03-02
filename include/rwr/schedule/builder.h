#pragma once

#include "rwr/schedule/schedule.h"


/// A builder structure that tracks state, providing easy ways to create dynamic schedule encounters for an 
/// `rwr_schedule_t`
typedef struct rwr_encounter_builder rwr_encounter_builder_t;

/// A random range defined by a minimum and maximum value
typedef struct rand_range {
    float min;
    float max;
} rand_range_t;

/// A collection of random parameters used to randomly generate an RWR contact's position
typedef struct rand_location {
    rand_range_t bearing;
    rand_range_t altitude;
    rand_range_t distance;
} rand_location_t;

/// Create a new encounter builder that schedules actions on the given schedule
rwr_encounter_builder_t* rwr_schedule_encounter(rwr_schedule_t *schedule, float time, location_t loc, source_idx_t source);

/// Delay a given timespan without scheduling anything more
void rwr_encounter_delay(rwr_encounter_builder_t *builder, float seconds);

/// Paint the radar receiver, updating the position on the display and remembering the contact if it was forgotten
void rwr_encounter_paint(rwr_encounter_builder_t *builder, location_t diff);

/// For the next `time` seconds, send a radar ping at the current location every `ping_interval` seconds
void rwr_encounter_paint_periodic(rwr_encounter_builder_t *builder, rand_range_t ping_interval, rand_location_t movement, float time);

/// Generate a random location using the given parameters
location_t rwr_encounter_rand_location(rand_location_t loc);

/// Generate a random number based on the given parameters
float rwr_encounter_rand(rand_range_t range);

/// Fire a new missile, returning the ID of the scheduled missile
rwr_scheduled_missile_t rwr_encounter_missile(rwr_encounter_builder_t *builder);

/// Delete the missile from the contact
void rwr_encounter_drop_missile(rwr_encounter_builder_t *builder, rwr_scheduled_missile_t missile);

/// Update the fired missile's position on the RWR display
void rwr_encounter_ping_missile(rwr_encounter_builder_t *builder, rwr_scheduled_missile_t missile, location_t loc);

/// Complete the encounter by dropping the contact and any fired missiles
void rwr_encounter_complete(rwr_encounter_builder_t *builder);
