#include "rwr.h"
#include <malloc.h>
#include <math.h>
#include <string.h>

#define PI (3.14159)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

float clamp(float v, float lim) {
    if(v > lim) {
        return lim;
    } else if(v < -lim) {
        return -lim;
    } else {
        return v;
    }
}

static size_t write_tone(tone_player_t *player, float *buf, tone_t *tone, int len) {
    size_t samplesRemaining = (size_t)((tone->length - tone->progress) * player->sample_rate);
    samplesRemaining = MIN((size_t)len, samplesRemaining);
    for(size_t i = 0; i < samplesRemaining; i++) {
        buf[i] += clamp(sin(2 * PI * tone->progress * tone->frequency) * 300, 1) * tone->amplitude;
        tone->progress += player->sample_ts;
    }
    
    return samplesRemaining;
}

typedef struct tone_write_tuple {
    tone_sequence_t *next;
    size_t written_samples;
} tone_write_tuple_t;

static tone_write_tuple_t write_seq(tone_player_t *player, tone_sequence_t *next, float *buf, int len, bool priority) {
    size_t samples = 0;
    for(;;) {
        samples += write_tone(player, buf + samples, &next->tones[next->tone_idx], len - samples);
        if(samples < (size_t)len) {
            next->tones[next->tone_idx].progress = 0;
            next->tone_idx += 1;
            if(next->tone_idx >= next->tone_len) {
                switch(next->end_behavior.flag) {
                    case TONE_SEQUENCE_LOOP: {
                        next->tone_idx = 0;
                    } break;
                    
                    tone_seq_stop:
                    case TONE_SEQUENCE_STOP: {
                        tone_sequence_t *tmp = next;
                        next = next->next;
                        if(priority) {
                            tone_player_remove_pri(player, tmp);
                        } else {
                            tone_player_remove_sequence(player, tmp);
                        }
                        
                        return (tone_write_tuple_t){ .next = next, .written_samples = samples };
                    } break;

                    case TONE_SEQUENCE_LOOPFOR: {
                        next->end_behavior.loopfor.loops -= 1;
                        if(next->end_behavior.loopfor.loops <= 0) {
                            goto tone_seq_stop;
                        } else {
                            next->tone_idx = 0;
                        }
                    } break;
                }
            }
            continue;
        }

        return (tone_write_tuple_t){ .next = next->next, .written_samples = samples };
    }
}

void tone_player_fill_buf(tone_player_t *player, float *buf, int len) {
    memset(buf, 0, sizeof(float) * len);
    
    tone_sequence_t *next = player->priority;
    while(next != NULL && len > 0) {
        tone_write_tuple_t ret = write_seq(player, next, buf, len, true);
        next = ret.next;
        len -= ret.written_samples;
        buf += ret.written_samples;
        if(len == 0) { return; }
    }

    next = player->tones;

    while(next != NULL) { next = write_seq(player, next, buf, len, false).next; }
}

tone_player_t* tone_player_new(float sample_rate) {
    tone_player_t *player = malloc(sizeof(*player));
    player->tones = NULL;
    player->priority = NULL;
    player->sample_rate = sample_rate;
    player->sample_ts = 1.0f / player->sample_rate;
    return player;
}

static void tones_list_add(tone_sequence_t **tones, tone_sequence_t *seq) {
    if(*tones == NULL) {
        *tones = seq;
        return;
    }

    tone_sequence_t *node = *tones;
    while(node->next != NULL) { node = node->next; }
    node->next = seq;
}

static void tones_list_remove(tone_sequence_t **tones, tone_sequence_t *seq) {
    tone_sequence_t *next = *tones;
    if(next == seq) {
        *tones = next->next;
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

void tone_player_add(tone_player_t *player, tone_sequence_t *seq) { tones_list_add(&player->tones, seq); }
void tone_player_add_pri(tone_player_t *player, tone_sequence_t *seq) { tones_list_add(&player->priority, seq); }

void tone_player_remove_sequence(tone_player_t *player, tone_sequence_t *seq) { tones_list_remove(&player->tones, seq); }
void tone_player_remove_pri(tone_player_t *player, tone_sequence_t *seq) { tones_list_remove(&player->priority, seq); }

void tone_sequence_free(tone_sequence_t *seq) {
    free(seq->tones);
    free(seq);
}

tone_sequence_t* tone_sequence_new(tone_t *tones, uint8_t len, tone_sequence_end_t end_behavior) {
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
