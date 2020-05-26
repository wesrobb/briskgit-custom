#include "profiler.h"
#include "SDL_thread.h"

#include <limits.h>
#include <string.h>
#include <stdio.h>

#include <SDL.h>

#define PROFILER_MAX_ZONES 5096

typedef struct ProfilerZone {
    uint64_t startTime;
    uint64_t endTime;

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
    // Stores a pointer to the currently active zone.
    ProfilerZone *activeZone;

    // Ring buffer of zones.
    ProfilerZone *zones;
    uint32_t zonesIndex;

    uint64_t lastLoggedAt;
} ProfilerContext;

// Profiler contexts are created per thread so this allows 16 different threads
// to have profiler contexts.
#define PROFILER_MAX_CONTEXTS 16
static SDL_threadID g_profilerContextThreadIds[PROFILER_MAX_CONTEXTS];
static ProfilerContext g_profilerContexts[PROFILER_MAX_CONTEXTS];
static int32_t g_profilerContextsIndex;

static double ElapsedMs(uint64_t start, uint64_t end)
{
    SDL_assert(end >= start);
    double result = (double)((end - start)*1000) / SDL_GetPerformanceFrequency();
    return result;
}

static void ResetZones(ProfilerContext *ctx)
{
    ctx->activeZone = 0;
    ctx->zonesIndex = 0;
    memset(ctx->zones, 0, sizeof(ProfilerZone) * PROFILER_MAX_ZONES);
}

static ProfilerZone * ActivateNextZone(ProfilerContext *ctx)
{
    uint32_t index = ctx->zonesIndex++;
    SDL_assert(index < PROFILER_MAX_ZONES);

    ProfilerZone *zone = &ctx->zones[index];
    memset(zone, 0, sizeof(ProfilerZone));

    if (ctx->activeZone != 0) {
        zone->level = ctx->activeZone->level + 1;
        zone->parent = ctx->activeZone;
    }

    ctx->activeZone = zone;

    return zone;
}

void Init(ProfilerContext *ctx)
{
    ctx->zones = malloc(sizeof(ProfilerZone) * PROFILER_MAX_ZONES);
    SDL_assert(ctx->zones);
    memset(ctx->zones, 0, PROFILER_MAX_ZONES);
}


static ProfilerContext *GetContext()
{
    // TODO: Use TLS for this.
    SDL_threadID currentThreadId = SDL_GetThreadID(NULL);

    // See if we have an existing ID that matches.
    for (int32_t i = 0; i < g_profilerContextsIndex; i++)
    {
        if (g_profilerContextThreadIds[i] == currentThreadId)
        {
            return &g_profilerContexts[i];
        }
    }

    // TODO: Remove this limit that sticks us on the main thread only.
    // This is done because the profiler does not support multi-threading
    // ... yet.
    if (g_profilerContextsIndex == 1)
    {
        return NULL;
    }

    // No context found - make one.
    int32_t index = g_profilerContextsIndex++;
    SDL_assert(index < PROFILER_MAX_CONTEXTS);
    g_profilerContextThreadIds[index] = currentThreadId;
    ProfilerContext *ctx = &g_profilerContexts[index];

    Init(ctx);
    return ctx;
}

void Profiler_Log(uint32_t maxLevel)
{
    ProfilerContext *ctx = GetContext();
    if (!ctx) return;

    if (maxLevel == 0)
    {
        maxLevel = UINT_MAX;
    }

    uint64_t currentTime = SDL_GetPerformanceCounter();

    if (ElapsedMs(ctx->lastLoggedAt, currentTime) > 1000.0f)
    {
        for (uint32_t i = 0; i < ctx->zonesIndex; i++)
        {
            ProfilerZone *zone = &ctx->zones[i];
            if (zone->level >= maxLevel)
            {
                continue;
            }
            for (uint32_t j = 0; j < zone->level; ++j) {
                printf("  ");
            }
            printf("%s: %f ms\n", zone->name, ElapsedMs(zone->startTime, zone->endTime));

        }
        ctx->lastLoggedAt = currentTime;
    }

    ResetZones(ctx);
}


int32_t Profiler_BeginZone(const char *name, int32_t lineNum, const char *fileName)
{
    ProfilerContext *ctx = GetContext();
    if (!ctx) return 0;

    ProfilerZone *zone = ActivateNextZone(ctx);
    zone->name = name;
    zone->lineNum = lineNum;
    zone->file = fileName;
    zone->startTime = SDL_GetPerformanceCounter();
    zone->endTime = 0;

    return 0;
}

void Profiler_EndZone(int32_t profilerSentinel)
{
    ProfilerContext *ctx = GetContext();
    if (!ctx) return;

    SDL_assert(ctx->activeZone);

    (void)profilerSentinel;

    uint64_t currentTime = SDL_GetPerformanceCounter();


    ctx->activeZone->endTime = currentTime;
    ctx->activeZone = ctx->activeZone->parent;
}
