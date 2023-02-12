#pragma once

#include "rwr/source.h"
#include <stddef.h>
#include <stdint.h>

/// Location of a contact in relation to an RWR
typedef struct location {
    /// Bearing from the aircraft to the contact in radians
    float bearing;
    /// Distance from aircraft to the signal source
    float distance;
} location_t;

/// A missile fired by a radar contact with an active lock
typedef struct fired_missile {
    location_t location;
    SDL_TimerID associated_timer;
    struct fired_missile *next;
} fired_missile_t;

/// An RWR contact including radar emitter and location data
typedef struct contact {
    const source_t *source;
    location_t location;
    enum { CONTACT_SEARCH, CONTACT_LOCK } status;
    union {
        /// Additional state for an RWR contact that has painted the aircraft with a search radar
        struct contact_search {
            size_t last_ping;
            location_t last_loc;
        } search;
        
        /// Additional state for an RWR contact that has an STT lock on the aircraft
        struct contact_lock {
            /// A pointer to a linked list node of missiles fired that are guided by this radar
            fired_missile_t *missile; 
        } lock;
    };
} contact_t;


void contact_add_missile(contact_t *contact, fired_missile_t missile);

/// Free any memory allocated for the given contact
void contact_delete(contact_t contact);

/// Free the given missile and any other linked list missiles
void fired_missile_free(fired_missile_t *msl);
