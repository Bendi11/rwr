#pragma once

#include "rwr.h"
#include "rwr/tones.h"
#include "rwr/source.h"
#include <SDL_timer.h>
#include <stddef.h>


#define ALR56_MAX_CONTACTS (16)
/// Airborne radar signals' audio will be capped to 3000hz
#define ALR56_MAX_AIR_PRF (3000)
/// Surface radar signals' audio will be capped to 500hz
#define ALR56_MAX_GROUND_PRF (500)

enum {
    ALR56_HANDOFF_NORMAL,
    ALR56_HANDOFF_DIAMOND_FLOAT,
    ALR56_HANDOFF_TRANSIENT,
    ALR56_HANDOFF_LATCH
};

/// Handoff mode, determines how the priority diamond will be assigned
typedef uint8_t alr56_handoff_mode_t;

/// An RWR priority contact containing a pointer to the currently playing lock tone for the 
/// contact
typedef struct alr56_priority_contact {
    contact_t *contact;
    tone_sequence_t *lock_tone;
} alr56_priority_contact_t;

/// Common state for a blinking light
typedef struct alr56_blink_common {
    bool light;
    uint8_t blinks_remaining;
    float period;
    SDL_TimerID timer;
} alr56_blink_common_t;

enum {
    ALR56_TWP_UNKNOWN_SHIP,
    ALR56_TWP_UNKNOWN_ON,
    ALR56_TWP_UNKNOWN_OFF,
};

typedef uint8_t alr56_twp_unknown_state_t;

/// State modelling the Theat Warning Panel found next to the RWR display
typedef struct alr56_twp {
    alr56_handoff_mode_t handoff_mode;
    alr56_blink_common_t missile_launch;
    struct {
        alr56_blink_common_t blink;
        alr56_twp_unknown_state_t state;
    } unknown;

    bool target_sep;

    /// Display only five high priority contacts instead of 12 or 16
    bool priority;
} alr56_twp_t;

/// State modelling the TWA panel controlling more functionality of the RWR
typedef struct alr56_twa {
    float dim;
    bool search;
    bool low_altitude_pri;
    bool power;
} alr56_twa_t;

/// Model for the ALR-56M RWR
typedef struct alr56 {
    contact_t contacts[ALR56_MAX_CONTACTS];
    alr56_twp_t twp;
    alr56_twa_t twa;
    alr56_priority_contact_t priority;
    tone_player_t *tones;
    contact_t *latest;
    SDL_TimerID periodic;

    struct alr56_forgotten_link {
        contact_t contact;
        struct alr56_forgotten_link *next;
    } *forgotten;
} alr56_t;

/// Create a new ALR56 RWR model with no contacts
alr56_t* alr56_new(tone_player_t *player);

/// Attempt to create a new RWR that has painted the aircraft with search radar
///
/// Returns NULL if the RWR has reached the maximum number of contacts
contact_id_t alr56_newguy(alr56_t *rwr, const source_t *source, location_t location);

/// Check if the given contact has been forgotten by the RWR because it has not been pinged
bool alr56_contact_forgotten(alr56_t *rwr, const contact_id_t contact);

/// Paint the RWR receiver with radar, updating the position on the display
void alr56_ping(alr56_t *rwr, contact_id_t contact, location_t loc);

/// Drop the given contact from the RWR, freeing the memory allocated for the contact
void alr56_drop(alr56_t *rwr, contact_id_t contact);

/// Upgrade the given contact to an STT lock
void alr56_lock(alr56_t *rwr, contact_id_t contact);

/// Launch a missile from the given target
fired_missile_t* alr56_missile(alr56_t *rwr, contact_id_t contact);

/// Drop the given RWR contact's lock if gained
void alr56_drop_lock(alr56_t *rwr, contact_id_t contact);

/// Free memory allocated for this model, does not free the tone player passed to the `alr56_new` function
void alr56_free(alr56_t *rwr);

/// Get a pointer to the given contact represented by ID, either from the forgotten list or the active contacts list
contact_t* alr56_lookup_contact(alr56_t *rwr, const contact_id_t contact);
