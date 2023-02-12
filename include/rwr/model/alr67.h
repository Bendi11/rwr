#pragma once

#include "rwr.h"
#include "rwr/source.h"
#include <stddef.h>
#include "rwr/tones.h"


#define ALR67_MAX_CONTACTS (16)


/// Model for the ALR-67 RWR
typedef struct alr67 {
    contact_t contacts[ALR67_MAX_CONTACTS];
    tone_player_t *tones;
    float volume;
} alr67_t;

/// Create a new ALR67 RWR model with no contacts
alr67_t* alr67_new(tone_player_t *player);

/// Attempt to create a new RWR that has painted the aircraft with search radar
///
/// Returns NULL if the RWR has reached the maximum number of contacts
contact_t *alr67_newguy(alr67_t *rwr, const source_t *source, location_t location);

/// Upgrade the given contact to an STT lock
void alr67_lock(alr67_t *rwr, contact_t *contact);

/// Launch a missile from the given target
void alr67_missile(alr67_t *rwr, contact_t *contact, uint32_t timer);

/// Drop the given RWR contact, breaking lock if gained
void alr67_drop(alr67_t *rwr, contact_t *contact);

/// Free memory allocated for this model, does not free the tone player passed to the `alr67_new` function
void alr67_free(alr67_t *rwr);
