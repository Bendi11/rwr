#include "rwr.h"
#include <SDL_timer.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

void contact_new(contact_t *contact, const source_t *source, location_t loc, contact_status_t status) {
    *contact = (contact_t){
        .location = loc,
        .status = status,
        .search = {
            .last_ping = SDL_GetTicks64(),
        },
        .source = source
    };
}

fired_missile_t* contact_add_missile(contact_t *contact, fired_missile_t copy) {
    if(contact->status != CONTACT_LOCK) { return NULL; }
    fired_missile_t *missile = malloc(sizeof(*missile));
    memcpy(missile, &copy, sizeof(copy));
    missile->next = NULL;

    fired_missile_t *next = contact->lock.missiles;
    if(next == NULL) {
        contact->lock.missiles = missile;
        return missile;
    }

    while(next->next != NULL) { next = next->next; }

    next->next = missile;
    return missile;
}

void contact_delete(contact_t contact) {
    if(contact.status == CONTACT_LOCK) {
        fired_missile_free(contact.lock.missiles);
    }
}

void fired_missile_free(fired_missile_t *msl) {
    if(msl == NULL) { return; }

    if(msl->next) {
        fired_missile_free(msl->next);
    }

    if(msl->timer != 0) {
        SDL_RemoveTimer(msl->timer);
    }

    free(msl);
}

void contact_remove_missile(contact_t *contact, fired_missile_t *missile) {
    if(contact->status != CONTACT_LOCK) { return; }
    fired_missile_t *msl = contact->lock.missiles;
    if(msl == missile) {
        contact->lock.missiles = missile->next;
    } else {
        while(msl != NULL && msl->next != missile) {
            msl = msl->next;
        }

        if(msl == NULL) { return; }
        msl->next = missile->next;
    }

    missile->next = NULL;
    fired_missile_free(missile);

}

fired_missile_t fired_missile_new(location_t loc) {
    return (fired_missile_t){
        .location = loc,
        .next = NULL
    };
}

size_t contact_missiles_count(contact_t *contact) {
    if(contact->status != CONTACT_LOCK) { return 0; }

    size_t count = 0;
    fired_missile_t *next = contact->lock.missiles;
    while(next != NULL) {
        next = next->next;
        count += 1;
    }

    return count;
}
