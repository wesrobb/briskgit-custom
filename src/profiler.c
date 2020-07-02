#include "profiler.h"

#include "eva/eva.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"

#define PROFILER_MAX_ZONES 5096

typedef struct profiler_zone {
    uint64_t start;
    uint64_t end;

    struct profiler_zone *parent;

    // The level at which the zone is in the hierarchy.
    // This is used to draw the zones in the UI at the right level
    // without requiring explicit parent/child pointers.
    uint32_t level;

    int32_t line_num;
    const char *name;
    const char *file;
} profiler_zone;

typedef struct profiler_ctx {
    // Stores a pointer to the currently active zone.
    profiler_zone *active;

    // Ring buffer of zones.
    profiler_zone *zones;
    uint32_t      zones_index;

    uint64_t last_logged_at;
} profiler_ctx;

static profiler_ctx g_profiler_ctx;

static void reset_zones(profiler_ctx *ctx)
{
    ctx->active = 0;
    ctx->zones_index = 0;
    memset(ctx->zones, 0, sizeof(profiler_zone) * PROFILER_MAX_ZONES);
}

static profiler_zone *activate_next_zone(profiler_ctx *ctx)
{
    uint32_t index = ctx->zones_index++;
    assert(index < PROFILER_MAX_ZONES);

    profiler_zone *zone = &ctx->zones[index];
    memset(zone, 0, sizeof(profiler_zone));

    if (ctx->active != 0) {
        zone->level  = ctx->active->level + 1;
        zone->parent = ctx->active;
    }

    ctx->active = zone;

    return zone;
}

static profiler_ctx *get_ctx()
{
    return &g_profiler_ctx;
}

void _profiler_init()
{
    profiler_ctx *ctx = get_ctx();
    ctx->zones = malloc(sizeof(profiler_zone) * PROFILER_MAX_ZONES);
    assert(ctx->zones);
    memset(ctx->zones, 0, PROFILER_MAX_ZONES);
}

void _profiler_log(uint32_t depth)
{
    profiler_ctx *ctx = get_ctx();
    if (!ctx)
        return;

    if (depth == 0) {
        depth = UINT_MAX;
    }

    uint64_t now = eva_time_now();

    if (eva_time_since_ms(ctx->last_logged_at) > 3000.0f) {
        char buf[1024];
        for (uint32_t i = 0; i < ctx->zones_index; i++) {
            profiler_zone *zone = &ctx->zones[i];
            if (zone->level >= depth) {
                continue;
            }
            int32_t buf_index = 0; 
            for (uint32_t j = 0; j < zone->level; ++j) {
                buf[buf_index++] = ' ';
                buf[buf_index++] = ' ';
                buf[buf_index++] = ' ';
                buf[buf_index++] = ' ';
            }
            int32_t x = snprintf(buf + buf_index, 
                                 1024 - buf_index, "%s: %f ms", 
                                 zone->name,
                                 eva_time_elapsed_ms(zone->start, zone->end));
            console_logn(buf, (size_t)(x + buf_index));
        }
        ctx->last_logged_at = now;
    }

    reset_zones(ctx);
}

int8_t
_profiler_begin_zone(const char *name, int32_t line_num, const char *fileName)
{
    profiler_ctx *ctx = get_ctx();
    if (!ctx)
        return 0;

    profiler_zone *zone = activate_next_zone(ctx);

    zone->name     = name;
    zone->line_num = line_num;
    zone->file     = fileName;
    zone->start    = eva_time_now();
    zone->end      = 0;

    return 0;
}

void _profiler_end_zone(int8_t profilerSentinel)
{
    profiler_ctx *ctx = get_ctx();
    if (!ctx)
        return;

    assert(ctx->active);

    (void)profilerSentinel;

    uint64_t no= eva_time_now();

    ctx->active->end = no;
    ctx->active = ctx->active->parent;
}
