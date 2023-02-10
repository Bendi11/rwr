
#include "rwr.h"
#include "rwr/model/alr56.h"
#include <SDL2/SDL.h>
#include <SDL_audio.h>

void test_cb(void *userdat, uint8_t *buf, int len) {
    tone_player_t *player = userdat;
    float *fbuf = (float*)buf;
    len = len / sizeof(float);
    tone_player_fill_buf(player, fbuf, len);
}

tone_sequence_t *newguy_air_tone = NULL;

int main(int argc, char *argv[]) {
    if(newguy_air_tone == NULL) {
        newguy_air_tone = TONE_SEQUENCE(
            (tone_sequence_end_t){TONE_SEQUENCE_STOP},
            (tone_t[]){
                (tone_t){ .frequency = 3000, .amplitude = 10, .length = 0.025 },
                (tone_t){ .amplitude = 0, .length = 0.025 },

                (tone_t){ .frequency = 3000, .amplitude = 10, .length = 0.025 },
                (tone_t){ .amplitude = 0, .length = 0.025 },
            }
        );
    }

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
    if(alr56_newguy(rwr, &SOURCES[SOURCE_F16], (location_t){.bearing = 0, .distance = 10}) == NULL) {
        fprintf(stderr, "FAIL");
    }


    SDL_PauseAudio(0);

    tone_sequence_t *seq = TONE_SEQUENCE(
        TONE_SEQUENCE_END(TONE_SEQUENCE_LOOP),
        (tone_t[]){
            (tone_t){.frequency = 856, .amplitude = 10, .length = 0.1, .progress = 0 },
            (tone_t){.frequency = 0, .amplitude = 0, .length =  0.1, .progress = 0 }
        }
    );
    

    tone_player_add(rwr->tones, newguy_air_tone);
    //tone_player_add(player, seq);



    SDL_Delay(4000);

    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}
