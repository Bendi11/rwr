#pragma once

#include "rwr.h"
#include "rwr/tones.h"
#include "rwr/source.h"
#include <stddef.h>


#define ALR56_MAX_CONTACTS (16)

enum {
    ALR56_HANDOFF_NORMAL,
    ALR56_HANDOFF_DIAMOND_FLOAT,
    ALR56_HANDOFF_TRANSIENT,
    ALR56_HANDOFF_LATCH
};

typedef uint8_t alr56_handoff_mode_t;

typedef struct alr56_priority_contact {
    contact_t *contact;
    tone_sequence_t *lock_tone;
} alr56_priority_contact_t;

/// Model for the ALR-56M RWR
typedef struct alr56 {
    contact_t contacts[ALR56_MAX_CONTACTS];
    tone_player_t *tones;
    alr56_handoff_mode_t handoff_mode;
    alr56_priority_contact_t priority;
    float volume;
} alr56_t;

/// Create a new ALR56 RWR model with no contacts
alr56_t* alr56_new(tone_player_t *player);

/// Attempt to create a new RWR that has painted the aircraft with search radar
///
/// Returns NULL if the RWR has reached the maximum number of contacts
contact_t *alr56_newguy(alr56_t *rwr, const source_t *source, location_t location);

/// Upgrade the given contact to an STT lock
void alr56_lock(alr56_t *rwr, contact_t *contact);

/// Launch a missile from the given target
void alr56_missile(alr56_t *rwr, contact_t *contact, uint32_t timer);

/// Drop the given RWR contact, breaking lock if gained
void alr56_drop(alr56_t *rwr, contact_t *contact);

/// Free memory allocated for this model, does not free the tone player passed to the `alr56_new` function
void alr56_free(alr56_t *rwr);
