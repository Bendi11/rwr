#include "rwr/source.h"
#include <icons.h>

#define SOURCE(sym, string, loc, icon, lethal, ...) [SOURCE_##sym] = (source_t){ \
    .name = string,                                                        \
    .id = SOURCE_##sym,                                                    \
    .location = loc,                                                       \
    .icon_svg = icon,                                                      \
    .lethal_range = lethal,                                                \
    .radar = __VA_ARGS__                                                   \
},

source_t SOURCES[SOURCES_COUNT] = {
    #include "rwr/list.h"
};

#undef SOURCE
