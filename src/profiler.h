#pragma once

#include "common.h"

void Profiler_Init();
void Profiler_Log();

int Profiler_BeginZone(const char *name, int32_t line_number, const char *file_name);
void Profiler_EndZone(int profilerSentinel);

#define Profiler_Begin int profilerSentinel = Profiler_BeginZone(__func__, __LINE__, __FILE__)
#define Profiler_BeginName(name) int profilerSentinel = Profiler_BeginZone(name, __LINE__, __FILE__)
#define Profiler_End Profiler_EndZone(profilerSentinel)


