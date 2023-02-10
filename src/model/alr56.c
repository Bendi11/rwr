#include "rwr/model/alr56.h"
#include "rwr.h"

#include <SDL2/SDL.h>

static tone_player_t tones;


static void tone_callback(void *userdata, uint8_t *stream, int len) {

}

void alr56_new(alr56_t *rwr) {
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        rwr->contacts[i].source = NULL;
    }
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

    return contact;
}

void alr56_lock(alr56_t *rwr, contact_t *contact) {
    
}
