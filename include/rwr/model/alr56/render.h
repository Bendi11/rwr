#pragma once
#include "../alr56.h"
#include <SDL_render.h>

void alr56_init_render(SDL_Renderer *renderer);

void alr56_render_scope(alr56_t *rwr, SDL_Renderer *surface);
