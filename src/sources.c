#include "rwr/source.h"

#define SOURCE(sym, string) [SOURCE_##sym] = (source_t){ .name = string },

source_t SOURCES[SOURCES_COUNT] = {
    #include "rwr/list.h"
};

#undef SOURCE
