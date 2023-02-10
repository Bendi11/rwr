#include "rwr.h"

void tone_player_fill_buf(tone_player_t *player, float *buf, int len) {
    tone_sequence_t *next = player->tones;
    while(next != NULL) {
        tone_t tone = next->tones[next->tone_idx];
        for(int i = 0; i < len; ++i) {
            if(tone.progress >= tone.length) {
                next->tones[next->tone_idx].progress = 0;
                next->tone_idx += 1;
                if(next->tone_idx >= next->tone_len) {
                    if(next->end_behavior == TONE_SEQUENCE_LOOP) {
                        next->tone_idx = 0;
                    }
                }
            }
        }
    }
}

void tone_player_remove_sequence(tone_sequence_t *seq) {

}
