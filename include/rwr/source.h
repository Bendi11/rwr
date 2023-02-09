#pragma once

#define SOURCE(sym, _) SOURCE_##sym,

enum {
    #include "list.h"
    SOURCES_COUNT
};

#undef SOURCE

/// A single radar emitter containing constant characteristics of the radar
typedef struct source {
    const char *const name;
} source_t;

extern source_t SOURCES[SOURCES_COUNT];
