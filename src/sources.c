#include "rwr/source.h"

#define SOURCE(sym, string, ...) [SOURCE_##sym] = (source_t){ .name = string, .radar = __VA_ARGS__ },

source_t SOURCES[SOURCES_COUNT] = {
    #include "rwr/list.h"
};

#undef SOURCE
#undef SOURCE_PASTE
