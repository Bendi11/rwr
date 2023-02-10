
#include <SDL2/SDL.h>


int main(int argc, char *argv[]) {
    if(SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize stderr: %s\n", SDL_GetError());
        return -1;
    }
}
