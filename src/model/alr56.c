#include "rwr/model/alr56.h"

void alr56_new(alr56_t *rwr) {
    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        rwr->contacts[i].source = NULL;
    }
}
