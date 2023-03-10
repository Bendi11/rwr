#pragma once

#include <stdint.h>

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

/// Tag indicating how a tone sequence should behave once all tones have been played
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
    tone_sequence_t *tones;
    tone_sequence_t *priority;
    float volume;
} tone_player_t;

/// Create a new tone player with the given sample rate when writing to an audio buffer
tone_player_t* tone_player_new(float sample_rate);

/// Set the global volume control for the tone player
void tone_player_set_volume(tone_player_t *tones, float volume);

/// Add a new tone sequence to this tone player
void tone_player_add(tone_player_t *player, tone_sequence_t *seq);

/// Add a sound to play while muting other tones
void tone_player_add_pri(tone_player_t *player, tone_sequence_t *seq);

/// Remove a tone from the priority tones list
void tone_player_remove_pri(tone_player_t *player, tone_sequence_t *seq);

/// Fill an audio buffer with the mixed tones
void tone_player_fill_buf(tone_player_t *player, float *buf, int len);

/// Remove a tone sequence from the standard tones list
void tone_player_remove(tone_player_t *player, tone_sequence_t *seq);

/// Free any resources allocated by the tone player
void tone_player_free(tone_player_t *player);

/// Create a new tone sequence from a series of tones and an end behavior
tone_sequence_t* tone_sequence_new(tone_t *tones, uint8_t len, tone_sequence_end_t end_behavior);

#define TONE_SEQUENCE_END(tflag, ...) (tone_sequence_end_t){.flag=tflag, __VA_ARGS__}

/// Macro to create a new tone sequence without providing length of the tones list
#define TONE_SEQUENCE(end_behavior, ...) \
    tone_sequence_new((__VA_ARGS__), sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]), end_behavior)

/// Free the given tone sequence's allocated memory
void tone_sequence_free(tone_sequence_t *seq);

