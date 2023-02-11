
#include "rwr.h"
#include "rwr/model/alr56.h"
#include <SDL2/SDL.h>
#include <SDL_audio.h>
#include <SDL_timer.h>

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

    tone_player_t *player = tone_player_new(44100);

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = 44100;
    spec.format = AUDIO_F32;
    spec.channels = 1;
    spec.samples = 4096;
    spec.callback = test_cb;
    spec.userdata = player;

    SDL_OpenAudio(&spec, NULL);
    
    alr56_t *rwr = alr56_new(player);
    contact_t *contact;
    if((contact = alr56_newguy(rwr, &SOURCES[SOURCE_SA10], (location_t){.bearing = 0, .distance = 10})) == NULL) {
        fprintf(stderr, "FAIL");
    }


    SDL_PauseAudio(0);
    SDL_Delay(2193);

    alr56_lock(rwr, contact);

    SDL_Delay(3976);

    contact_t *nc = alr56_newguy(rwr, &SOURCES[SOURCE_F16], (location_t){0});

    SDL_Delay(3470);

    alr56_missile(rwr, contact, 10);

    SDL_Delay(3819);

    alr56_lock(rwr, nc);

    SDL_Delay(2000);

    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}
