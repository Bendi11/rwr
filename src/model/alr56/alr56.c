#include "rwr/model/alr56.h"
#include "rwr.h"
#include "rwr/source.h"
#include "private.h"

#include <SDL2/SDL.h>
#include <SDL_timer.h>
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
            .period = ALR56_LAUNCH_PERIOD,
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

    rwr->periodic = SDL_AddTimer(ALR56_UPDATE_INTERVAL_MS, alr56_periodic_cb, rwr);

    return rwr;
}

contact_t* alr56_newguy(alr56_t *rwr, const source_t *source, location_t location) {
    contact_t *contact = NULL;
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        if(rwr->contacts[i].source == NULL) {
            contact = &rwr->contacts[i];
            contact_new(contact, source, location, CONTACT_SEARCH);
            break;
        }
    }


    if(contact != NULL) {
        rwr->latest = contact;
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

uint16_t alr56_get_threat(alr56_t *rwr, const contact_t *source) {
    uint16_t threat = 0;
    if(source->location.distance <= source->source->lethal_range) {
        threat += (uint16_t)(
            (source->source->lethal_range - source->location.distance) * 20 +
            (source->status == CONTACT_LOCK ? 100 : 0)
        );
    } else if(source->status == CONTACT_LOCK) {
        threat += 50;
    }

    if(source->source->location != RADAR_SOURCE_AIR) {
        if(rwr->twa.low_altitude_pri && source->source->location == RADAR_SOURCE_SURFACE_LOW_ALT) {
            threat += 100;
        } else if(!rwr->twa.low_altitude_pri && source->source->location == RADAR_SOURCE_SURFACE_HIGH_ALT) {
            threat += 100;
        }
    }

    return threat;
}

contact_t* alr56_find_priority(alr56_t *rwr) {
    contact_t *highest;
    uint16_t threat = 0;
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        if(rwr->contacts[i].source != NULL) {
            uint16_t contact_threat = alr56_get_threat(rwr, &rwr->contacts[i]);
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

void alr56_drop_lock(alr56_t *rwr, contact_t *contact) {
    if(contact->status == CONTACT_LOCK) {
        fired_missile_free(contact->lock.missiles);
        contact->lock.missiles = NULL;
        contact->status = CONTACT_SEARCH;
        contact->search.last_ping = SDL_GetTicks64();
        contact->location = contact->location;
        if(rwr->priority.contact == contact) {
            alr56_recompute_priority(rwr); 
        }
    }
}

void alr56_drop_contact(alr56_t *rwr, contact_t *contact) {
    alr56_drop_lock(rwr, contact);
    contact_delete(*contact);
    contact->source = NULL;

    if(rwr->priority.contact == contact) {
        alr56_recompute_priority(rwr); 
    }

    if(rwr->latest == contact) {
        rwr->latest = NULL;
    }
}

void alr56_ping(alr56_t *rwr, contact_t *contact, location_t loc) {
    contact->location = loc;
    if(contact->status == CONTACT_SEARCH) {
        contact->search.last_ping = SDL_GetTicks64();
    }
}

void alr56_lock(alr56_t *rwr, contact_t *contact) {
    if(contact->status == CONTACT_LOCK) { return; }
   
    contact->status = CONTACT_LOCK;
    contact->lock.missiles = NULL;
    if(rwr->twp.handoff_mode == ALR56_HANDOFF_DIAMOND_FLOAT) {
        alr56_recompute_priority(rwr);
    }
}

struct alr56_missile_cb_param {
    contact_t *contact;
    fired_missile_t *msl;
};

static unsigned int alr56_missile_cb(unsigned int _, void *vparam) {
    struct alr56_missile_cb_param *p = vparam;
    contact_remove_missile(p->contact, p->msl);
    free(p);
    return 0;
}

void alr56_missile(alr56_t *rwr, contact_t *contact, uint32_t timer) {
    if(contact->status != CONTACT_LOCK) {
        alr56_lock(rwr, contact);
    }

    fired_missile_t missile = fired_missile_new(contact->location);
    fired_missile_t *fired = contact_add_missile(contact, missile);
    
    struct alr56_missile_cb_param *param = malloc(sizeof(*param));
    param->contact = contact;
    param->msl = fired;
    SDL_AddTimer(timer, alr56_missile_cb, param);

    rwr->twp.missile_launch.blinks_remaining = ALR56_LAUNCH_REPETITIONS;
    alr56_blink_timer_set(&rwr->twp.missile_launch);

    tone_player_add_pri(rwr->tones, alr56_missile_tone());
}

void alr56_free(alr56_t *rwr) {
    SDL_RemoveTimer(rwr->periodic);

    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        contact_delete(rwr->contacts[i]);
    }

    free(rwr);
}

static unsigned int alr56_blink_cb(unsigned int _, void *vparam) {
    alr56_blink_common_t *blink = vparam;
    blink->light = !blink->light;
    blink->blinks_remaining -= 1;
    if(blink->blinks_remaining == 0) {
        blink->light = false;
        return 0;
    }

    return (int)(blink->period * 1000.f);
}

void alr56_blink_timer_set(alr56_blink_common_t *blink) {
    if(blink->timer != 0) {
        SDL_RemoveTimer(blink->timer);
        blink->timer = 0;
    }
    blink->timer = SDL_AddTimer((int)(blink->period * 1000.f), alr56_blink_cb, blink);
}

unsigned int alr56_periodic_cb(unsigned int _, void *vrwr) {
    alr56_t *rwr = (alr56_t*)vrwr;
    uint64_t time = SDL_GetTicks64();
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        contact_t *contact = &rwr->contacts[i];
        if(
            contact->source != NULL &&
            contact->status == CONTACT_SEARCH &&
            time - contact->search.last_ping >= ALR56_MS_BEFORE_DROP
        ) {
            alr56_drop_contact(rwr, contact);
        }
    }
    return ALR56_UPDATE_INTERVAL_MS;
}
