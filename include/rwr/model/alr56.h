#pragma once

#include "rwr.h"
#include "rwr/source.h"
#include <stddef.h>


#define ALR56_MAX_CONTACTS (16)

/// Model for the ALR-56M RWR
typedef struct alr56 {
    contact_t contacts[ALR56_MAX_CONTACTS]; 
} alr56_t;

/// Create a new ALR56 RWR model with no contacts
void alr56_new(alr56_t *rwr);

/// Attempt to create a new RWR that has painted the aircraft with search radar
///
/// Returns NULL if the RWR has reached the maximum number of contacts
contact_t *alr56_newguy(alr56_t *rwr, const source_t *source, location_t location);

/// Upgrade the given contact to an STT lock
void alr56_lock(alr56_t *rwr, contact_t *contact);

/// Drop the given RWR contact, breaking lock if gained
void alr56_drop(alr56_t *rwr, contact_t *contact);
