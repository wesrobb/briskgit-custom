#include "profiler.h"

#include <limits.h>
#include <string.h>
#include <stdio.h>

#include <SDL.h>

#define PROFILER_MAX_ZONES 256

typedef struct ProfilerZone {
    uint64_t startTime;
    uint64_t endTime;

    struct ProfilerZone *childZones[32];
    uint32_t childIndex;

    struct ProfilerZone *parent;

    // The level at which the zone is in the hierarchy.
    // This is used to draw the zones in the UI at the right level
    // without requiring explicit parent/child pointers.
    uint32_t level;

    int32_t lineNum;
    const char *name;
    const char *file;
} ProfilerZone;

typedef struct ProfilerContext {
    bool initialized;

    // Stores a pointer to the currently active zone.
    ProfilerZone *activeZone;

    // Ring buffer of zones.
    ProfilerZone *zones;
    uint32_t zonesIndex;

    uint64_t lastLoggedAt;
} ProfilerContext;

ProfilerContext g_profilerContext;
bool g_paused;

static double ElapsedMs(uint64_t start, uint64_t end)
{
    SDL_assert(end >= start);
    double result = (double)((end - start)*1000) / SDL_GetPerformanceFrequency();
    return result;
}

static void ResetZones()
{
    g_profilerContext.activeZone = 0;
    g_profilerContext.zonesIndex = 0;
    memset(g_profilerContext.zones, 0, sizeof(ProfilerZone) * PROFILER_MAX_ZONES);
}

static ProfilerZone * ActivateNextZone()
{
    uint32_t index = g_profilerContext.zonesIndex++ % PROFILER_MAX_ZONES;

    ProfilerZone *zone = &g_profilerContext.zones[index];
    memset(zone, 0, sizeof(ProfilerZone));

    if (g_profilerContext.activeZone != 0) {
        zone->level = g_profilerContext.activeZone->level + 1;
        zone->parent = g_profilerContext.activeZone;
    }

    g_profilerContext.activeZone = zone;

    return zone;
}

void Profiler_Init()
{
    SDL_assert(!g_profilerContext.initialized);

    g_profilerContext.zones = malloc(sizeof(ProfilerZone) * PROFILER_MAX_ZONES);
    SDL_assert(g_profilerContext.zones);
    memset(g_profilerContext.zones, 0, PROFILER_MAX_ZONES);
    g_profilerContext.initialized = true;
}

void Profiler_Log(uint32_t maxLevel)
{
    SDL_assert(g_profilerContext.initialized);

    if (maxLevel == 0)
    {
        maxLevel = UINT_MAX;
    }

    uint64_t currentTime = SDL_GetPerformanceCounter();

    if (ElapsedMs(g_profilerContext.lastLoggedAt, currentTime) > 1000.0f)
    {
        for (uint32_t i = 0; i < g_profilerContext.zonesIndex; i++)
        {
            ProfilerZone *zone = &g_profilerContext.zones[i];
            if (zone->level >= maxLevel)
            {
                continue;
            }
            for (uint32_t j = 0; j < zone->level; ++j) {
                printf("  ");
            }
            printf("%s: %f ms\n", zone->name, ElapsedMs(zone->startTime, zone->endTime));

        }
        g_profilerContext.lastLoggedAt = currentTime;
    }

    ResetZones();
}


int32_t Profiler_BeginZone(const char *name, int32_t lineNum, const char *fileName)
{
    SDL_assert(g_profilerContext.initialized);

    ProfilerZone *zone = ActivateNextZone();
    zone->name = name;
    zone->lineNum = lineNum;
    zone->file = fileName;
    zone->startTime = SDL_GetPerformanceCounter();

    return 0;
}

void Profiler_EndZone(int32_t profilerSentinel)
{
    SDL_assert(g_profilerContext.initialized);
    SDL_assert(g_profilerContext.activeZone);

    (void)profilerSentinel;

    uint64_t currentTime = SDL_GetPerformanceCounter();


    g_profilerContext.activeZone->endTime = currentTime;
    g_profilerContext.activeZone = g_profilerContext.activeZone->parent;
}
