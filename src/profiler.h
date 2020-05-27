#pragma once

#include "common.h"

void _Profiler_Log(uint32_t maxLevel);
int _Profiler_BeginZone(const char *name, int32_t line_number, const char *file_name);
void _Profiler_EndZone(int profilerSentinel);

#ifdef PROFILER_ENABLED
#define Profiler_Begin int profilerSentinel = _Profiler_BeginZone(__func__, __LINE__, __FILE__)
#define Profiler_BeginName(name) int profilerSentinel = _Profiler_BeginZone(name, __LINE__, __FILE__)
#define Profiler_End _Profiler_EndZone(profilerSentinel)
#define Profiler_Log(maxLevel) _Profiler_Log(maxLevel)
#else
#define Profiler_Begin
#define Profiler_BeginName(name)
#define Profiler_End
#define Profiler_Log(maxLevel)
#endif


