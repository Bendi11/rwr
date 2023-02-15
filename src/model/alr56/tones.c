#include "private.h"

tone_sequence_t* alr56_get_lock_tone(alr56_t *rwr, const source_t *const source) {
    source_radar_t radar = source->radar;
    if(source->location == RADAR_SOURCE_AIR && radar.prf > ALR56_MAX_AIR_PRF) {
        return TONE_SEQUENCE(
            TONE_SEQUENCE_END(TONE_SEQUENCE_LOOP),
            (tone_t[]){
                (tone_t){ .frequency = ALR56_MAX_AIR_PRF, .amplitude = 1, .length = 0.075 },
                (tone_t){ .amplitude = 0, .length = 1.411 }
            }
        );
    } else if(source->location != RADAR_SOURCE_AIR && radar.prf > ALR56_MAX_GROUND_PRF) {
        return TONE_SEQUENCE(
            TONE_SEQUENCE_END(TONE_SEQUENCE_LOOP),
            (tone_t[]){
                (tone_t){ .frequency = ALR56_MAX_GROUND_PRF, .amplitude = 1, .length = 0.048 },
                (tone_t){ .amplitude = 0, .length = 0.752 }
            }
        );
    } else {
        return TONE_SEQUENCE(
            TONE_SEQUENCE_END(TONE_SEQUENCE_LOOP),
            (tone_t[]){
                (tone_t){ .frequency = radar.prf, .amplitude = 1, .length = radar.on_s },
                (tone_t){ .frequency = 0, .amplitude = 0, .length = radar.off_s }
            }
        );
    }
}

tone_sequence_t* alr56_newguy_air_tone(void) {
    return TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_LOOPFOR, .loopfor = { .loops = 8 }),
        (tone_t[]){
            (tone_t){ .frequency = 3000, .amplitude = 1, .length = 0.025 },
            (tone_t){ .amplitude = 0, .length = 0.025 },
        }
    );
}

tone_sequence_t* alr56_newguy_surface_tone(void) {
    return TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_LOOPFOR, .loopfor = { .loops = 8 }),
        (tone_t[]){
            (tone_t){ .frequency = 500, .amplitude = 1, .length = 0.025 },
            (tone_t){ .amplitude = 0, .length = 0.025 },
        }
    );
}

tone_sequence_t* alr56_missile_tone(void) {
    return TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_LOOPFOR, .loopfor = { .loops = ALR56_LAUNCH_REPETITIONS }),
        (tone_t[]){
            (tone_t){ .frequency = 1000, .amplitude = 1, .length = ALR56_LAUNCH_PERIOD },
            (tone_t){ .amplitude = 0, .length = ALR56_LAUNCH_PERIOD }
        }
    );
}

tone_sequence_t* alr56_silence_tone(void) {
    return TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_STOP),
        (tone_t[]){ (tone_t){ .amplitude = 0, .length = 0.4 } }
    );
}
