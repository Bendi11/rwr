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

void alr56_newguy_impl(alr56_t *rwr, contact_t *contact) {
    rwr->latest = contact;
    if(rwr->twa.search) {
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
    }

    alr56_recompute_priority(rwr);
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
    contact_t *highest = NULL;
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

void alr56_drop_lock_impl(alr56_t *rwr, contact_t *contact) {
    if(contact->status == CONTACT_LOCK) {
        fired_missile_free(contact->lock.missiles);
        contact->lock.missiles = NULL;
        contact->status = CONTACT_SEARCH;
        contact->search.last_ping = SDL_GetTicks64();
        contact->location = contact->location;
        if(rwr->priority.contact == contact) {
            alr56_clear_priority(rwr);
            alr56_recompute_priority(rwr); 
        }
    }
}

void alr56_drop_impl(alr56_t *rwr, contact_t *contact) {
    alr56_drop_lock_impl(rwr, contact);
    contact_delete(*contact);

    if(alr56_contact_forgotten_impl(rwr, contact)) {
        alr56_remove_forgotten(rwr, contact);
        return;
    }
    contact->source = NULL;

    if(rwr->priority.contact == contact) {
        alr56_recompute_priority(rwr); 
    }

    if(rwr->latest == contact) {
        rwr->latest = NULL;
    }
}

contact_t* alr56_ping_impl(alr56_t *rwr, contact_t *contact, location_t loc) {
    contact = alr56_remember_contact(rwr, contact);
    contact->location = loc;
    if(contact->status == CONTACT_SEARCH) {
        contact->search.last_ping = SDL_GetTicks64();
    }

    return contact;
}

void alr56_lock_impl(alr56_t *rwr, contact_t *contact) {
    if(contact->status == CONTACT_LOCK) { return; }
   
    contact->status = CONTACT_LOCK;
    contact->lock.missiles = NULL;
    if(rwr->twp.handoff_mode == ALR56_HANDOFF_DIAMOND_FLOAT) {
        alr56_recompute_priority(rwr);
    }
}

struct alr56_missile_cb_param {
    alr56_t *rwr;
    contact_t *contact;
    fired_missile_t *msl;
};

static unsigned int alr56_missile_recycle_cb(unsigned int _, void *vparam) {
    struct alr56_missile_cb_param *p = vparam;
    if(!contact_has_missile(p->contact, p->msl)) { free(p); return 0; }
    
    p->rwr->twp.missile_launch.blinks_remaining = ALR56_LAUNCH_REPETITIONS;
    alr56_blink_timer_set(&p->rwr->twp.missile_launch);

    tone_player_add_pri(p->rwr->tones, alr56_missile_recycle_tone());

    return ALR56_MISSILE_RECYCLE_INTERVAL_MS;
}

fired_missile_t* alr56_missile_impl(alr56_t *rwr, contact_t *contact) {
    if(contact->status != CONTACT_LOCK) {
        alr56_lock_impl(rwr, contact);
    }

    fired_missile_t missile = fired_missile_new(contact->location);
    fired_missile_t *fired = contact_add_missile(contact, missile);
    
    struct alr56_missile_cb_param *param = malloc(sizeof(*param));
    param->contact = contact;
    param->msl = fired;
    param->rwr = rwr;
    SDL_AddTimer(1, alr56_missile_recycle_cb, param);

    return fired;
}

void alr56_free(alr56_t *rwr) {
    SDL_RemoveTimer(rwr->periodic);

    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        contact_delete(rwr->contacts[i]);
    }

    while(rwr->forgotten != NULL) {
        contact_delete(rwr->forgotten->contact);
        alr56_remove_forgotten(rwr, &rwr->forgotten->contact);
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
            alr56_forget_contact_impl(rwr, contact, false);
        }
    }
    return ALR56_UPDATE_INTERVAL_MS;
}

bool alr56_contact_forgotten_impl(alr56_t *rwr, const contact_t *contact) {
    return contact < rwr->contacts || (contact > (rwr->contacts + ALR56_MAX_CONTACTS));
}

contact_t* alr56_forget_contact_impl(alr56_t *rwr, contact_t *contact, bool force) {
    if(!force && alr56_contact_forgotten_impl(rwr, contact)) { return contact; }

    struct alr56_forgotten_link *forgotten = malloc(sizeof(*forgotten));
    forgotten->contact = *contact;
    forgotten->next = NULL;
    contact->source = NULL;

    if(rwr->priority.contact == contact) {
        alr56_recompute_priority(rwr);
    }

    if(rwr->forgotten == NULL) {
        rwr->forgotten = forgotten;
    } else {
        struct alr56_forgotten_link *node = rwr->forgotten;
        while(node->next != NULL) { node = node->next; }
        node->next = forgotten;
    }

    return &forgotten->contact;
}

contact_t *alr56_remember_contact(alr56_t *rwr, contact_t *contact) {
    if(!alr56_contact_forgotten_impl(rwr, contact) || rwr->forgotten == NULL) { return contact; }

    contact_t *empty_slot = NULL;
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        if(rwr->contacts[i].source == NULL) {
            empty_slot = &rwr->contacts[i];
            break;
        }
    }

    if(empty_slot == NULL) { return contact; }
    
    *empty_slot = *contact;
    alr56_remove_forgotten(rwr, contact);
    
    return empty_slot;
}

void alr56_remove_forgotten(alr56_t *rwr, contact_t *contact) {
    if(&rwr->forgotten->contact == contact) {
        struct alr56_forgotten_link *tmp = rwr->forgotten;
        rwr->forgotten = rwr->forgotten->next;
        free(tmp);
    } else {
        struct alr56_forgotten_link *node = rwr->forgotten;
        while(node->next != NULL && &node->next->contact != contact) {
            node = node->next;
        }

        struct alr56_forgotten_link *tmp = node->next;
        node->next = tmp->next;
        free(tmp);
    }
}

contact_t* alr56_lookup_contact(alr56_t *rwr, const contact_id_t id) {
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        if(rwr->contacts[i].source != NULL && rwr->contacts[i].id == id) {
            return &rwr->contacts[i];
        }
    }

    struct alr56_forgotten_link *node = rwr->forgotten;
    while(node != NULL) {
        if(node->contact.id == id) {
            return &node->contact;
        }

        node = node->next;
    }

    fprintf(stderr, "alr56_lookup_contact called with invalid ID %u\n", id);
    exit(-1);
}
