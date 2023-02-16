
#include "rwr.h"
#include "rwr/model/alr56.h"
#include "rwr/model/alr56/render.h"
#include "rwr/tones.h"
#include <SDL2/SDL.h>
#include <SDL_audio.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include <SDL_video.h>

void test_cb(void *userdat, uint8_t *buf, int len) {
    tone_player_t *player = userdat;
    float *fbuf = (float*)buf;
    len = len / sizeof(float);
    tone_player_fill_buf(player, fbuf, len);
}

int main(int argc, char *argv[]) {
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    TTF_Init();

    tone_player_t *player = tone_player_new(44100);
    tone_player_set_volume(player, 0.05);

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
    contact = alr56_newguy(rwr, &SOURCES[SOURCE_F16], (location_t){.bearing = 45 * 3.14195f / 180.f, .distance = 25});
    alr56_lock(rwr, contact);
    alr56_newguy(rwr, &SOURCES[SOURCE_SA10], (location_t){.bearing = 195 * 3.14195f / 180.f, .distance = 37});


    SDL_Window *window = NULL;
    SDL_Renderer *render = NULL;
    SDL_CreateWindowAndRenderer(600, 600, 0, &window, &render);
    SDL_SetWindowTitle(window, "RWR");
    SDL_Event event;
    bool run = true;
    while(run) {
        SDL_RenderClear(render);
        alr56_render_scope(rwr, render);
        SDL_RenderPresent(render);

        while(SDL_PollEvent(&event) == 1) {
            if(event.type == SDL_QUIT) {
                run = false;
                break;
            }
        }

        SDL_Delay(16);

        //alr56_ping(rwr, contact, (location_t){ .bearing = 1.47, .distance = 15 });
    }

    /*SDL_PauseAudio(0);
    SDL_Delay(2193);

    alr56_lock(rwr, contact);

    SDL_Delay(3976);

    contact_t *nc = alr56_newguy(rwr, &SOURCES[SOURCE_F16], (location_t){0});

    SDL_Delay(3470);

    alr56_missile(rwr, contact, 10);

    SDL_Delay(3819);

    alr56_lock(rwr, nc);

    SDL_Delay(2000);*/
    
    SDL_DestroyWindow(window);
    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}
