#pragma once

#include "common.h"

void _profiler_init();
void _profiler_log(uint32_t maxLevel);
int8_t _profiler_begin_zone(const char *name, int32_t line_number, const char *file_name);
void _profiler_end_zone(int8_t profilerSentinel);

#ifdef PROFILER_ENABLED
#define profiler_init _profiler_init();
#define Profiler_Begin int8_t profilerSentinel = _profiler_begin_zone(__func__, __LINE__, __FILE__)
#define Profiler_BeginName(name) int8_t profilerSentinel = _profiler_begin_zone(name, __LINE__, __FILE__)
#define Profiler_End _profiler_end_zone(profilerSentinel)
#define Profiler_Log(maxLevel) _profiler_log(maxLevel)
#else
#define profiler_init
#define Profiler_Begin
#define Profiler_BeginName(name)
#define Profiler_End
#define Profiler_Log(maxLevel)
#endif


