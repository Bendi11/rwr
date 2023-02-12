#include "private.h"

tone_sequence_t* alr56_get_lock_tone(alr56_t *rwr, const source_t *const source) {
    source_radar_t radar = source->radar;
    if(source->location == RADAR_SOURCE_AIR && radar.prf > ALR56_MAX_AIR_PRF) {
        return TONE_SEQUENCE(
            TONE_SEQUENCE_END(TONE_SEQUENCE_LOOP),
            (tone_t[]){
                (tone_t){ .frequency = ALR56_MAX_AIR_PRF, .amplitude = rwr->volume, .length = 0.075 },
                (tone_t){ .amplitude = 0, .length = 1.411 }
            }
        );
    } else if(source->location == RADAR_SOURCE_SURFACE && radar.prf > ALR56_MAX_GROUND_PRF) {
        return TONE_SEQUENCE(
            TONE_SEQUENCE_END(TONE_SEQUENCE_LOOP),
            (tone_t[]){
                (tone_t){ .frequency = ALR56_MAX_GROUND_PRF, .amplitude = rwr->volume, .length = 0.048 },
                (tone_t){ .amplitude = 0, .length = 0.752 }
            }
        );
    } else {
        return TONE_SEQUENCE(
            TONE_SEQUENCE_END(TONE_SEQUENCE_LOOP),
            (tone_t[]){
                (tone_t){ .frequency = radar.prf, .amplitude = rwr->volume, .length = radar.on_s },
                (tone_t){ .frequency = 0, .amplitude = 0, .length = radar.off_s }
            }
        );
    }
}

tone_sequence_t* newguy_air_tone(float volume) {
    return TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_LOOPFOR, .loopfor = { .loops = 8 }),
        (tone_t[]){
            (tone_t){ .frequency = 3000, .amplitude = volume, .length = 0.025 },
            (tone_t){ .amplitude = 0, .length = 0.025 },
        }
    );
}

tone_sequence_t* newguy_surface_tone(float volume) {
    return TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_LOOPFOR, .loopfor = { .loops = 8 }),
        (tone_t[]){
            (tone_t){ .frequency = 500, .amplitude = volume, .length = 0.025 },
            (tone_t){ .amplitude = 0, .length = 0.025 },
        }
    );
}

tone_sequence_t* missile_tone(float volume) {
    return TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_LOOPFOR, .loopfor = { .loops = 10 }),
        (tone_t[]){
            (tone_t){ .frequency = 1000, .amplitude = volume, .length = 0.1 },
            (tone_t){ .amplitude = 0, .length = 0.1 }
        }
    );
}

tone_sequence_t* silence_tone(void) {
    return TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_STOP),
        (tone_t[]){ (tone_t){ .amplitude = 0, .length = 0.4 } }
    );
}
