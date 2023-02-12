#include "rwr.h"
#include <malloc.h>
#include <math.h>
#include <string.h>



void contact_add_missile(contact_t *contact, fired_missile_t missile) {
    if(contact->status != CONTACT_LOCK) { return; }

    fired_missile_t *next = contact->lock.missile;
    if(next == NULL) {
        *contact->lock.missile = missile;
    }

    while(next->next != NULL) {
        next = next->next;
    }

    *next->next = missile;
}

void contact_delete(contact_t contact) {
    if(contact.status == CONTACT_LOCK) {
        fired_missile_free(contact.lock.missile);
    }
}

void fired_missile_free(fired_missile_t *msl) {
    if(msl->next) {
        fired_missile_free(msl->next);
    }

    free(msl);
}
