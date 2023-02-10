
#include "rwr.h"
#include <SDL2/SDL.h>
#include <SDL_audio.h>

void test_cb(void *userdat, uint8_t *buf, int len) {
    tone_player_t *player = userdat;
    float *fbuf = (float*)buf;
    len = len / sizeof(float);
    tone_player_fill_buf(player, fbuf, len);
}

int main(int argc, char *argv[]) {
    if(SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize stderr: %s\n", SDL_GetError());
        return -1;
    }

    tone_player_t *player = tone_player_new();
    tone_sequence_t *seq = TONE_SEQUENCE(
        TONE_SEQUENCE_LOOP,
        (tone_t[]){
            (tone_t){.frequency = 1000, .amplitude = 200, .length = 200 },
            (tone_t){.frequency = 500, .amplitude = 500, .length = 2 }
        }
    );

    tone_player_add(player, seq);

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = 44100;
    spec.format = AUDIO_F32;
    spec.channels = 1;
    spec.samples = 4096;
    spec.callback = test_cb;
    spec.userdata = player;

    SDL_OpenAudio(&spec, NULL);
    SDL_PauseAudio(0);

    SDL_Delay(5000);

    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}
