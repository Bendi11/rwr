#pragma once

#include "rwr/source.h"
#include <SDL_timer.h>
#include <stddef.h>
#include <stdint.h>

/// Location of a contact in relation to an RWR
typedef struct location {
    /// Bearing from the aircraft to the contact in radians
    float bearing;
    /// Distance from aircraft to the signal source
    float distance;
    /// Altitude of the source radar in feet
    float altitude;
} location_t;

/// A missile fired by a radar contact with an active lock
typedef struct fired_missile {
    location_t location;
    SDL_TimerID timer;
    struct fired_missile *next;
} fired_missile_t;

enum {
    CONTACT_SEARCH,
    CONTACT_LOCK
};

typedef uint8_t contact_status_t;

/// Unique identifier for every `contact_t`, used to differentiate contacts that have been moved in memory
typedef uint16_t contact_id_t;

#define INVALID_CONTACT_ID ((uint16_t)0)

/// An RWR contact including radar emitter and location data
typedef struct contact {
    contact_status_t status;
    contact_id_t id;
    location_t location;
    const source_t *source;
    union {
        /// Additional state for an RWR contact that has painted the aircraft with a search radar
        struct contact_search {
            uint64_t last_ping;
        } search;
        
        /// Additional state for an RWR contact that has an STT lock on the aircraft
        struct contact_lock {
            /// A pointer to a linked list node of missiles fired that are guided by this radar
            fired_missile_t *missiles; 
        } lock;
    };
} contact_t;

/// Initialize the given contact with default values
void contact_new(contact_t *contact, const source_t *source, location_t loc, contact_status_t status);

/// Create a new missile with the given launch location
fired_missile_t fired_missile_new(location_t loc);

/// Add a fired missile to the linked list of missiles fired by the given locked contact
fired_missile_t* contact_add_missile(contact_t *contact, fired_missile_t missile);

/// Get the number of missiles fired by this contact
size_t contact_missiles_count(contact_t *contact);

/// Check if the contact still contains the given fired missile
bool contact_has_missile(contact_t *contact, fired_missile_t *missile);

/// Remove a fired missile from the linked list of a locked RWR contact
void contact_remove_missile(contact_t *contact, fired_missile_t *missile);

/// Free any memory allocated for the given contact
void contact_delete(contact_t contact);

/// Free the given missile and any other linked list missiles
void fired_missile_free(fired_missile_t *msl);
