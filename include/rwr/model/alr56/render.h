#pragma once
#include "../alr56.h"
#include <SDL_render.h>

/// Render all RWR contacts on the circular scope
void alr56_render_scope(alr56_t *rwr, SDL_Renderer *render);

/// Render the Threat Warning Prime panel for the current time
void alr56_render_twp(alr56_t *rwr, SDL_Renderer *render);
