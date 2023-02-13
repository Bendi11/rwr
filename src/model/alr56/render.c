#include "rwr.h"
#include "rwr/model/alr56.h"
#include "rwr/model/alr56/render.h"
#include "panels.h"
#include "rwr/source.h"


#include <SDL_render.h>
#include <SDL_rwops.h>
#include <SDL_image.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>
#include <math.h>

static SDL_Texture *scope_tx = NULL;
static SDL_Texture *twp = NULL;
static SDL_Texture *twa = NULL;
static SDL_Texture *icons[SOURCES_COUNT];
static SDL_Color icon_color = (SDL_Color){
    .r = 0,
    .g = 255,
    .b = 0,
    .a = 230
};

static TTF_Font *icon_font = NULL;

static void init_icons(SDL_Renderer *render);

static float clamp(float d, float v) {
  const double t = d < -v ? -v : d;
  return t > v ? v : t;
}

#define ALR56_MAX_DISTANCE (50.f)

void alr56_render_scope(alr56_t *rwr, SDL_Renderer *render) {
    if(scope_tx == NULL) {
        init_icons(render); 
    }

    SDL_RenderCopy(render, scope_tx, NULL, NULL);

    int w, h;
    SDL_GetRendererOutputSize(render, &w, &h);


    float icon_w = (float)w / 15.f;
    float icon_h = (float)h / 8.f;
    
    SDL_Rect dest;
    dest.h = (int)icon_h;
    dest.w = (int)icon_w;

    for(uint8_t i = 0; i < ALR56_MAX_CONTACTS; ++i) {
        contact_t *contact = &rwr->contacts[i];
        if(contact->source != NULL) {
            float dist = clamp(contact->location.distance / ALR56_MAX_DISTANCE, 1.f);
            float x = cosf(-contact->location.bearing + 1.57079632679) * dist;
            x *= (float)w / 2.f;
            x += (float)w / 2.f - icon_w / 2.f;

            float y = -sinf(-contact->location.bearing + 1.57079632679) * dist;
            y *= (float)h / 2.f;
            y += (float)h / 2.f - icon_h / 2.f;

            dest.x = (int)x;
            dest.y = (int)y;
            SDL_RenderCopy(render, icons[contact->source->id], NULL, &dest);
        }
    }

    SDL_RenderPresent(render);
}

static SDL_Texture* text(SDL_Renderer *render, const char *const txt) {
    SDL_Surface *txt_surface = TTF_RenderText_Blended(icon_font, txt, icon_color);
    SDL_Texture *tx = SDL_CreateTextureFromSurface(render, txt_surface);
    SDL_SetTextureAlphaMod(tx, 255);

    SDL_FreeSurface(txt_surface);
    return tx;
}

static void init_icons(SDL_Renderer *r) {
    if(icon_font == NULL) {
        SDL_RWops *rw = SDL_RWFromConstMem(RobotoMono_Bold_ttf, RobotoMono_Bold_ttf_size);
        icon_font = TTF_OpenFontRW(rw, 0, 24);
        if(icon_font == NULL) {
            perror(SDL_GetError());
        }
    }

    if(scope_tx == NULL) {
        SDL_RWops *rw = SDL_RWFromConstMem(F16_RWR_png, F16_RWR_png_size);
        SDL_Surface *img = IMG_Load_RW(rw, 0);
        scope_tx = SDL_CreateTextureFromSurface(r, img);

        SDL_FreeSurface(img);
    }

    icons[SOURCE_F16] = text(r, "16");
    icons[SOURCE_SA10] = text(r, "S");
}
