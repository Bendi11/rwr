#pragma once
#include "rwr/model/alr56.h"

/// Clear the current priority target
void alr56_clear_priority(alr56_t *rwr);

/// Find the contact with the highest value returned from `alr56_get_threat`
contact_t* alr56_find_priority(alr56_t *rwr);

/// Convert a PRF description into a lock tone that can be played
tone_sequence_t* alr56_get_lock_tone(alr56_t *rwr, const source_t *const source);

/// Get a number representing the threat posed by a radar contact, weighing distance and current lock status
uint16_t alr56_get_threat(const contact_t *source);

/// Remove the currently prioritized contact and recalculate the priority contact for floating diamond handoff mode
///
/// Does not remove the currently playing lock tone if the priority contact does not change
void alr56_recompute_priority(alr56_t *rwr);

tone_sequence_t* alr56_newguy_air_tone(void);

tone_sequence_t* alr56_newguy_surface_tone(void);

tone_sequence_t* alr56_missile_tone(void);

tone_sequence_t* alr56_silence_tone(void);
