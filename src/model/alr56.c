#include "rwr/model/alr56.h"
#include "rwr.h"
#include "rwr/source.h"

#include <SDL2/SDL.h>
#include <malloc.h>

static tone_sequence_t *newguy_air_tone = NULL;
static tone_sequence_t *newguy_surface_tone = NULL;
static tone_sequence_t *missile_tone = NULL;


static void alr56_diamond_float_assign(alr56_t *rwr);
static void alr56_clear_priority(alr56_t *rwr);

static void alr56_recompute_priority(alr56_t *rwr) {
    alr56_clear_priority(rwr);
    alr56_diamond_float_assign(rwr);
}

alr56_t* alr56_new(tone_player_t *tone_player) {
    alr56_t *rwr = malloc(sizeof(*rwr));
    memset(rwr->contacts, 0, sizeof(rwr->contacts));
    rwr->tones = tone_player;
    rwr->volume = 0.05;
    rwr->handoff_mode = ALR56_HANDOFF_DIAMOND_FLOAT;
    rwr->priority = (alr56_priority_contact_t){
        .contact = NULL,
        .lock_tone = NULL
    };
    
    if(newguy_air_tone == NULL) {
        newguy_air_tone = TONE_SEQUENCE(
            TONE_SEQUENCE_END(TONE_SEQUENCE_LOOPFOR, .loopfor = { .loops = 8 }),
            (tone_t[]){
                (tone_t){ .frequency = 3000, .amplitude = rwr->volume / 20, .length = 0.025 },
                (tone_t){ .amplitude = 0, .length = 0.025 },

                (tone_t){ .frequency = 3000, .amplitude = rwr->volume / 20, .length = 0.025 },
                (tone_t){ .amplitude = 0, .length = 0.025 },
            }
        );
    }

    if(newguy_surface_tone == NULL) {
        newguy_surface_tone = TONE_SEQUENCE(
            TONE_SEQUENCE_END(TONE_SEQUENCE_LOOPFOR, .loopfor = { .loops = 8 }),
            (tone_t[]){
                (tone_t){ .frequency = 500, .amplitude = rwr->volume, .length = 0.025 },
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
        if(contact->source->location == RADAR_SOURCE_AIR) {
            tone_player_add(rwr->tones, newguy_air_tone);
        } else {
            tone_player_add(rwr->tones, newguy_surface_tone);
        }

        alr56_recompute_priority(rwr);
    }

    return contact;
}

static uint16_t alr56_get_threat(const contact_t *source) {
    uint16_t threat = 0;
    if(source->location.distance <= source->source->lethal_range) {
        threat += (uint16_t)(
            (source->source->lethal_range - source->location.distance) *
            (source->status == CONTACT_LOCK ? 100 : 10)
        );
    } else if(source->status == CONTACT_LOCK) {
        threat += 50;
    }

    return threat;
}

static contact_t* alr56_find_priority(alr56_t *rwr) {
    contact_t *highest;
    uint16_t threat = 0;
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        if(rwr->contacts[i].source != NULL) {
            uint16_t contact_threat = alr56_get_threat(&rwr->contacts[i]);
            if(contact_threat >= threat) {
                highest = &rwr->contacts[i];
                threat = contact_threat;
            } 
        }
    }

    return highest;
}

static tone_sequence_t* alr56_get_lock_tone(alr56_t *rwr, source_radar_t radar) {
    return TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_LOOP),
        (tone_t[]){
            (tone_t){ .frequency = radar.prf, .amplitude = rwr->volume, .length = radar.on_s },
            (tone_t){ .frequency = 0, .amplitude = 0, .length = radar.off_s }
        }
    );
}

static void alr56_diamond_float_assign(alr56_t *rwr) {
    contact_t *pri = alr56_find_priority(rwr);
    if(pri != NULL) {
        rwr->priority.contact = pri;
        if(pri->status == CONTACT_LOCK) {
            rwr->priority.lock_tone = alr56_get_lock_tone(rwr, pri->source->radar);
            tone_player_add(rwr->tones, rwr->priority.lock_tone);
        }
    }
}


static void alr56_clear_priority(alr56_t *rwr) {
    rwr->priority.contact = NULL;
    if(rwr->priority.lock_tone != NULL) {
        tone_player_remove_sequence(rwr->tones, rwr->priority.lock_tone);
        rwr->priority.lock_tone = NULL;
    }
}

void alr56_drop(alr56_t *rwr, contact_t *contact) {
    if(rwr->priority.contact == contact) {
        alr56_recompute_priority(rwr); 
    }

    contact->source = NULL;
}

void alr56_lock(alr56_t *rwr, contact_t *contact) {
    if(contact->status == CONTACT_LOCK) { return; }
   
    contact->status = CONTACT_LOCK;
    alr56_recompute_priority(rwr);
}

void alr56_missile(alr56_t *rwr, contact_t *contact, uint32_t timer) {
    if(contact->status != CONTACT_LOCK) {
        alr56_lock(rwr, contact);
    }

    fired_missile_t *missile = malloc(sizeof(*missile));
    *missile = (fired_missile_t){
        .location = contact->location,
        .next = NULL
    };

    

    tone_player_add(rwr->tones, missile_tone);
}

void alr56_free(alr56_t *rwr) {
    free(rwr);
}
