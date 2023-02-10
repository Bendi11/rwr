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
    TONE_SEQUENCE_STOP,
    TONE_SEQUENCE_LOOPFOR,
};

typedef uint8_t tone_sequence_end_flag_t;

/// Behavior when a tone sequence reaches the final tone
typedef struct tone_sequence_end {
    tone_sequence_end_flag_t flag;
    union {
        struct {
            uint8_t loops;
        } loopfor;
    };
} tone_sequence_end_t;

/// A sequence of tones that can loop or end
typedef struct tone_sequence {
    uint8_t tone_idx;
    uint8_t tone_len;
    tone_sequence_end_t end_behavior;
    tone_t *tones;
    struct tone_sequence *next;
} tone_sequence_t;

/// Audio player that mixes multiple `tone_sequence`s
typedef struct tone_player {
    float sample_rate;
    float sample_ts;
    float pause_timer;
    tone_sequence_t *tones;
    tone_sequence_t *priority;
} tone_player_t;

tone_player_t* tone_player_new(float sample_rate);

/// Add a new tone sequence to this tone player
void tone_player_add(tone_player_t *player, tone_sequence_t *seq);

/// Fill an audio buffer with the mixed tones
void tone_player_fill_buf(tone_player_t *player, float *buf, int len);

void tone_player_remove_sequence(tone_player_t *player, tone_sequence_t *seq);

void tone_player_free(tone_player_t *player);

/// Create a new tone sequence from a series of tones and an end behavior
tone_sequence_t* tone_sequence_new(tone_t *tones, uint8_t len, tone_sequence_end_t end_behavior);

#define TONE_SEQUENCE_END(tflag, ...) (tone_sequence_end_t){.flag=tflag, __VA_ARGS__}

#define TONE_SEQUENCE(end_behavior, ...) \
    tone_sequence_new((__VA_ARGS__), sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]), end_behavior)

/// Free the given tone sequence's allocated memory
void tone_sequence_free(tone_sequence_t *seq);

void contact_add_missile(contact_t *contact, fired_missile_t missile);
