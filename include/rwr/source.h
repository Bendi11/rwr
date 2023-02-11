#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define SOURCE(sym, ...) SOURCE_##sym,

enum {
    #include "list.h"
    SOURCES_COUNT
};

#undef SOURCE

/// Audio profile for a source radar
typedef struct source_radar {
    /// Pulse Rate Frequency in hertz
    float prf;
    /// Time while pulses are transmitted in seconds
    float on_s;
    /// Time while pulses are received in seconds
    float off_s;
} source_radar_t;

enum {
    RADAR_SOURCE_SURFACE,
    RADAR_SOURCE_AIR,
};

typedef uint8_t source_location_t;

/// A single radar emitter containing constant characteristics of the radar
typedef struct source {
    const char *const name;
    source_location_t location;
    float lethal_range;
    const source_radar_t radar;
} source_t;


extern source_t SOURCES[SOURCES_COUNT];
