#include "rwr/model/alr56.h"
#include "rwr.h"
#include "rwr/source.h"
#include "private.h"

#include <SDL2/SDL.h>
#include <malloc.h>


void alr56_recompute_priority(alr56_t *rwr) {
    contact_t *pri = alr56_find_priority(rwr);
    if(pri != NULL) {
        if(pri != rwr->priority.contact) {
            alr56_clear_priority(rwr);
            rwr->priority.contact = pri;
        }

        if(pri->status == CONTACT_LOCK && rwr->priority.lock_tone == NULL && rwr->twp.handoff_mode != ALR56_HANDOFF_NORMAL) {
            rwr->priority.lock_tone = alr56_get_lock_tone(rwr, pri->source);
            tone_player_add(rwr->tones, rwr->priority.lock_tone);
        }
    }
}

alr56_t* alr56_new(tone_player_t *tone_player) {
    alr56_t *rwr = malloc(sizeof(*rwr));
    memset(rwr->contacts, 0, sizeof(rwr->contacts));
    rwr->tones = tone_player;
    rwr->twp = (alr56_twp_t){
        .handoff_mode = ALR56_HANDOFF_DIAMOND_FLOAT,
        .missile_launch = (alr56_blink_common_t){
            .blinks_remaining = 0,
        },
        .priority = false,
        .unknown = {
            .blink = (alr56_blink_common_t){ .blinks_remaining = 0 },
            .state = ALR56_TWP_UNKNOWN_OFF
        },
        .target_sep = false
    };
    rwr->twa = (alr56_twa_t){
        .dim = 1,
        .low_altitude_pri = false,
        .power = true,
        .search = true
    };
    rwr->priority = (alr56_priority_contact_t){
        .contact = NULL,
        .lock_tone = NULL
    };

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
            tone_player_add_pri(rwr->tones, alr56_newguy_air_tone());
            tone_player_add_pri(rwr->tones, alr56_silence_tone());
            tone_player_add_pri(rwr->tones, alr56_newguy_air_tone());
            tone_player_add_pri(rwr->tones, alr56_silence_tone());
        } else {
            tone_player_add_pri(rwr->tones, alr56_newguy_surface_tone());
            tone_player_add_pri(rwr->tones, alr56_silence_tone());
            tone_player_add_pri(rwr->tones, alr56_newguy_surface_tone());
            tone_player_add_pri(rwr->tones, alr56_silence_tone());
        }

        alr56_recompute_priority(rwr);
    }

    return contact;
}

uint16_t alr56_get_threat(const contact_t *source) {
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

contact_t* alr56_find_priority(alr56_t *rwr) {
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

void alr56_clear_priority(alr56_t *rwr) {
    rwr->priority.contact = NULL;
    if(rwr->priority.lock_tone != NULL) {
        tone_player_remove(rwr->tones, rwr->priority.lock_tone);
        rwr->priority.lock_tone = NULL;
    }
}

void alr56_drop(alr56_t *rwr, contact_t *contact) {
    contact_delete(*contact);
    contact->source = NULL;

    if(rwr->priority.contact == contact) {
        alr56_recompute_priority(rwr); 
    }
}

void alr56_lock(alr56_t *rwr, contact_t *contact) {
    if(contact->status == CONTACT_LOCK) { return; }
   
    contact->status = CONTACT_LOCK;
    if(rwr->twp.handoff_mode == ALR56_HANDOFF_DIAMOND_FLOAT) {
        alr56_recompute_priority(rwr);
    }
}

void alr56_missile(alr56_t *rwr, contact_t *contact, uint32_t timer) {
    if(contact->status != CONTACT_LOCK) {
        alr56_lock(rwr, contact);
    }

    fired_missile_t missile = fired_missile_new(contact->location);

    contact_add_missile(contact, missile);

    tone_player_add_pri(rwr->tones, alr56_missile_tone());
}

void alr56_free(alr56_t *rwr) {
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        contact_delete(rwr->contacts[i]);
    }
    free(rwr);
}
