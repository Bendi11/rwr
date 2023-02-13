#include "rwr.h"
#include "rwr/model/alr56.h"
#include "rwr/model/alr56/render.h"
#include "panels.h"


#include <SDL_render.h>
#include <SDL_rwops.h>
#include <SDL_image.h>

static SDL_Texture *scope_tx = NULL;
static SDL_Texture *twp = NULL;
static SDL_Texture *twa = NULL;


void alr56_render_scope(alr56_t *rwr, SDL_Renderer *renderer) {
    int w, h;
    if(scope_tx == NULL) {
        SDL_RWops *rw = SDL_RWFromConstMem(F16_TWP_png, F16_TWP_png_size);
        SDL_RenderGetLogicalSize(renderer, &w, &h);
        SDL_Surface *img = IMG_LoadSizedSVG_RW(rw, w, h);
        scope_tx = SDL_CreateTextureFromSurface(renderer, img);
    }
}
