#pragma once

#include "rwr/model/alr56.h"
#include "rwr/schedule/list.h"

/// An RWR contact schedule made up of multiple timers
typedef struct schedule {
    timerlist_t timers;
    alr56_t *rwr;
} schedule_t;

