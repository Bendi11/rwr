#include "rwr/source.h"

#define SOURCE(sym, string, loc, ...) [SOURCE_##sym] = (source_t){ .name = string, .location = loc, .radar = __VA_ARGS__ },

source_t SOURCES[SOURCES_COUNT] = {
    #include "rwr/list.h"
};

#undef SOURCE
#undef SOURCE_PASTE
