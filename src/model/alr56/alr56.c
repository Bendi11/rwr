#include "private.h"

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
    rwr->forgotten = NULL;

    return rwr;
}

contact_id_t alr56_newguy(alr56_t *rwr, const source_t *source, location_t location) {
    contact_t *contact = NULL;
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        if(rwr->contacts[i].source == NULL) {
            contact = &rwr->contacts[i];
            contact_new(contact, source, location, CONTACT_SEARCH);
            break;
        }
    }

    if(contact == NULL) {
        contact_t *tmp = malloc(sizeof(*contact));
        contact_new(tmp, source, location, CONTACT_SEARCH);
        contact = alr56_forget_contact_impl(rwr, tmp, true);
        free(tmp);
    }

    contact_id_t id = contact->id;

    if(!alr56_contact_forgotten_impl(rwr, contact)) {
        alr56_newguy_impl(rwr, contact);
    }

    return contact->id;
}

/// Check if the given contact has been forgotten by the RWR because it has not been pinged
bool alr56_contact_forgotten(alr56_t *rwr, const contact_id_t contact) {
    return alr56_contact_forgotten_impl(rwr, alr56_lookup_contact(rwr, contact));
}

/// Paint the RWR receiver with radar, updating the position on the display
void alr56_ping(alr56_t *rwr, contact_id_t contact, location_t loc) {
    alr56_ping_impl(rwr, alr56_lookup_contact(rwr, contact), loc);
}

/// Drop the given contact from the RWR, freeing the memory allocated for the contact
void alr56_drop(alr56_t *rwr, contact_id_t contact) {
    alr56_drop_impl(rwr, alr56_lookup_contact(rwr, contact));
}

/// Upgrade the given contact to an STT lock
void alr56_lock(alr56_t *rwr, contact_id_t contact) {
    alr56_lock_impl(rwr, alr56_lookup_contact(rwr, contact));
}

/// Launch a missile from the given target
fired_missile_t* alr56_missile(alr56_t *rwr, contact_id_t contact) {
    return alr56_missile_impl(rwr, alr56_lookup_contact(rwr, contact));
}

/// Drop the given RWR contact's lock if gained
void alr56_drop_lock(alr56_t *rwr, contact_id_t contact) {
    alr56_drop_lock_impl(rwr, alr56_lookup_contact(rwr, contact));
}

