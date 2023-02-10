#include "rwr.h"
#include <malloc.h>
#include <math.h>
#include <string.h>

#define PI (3.14159)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

void tone_player_fill_buf(tone_player_t *player, float *buf, int len) {
    memset(buf, 0, sizeof(float) * len);

    tone_sequence_t *next = player->tones;
o:  while(next != NULL) {
        tone_t tone = next->tones[next->tone_idx];
        size_t samplesRemaining = ((tone.length - tone.progress) * (float)44100);
        for(size_t i = 0; i < MIN((size_t)len, samplesRemaining); ++i) {
            buf[i] = sin(2 * PI * tone.progress * tone.frequency) * tone.amplitude;
            tone.progress += 1 / (float)44100;
        }


        if(samplesRemaining <= (size_t)len) {
            next->tones[next->tone_idx].progress = 0;
            next->tone_idx += 1;
            tone = next->tones[next->tone_idx];
            tone.progress = 0;
            if(next->tone_idx >= next->tone_len) {
                if(next->end_behavior == TONE_SEQUENCE_LOOP) {
                    next->tone_idx = 0;
                } else {
                    tone_sequence_t *tmp = next;
                    next = next->next;
                    tone_player_remove_sequence(player, tmp);
                }
            }
            continue;
        }

        next->tones[next->tone_idx] = tone;
        next = next->next;
    }
}

tone_player_t* tone_player_new() {
    tone_player_t *player = malloc(sizeof(*player));
    player->tones = NULL;
    return player;
}

void tone_player_add(tone_player_t *player, tone_sequence_t *seq) {
    if(player->tones == NULL) {
        player->tones = seq;
        return;
    }

    tone_sequence_t *node = player->tones;
    while(node->next != NULL) { node = node->next; }
    node->next = seq;
}

void tone_player_remove_sequence(tone_player_t *player, tone_sequence_t *seq) {
    tone_sequence_t *next = player->tones;
    if(next == seq) {
        player->tones = next->next;
        tone_sequence_free(next);
    }

    while(next != NULL) {
        if(next->next == seq) {
            next->next = seq->next;
            tone_sequence_free(seq);
        }

        next = next->next;
    }
}

void tone_sequence_free(tone_sequence_t *seq) {
    free(seq->tones);
    free(seq);
}

tone_sequence_t* tone_sequence_new(tone_t *tones, uint8_t len, uint8_t end_behavior) {
    tone_sequence_t *seq = malloc(sizeof(*seq));
    seq->tones = calloc(len, sizeof(tone_t));
    memcpy(seq->tones, tones, len * sizeof(*tones));
    seq->next = NULL;
    seq->tone_idx = 0;
    seq->end_behavior = end_behavior;
    seq->tone_len = len;
    return seq;
}

void tone_player_free(tone_player_t *player) {
    tone_sequence_t *next = player->tones;
    while(next != NULL) {
        tone_sequence_t *tmp = next->next;
        tone_sequence_free(next);
        next = tmp;
    }

    free(player);
}
