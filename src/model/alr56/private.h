#pragma once
#include "rwr/model/alr56.h"


void alr56_clear_priority(alr56_t *rwr);

contact_t* alr56_find_priority(alr56_t *rwr);

tone_sequence_t* alr56_get_lock_tone(alr56_t *rwr, const source_t *const source);


uint16_t alr56_get_threat(const contact_t *source);


tone_sequence_t* newguy_air_tone(float volume);

tone_sequence_t* newguy_surface_tone(float volume);

tone_sequence_t* missile_tone(float volume);

tone_sequence_t* silence_tone(void);
