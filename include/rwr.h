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

/// A single sine wave played for a period of time
typedef struct tone {
    float amplitude;
    float frequency;
    float length;
    float progress;
} tone_t;

enum {
    TONE_SEQUENCE_LOOP,
    TONE_SEQUENCE_STOP
};

/// A sequence of tones that can loop or end
typedef struct tone_sequence {
    uint8_t tone_idx;
    uint8_t end_behavior;
    struct tone_sequence *next;
    uint8_t tone_len;
    tone_t tones[];
} tone_sequence_t;


typedef struct tone_player {
    uint64_t total_samples;
    tone_sequence_t *tones; 
} tone_player_t;

/// Fill an audio buffer with the mixed tones
void tone_player_fill_buf(tone_player_t *player, float *buf, int len);

void tone_player_remove_sequence(tone_sequence_t *seq);
