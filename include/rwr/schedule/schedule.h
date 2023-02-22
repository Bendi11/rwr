#pragma once

#include <SDL_timer.h>

/// An RWR contact schedule made up of multiple timers
typedef struct schedule schedule_t;

schedule_t* rwr_sa10_schedule();
