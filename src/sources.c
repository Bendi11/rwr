#include "rwr/source.h"

#define SOURCE(sym, string, rwrsym, loc, lethal, ...) [SOURCE_##sym] = (source_t){ \
    .name = string,                                                        \
    .rwr = rwrsym,                                                         \
    .id = SOURCE_##sym,                                                    \
    .location = loc,                                                       \
    .lethal_range = lethal,                                                \
    .radar = __VA_ARGS__                                                   \
},

source_t SOURCES[SOURCES_COUNT] = {
    #include "rwr/list.h"
};

#undef SOURCE
