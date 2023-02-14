#include "rwr.h"
#include "rwr/model/alr56.h"
#include "rwr/model/alr56/render.h"
#include "panels.h"
#include "rwr/source.h"
#include "icons.h"


#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_rwops.h>
#include <SDL_image.h>
#include <SDL_surface.h>
#include <SDL_ttf.h>
#include <math.h>


static SDL_Texture *scope_tx = NULL;
static SDL_Texture *twp = NULL;
static SDL_Texture *twa = NULL;
static SDL_Texture *hat = NULL;
static SDL_Texture *icons[SOURCES_COUNT];
static SDL_Color icon_color = (SDL_Color){
    .r = 0,
    .g = 232,
    .b = 0,
    .a = 255
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
    float icon_h = (float)h / 15.f;
    
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

static inline SDL_Texture *texture(SDL_Renderer *r, const void *data, int sz) {
    SDL_RWops *rw = SDL_RWFromConstMem(data, sz);
    SDL_Surface *img = IMG_Load_RW(rw, SDL_FALSE);
    SDL_Texture *tx = SDL_CreateTextureFromSurface(r, img);

    SDL_FreeSurface(img);
    return tx;
}

static void init_icons(SDL_Renderer *r) {
    SDL_RWops *rw = SDL_RWFromConstMem(RobotoMono_SemiBold_ttf, RobotoMono_SemiBold_ttf_size);
    icon_font = TTF_OpenFontRW(rw, 0, 32);
    if(icon_font == NULL) {
        perror(SDL_GetError());
    }

    scope_tx = texture(r, F16_RWR_png, F16_RWR_png_size);
    twp = texture(r, F16_TWP_png, F16_TWP_png_size);
    twa = texture(r, F16_TWA_png, F16_TWA_png_size);
    hat = texture(r, airplane_hat_png, airplane_hat_png_size);

    for(uint16_t i = 0; i < SOURCES_COUNT; ++i) {
        source_t *src = &SOURCES[i];
        SDL_Surface *txt_surface = TTF_RenderText_Blended(icon_font, src->rwr, icon_color);
        int w = txt_surface->w;
        int h = txt_surface->h;
        SDL_Texture *tx = SDL_CreateTextureFromSurface(r, txt_surface);
        SDL_FreeSurface(txt_surface);
        SDL_SetTextureAlphaMod(tx, 255);

        if(src->location == RADAR_SOURCE_AIR) {
            SDL_Rect dest = {
                .x = 48,
                .y = 0,
                .h = 64,
                .w = 160
            };

            SDL_Texture* auxtexture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 256, 256 + 22);
            SDL_SetTextureBlendMode(auxtexture, SDL_BLENDMODE_BLEND);
            SDL_SetRenderTarget(r, auxtexture);
            SDL_RenderClear(r);
            
            SDL_RenderCopy(r, hat, NULL, &dest);

            SDL_Rect text = {
                .x = 0,
                .y = 22,
                .h = 256,
                .w = 256
            };
            SDL_RenderCopy(r, tx, NULL, &text);
            SDL_DestroyTexture(tx);

            SDL_RenderPresent(r);
            tx = auxtexture;
        }
 
        SDL_SetRenderTarget(r, NULL);

        icons[i] = tx;
    }
}
