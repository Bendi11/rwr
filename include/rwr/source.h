#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define SOURCE(sym, ...) SOURCE_##sym,

typedef uint16_t source_idx_t;

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
    /// SAM site most lethal to high altitude targets
    RADAR_SOURCE_SURFACE_HIGH_ALT,
    /// SAM site most lethal to low altitude targets
    RADAR_SOURCE_SURFACE_LOW_ALT,
    RADAR_SOURCE_AIR,
};

typedef uint8_t source_location_t;

/// A single radar emitter containing constant characteristics of the radar
typedef struct source {
    const char *const name;
    const char *const rwr;
    uint16_t id;
    source_location_t location;
    float lethal_range;
    const source_radar_t radar;
} source_t;


extern source_t SOURCES[SOURCES_COUNT];
