#include "rwr/model/alr56.h"
#include "rwr.h"

#include <SDL2/SDL.h>
#include <malloc.h>

static tone_sequence_t *newguy_air_tone = NULL;
static tone_sequence_t *newguy_surface_tone = NULL;
static tone_sequence_t *missile_tone = NULL;

alr56_t* alr56_new(tone_player_t *tone_player) {
    alr56_t *rwr = malloc(sizeof(*rwr));
    memset(rwr->contacts, 0, sizeof(rwr->contacts));
    rwr->tones = tone_player;
    rwr->volume = 0.5;
    
    if(newguy_air_tone == NULL) {
        newguy_air_tone = TONE_SEQUENCE(
            (tone_sequence_end_t){TONE_SEQUENCE_STOP},
            (tone_t[]){
                (tone_t){ .frequency = 3000, .amplitude = rwr->volume / 20, .length = 0.25 },
                (tone_t){ .amplitude = 0, .length = 0.25 },

                (tone_t){ .frequency = 3000, .amplitude = rwr->volume / 20, .length = 0.25 },
                (tone_t){ .amplitude = 0, .length = 0.25 },
            }
        );
    }

    if(newguy_surface_tone == NULL) {
        newguy_surface_tone = TONE_SEQUENCE(
            (tone_sequence_end_t){TONE_SEQUENCE_STOP},
            (tone_t[]){
                (tone_t){ .frequency = 500, .amplitude = rwr->volume, .length = 0.25 },
                (tone_t){ .amplitude = 0, .length = 0.025 },

                (tone_t){ .frequency = 500, .amplitude = rwr->volume, .length = 0.25 },
                (tone_t){ .amplitude = 0, .length = 0.025 },
            }
        );
    }

    if(missile_tone == NULL) {
        missile_tone = TONE_SEQUENCE(
            TONE_SEQUENCE_END(TONE_SEQUENCE_LOOPFOR, .loopfor = { .loops = 10 }),
            (tone_t[]){
                (tone_t){ .frequency = 1000, .amplitude = rwr->volume, .length = 0.1 },
                (tone_t){ .amplitude = 0, .length = 0.1 }
            }
        );
    }


    return rwr;
}

contact_t* alr56_newguy(alr56_t *rwr, const source_t *source, location_t location) {
    contact_t *contact = NULL;
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        if(rwr->contacts[i].source == NULL) {
            contact = &rwr->contacts[i];
            *contact = (contact_t){
                .location = location,
                .status = CONTACT_SEARCH,
                .search = {
                    .last_ping = SDL_GetTicks64(),
                },
                .source = source
            };
            break;
        }
    }

    if(contact != NULL) {
        tone_player_add(rwr->tones, newguy_air_tone);
    }

    return contact;
}

void alr56_lock(alr56_t *rwr, contact_t *contact) {
    
}

void alr56_missile(alr56_t *rwr, contact_t *contact) {
    tone_player_add(rwr->tones, missile_tone);
}
