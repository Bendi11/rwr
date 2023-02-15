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

/// An RWR contact including radar emitter and location data
typedef struct contact {
    const source_t *source;
    location_t location;
    contact_status_t status;
    union {
        /// Additional state for an RWR contact that has painted the aircraft with a search radar
        struct contact_search {
            size_t last_ping;
            location_t last_loc;
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

void contact_remove_missile(contact_t *contact, fired_missile_t *missile);

/// Free any memory allocated for the given contact
void contact_delete(contact_t contact);

/// Free the given missile and any other linked list missiles
void fired_missile_free(fired_missile_t *msl);
