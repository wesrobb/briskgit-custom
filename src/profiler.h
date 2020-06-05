#pragma once

#include "common.h"

void _profiler_init();
void _profiler_log(uint32_t depth);
int8_t _profiler_begin_zone(const char *name, int32_t line_number, const char *file_name);
void _profiler_end_zone(int8_t sentinel);

#ifdef PROFILER_ENABLED
#define profiler_init _profiler_init();
#define profiler_begin \
    int8_t sentinel = _profiler_begin_zone(__func__, __LINE__, __FILE__)
#define profiler_begin_name(name) \
    int8_t sentinel = _profiler_begin_zone(name, __LINE__, __FILE__)
#define profiler_end _profiler_end_zone(sentinel)
#define profiler_log(depth) _profiler_log(depth)
#else
#define profiler_init
#define profiler_begin
#define profiler_begin_name(name)
#define profiler_end
#define profiler_log(depth)
#endif


