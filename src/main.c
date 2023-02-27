
#include "model/alr56/private.h"
#include "rwr.h"
#include "rwr/model/alr56.h"
#include "rwr/model/alr56/render.h"
#include "rwr/schedule/schedule.h"
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
    tone_player_set_volume(player, 1.f);

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = 44100;
    spec.format = AUDIO_F32;
    spec.channels = 1;
    spec.samples = 4096;
    spec.callback = test_cb;
    spec.userdata = player;

    SDL_AudioDeviceID id = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    SDL_PauseAudioDevice(id, 0);
    
    alr56_t *rwr = alr56_new(player);

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

        SDL_Delay(60);
    }

    rwr_schedule_free(&schedule);
    alr56_free(rwr);
        
    SDL_DestroyWindow(window);
    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}
