#include "render.h"

#include <assert.h>
#include <blend2d/api.h>
#include <math.h>
#include <string.h>

#define BL_STATIC
#include <blend2d.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>

#include "eva/eva.h"

#include "color.h"
#include "common.h"
#include "hash.h"
#include "profiler.h"
#include "rect.h"
#include "text.h"
#include "vec2.h"

typedef enum render_cmd_type {
    RENDER_COMMAND_RECT,
    RENDER_COMMAND_TEXT
} render_cmd_type;

typedef struct render_cmd_rect {
    recti rect;
    color color;
} render_cmd_rect;

#define MAX_TEXT_LEN 1000
typedef struct render_cmd_text {
    recti clip;
    recti bbox;
    text *t;
} render_cmd_text;

typedef struct render_cmd {
    render_cmd_type type;
    union {
        render_cmd_rect rect_cmd;
        render_cmd_text text_cmd;
    };
} render_cmd;

#define RENDER_COMMAND_QUEUE_SIZE 1000
typedef struct render_cmd_ctx {
    render_cmd cmds1[RENDER_COMMAND_QUEUE_SIZE];
    render_cmd cmds2[RENDER_COMMAND_QUEUE_SIZE];
    int32_t cmds_index1;
    int32_t cmds_index2;

    render_cmd *current;
    render_cmd *previous;
    int32_t *curr_index;
    int32_t *prev_index;
} render_cmd_ctx;
static render_cmd_ctx _render_cmd_ctx;

static BLContextCore _bl_ctx;
static BLImageCore _bl_img;

#define MAX_TILE_CACHE_X 80
#define MAX_TILE_CACHE_Y 50
#define TILE_SIZE 96
static uint32_t _tile_cache1[MAX_TILE_CACHE_X * MAX_TILE_CACHE_Y];
static uint32_t _tile_cache2[MAX_TILE_CACHE_X * MAX_TILE_CACHE_Y];
static uint32_t *_tile_cache;
static uint32_t *_prev_tile_cache;

static void clip_to_framebuffer(recti *r);

void draw_rect(const render_cmd_rect *cmd, const recti *clip_rect)
{
    profiler_begin;

    BLRectI *bl_clip = (BLRectI*)clip_rect;
    blContextClipToRectI(&_bl_ctx, bl_clip);

    blContextSetCompOp(&_bl_ctx, BL_COMP_OP_SRC_OVER);
    BLRgba col = {
        .r = cmd->color.r,
        .g = cmd->color.g,
        .b = cmd->color.b,
        .a = cmd->color.a
    };
    blContextSetFillStyleRgba(&_bl_ctx, &col);

    BLRectI *bl_rect = (BLRectI*)&cmd->rect;
    blContextFillRectI(&_bl_ctx, bl_rect);

    blContextRestoreClipping(&_bl_ctx);

    profiler_end;
}

void draw_text(render_cmd_text *cmd, const recti *clip_rect)
{
#ifdef BG_MACOS
    text_draw(cmd->t, &cmd->bbox, clip_rect);
#elif BG_WINDOWS
#endif
}

bool render_init(void)
{
    _render_cmd_ctx.current = _render_cmd_ctx.cmds1;
    _render_cmd_ctx.previous = _render_cmd_ctx.cmds2;
    _render_cmd_ctx.curr_index = &_render_cmd_ctx.cmds_index1;
    _render_cmd_ctx.prev_index = &_render_cmd_ctx.cmds_index2;

    _tile_cache = _tile_cache1;
    _prev_tile_cache = _tile_cache2;

    // Initialize the tile caches.
    for (uint32_t y = 0; y < MAX_TILE_CACHE_Y; y++)
    {
        for (uint32_t x = 0; x < MAX_TILE_CACHE_X; x++)
        {
            uint32_t tile_index = x + y * MAX_TILE_CACHE_X;
            _tile_cache[tile_index] = HASH_INITIAL;
            _prev_tile_cache[tile_index] = HASH_INITIAL;
        }
    }

    return true;
}

void render_shutdown(void)
{
}

void render_begin_frame(void)
{
}

static void update_tile_cache(const recti *r, uint32_t hash_value)
{
    int32_t x1 = r->x / TILE_SIZE;
    int32_t y1 = r->y / TILE_SIZE;
    int32_t x2 = (r->x + r->w) / TILE_SIZE;
    int32_t y2 = (r->y + r->h) / TILE_SIZE;
    for (int32_t y = y1; y <= y2; y++) {
        for (int32_t x = x1; x <= x2; x++) {
            uint32_t *v = &_tile_cache[x + y * MAX_TILE_CACHE_X];
            hash(v, (uint8_t*)&hash_value, sizeof(hash_value));
        }
    }
}

void render_end_frame(void)
{
    profiler_begin;

    eva_framebuffer fb = eva_get_framebuffer();

    recti dirty_rect = {0};

    // Process current queue.
    int32_t num_cmds = *_render_cmd_ctx.curr_index;
    for (int32_t i = 0; i < num_cmds; i++)
    {
        uint32_t cmd_hash = HASH_INITIAL;
        render_cmd *cmd = &_render_cmd_ctx.current[i];
        recti *r;
        if (cmd->type == RENDER_COMMAND_RECT) {
            hash(&cmd_hash, (uint8_t*)cmd, sizeof(*cmd));
            r = &cmd->rect_cmd.rect;
        }
        else if (cmd->type == RENDER_COMMAND_TEXT) {
            hash(&cmd_hash, (uint8_t*)cmd, sizeof(*cmd));
            text_hash(cmd->text_cmd.t, &cmd_hash);
            r = &cmd->text_cmd.bbox;
        }
        update_tile_cache(r, cmd_hash);
    }

    // Merge all the dirty rects into 1 rect.
    // This can be dangerous if we have pixels
    // that are frequently changing on opposite
    // corners of the screen.
    // TODO: Consider only merging adjacent rects.
    uint32_t max_x = fb.w / TILE_SIZE + 1;
    uint32_t max_y = fb.h / TILE_SIZE + 1;
    bool first_difference = true;
    for (uint32_t y = 0; y < max_y; y++)
    {
        for (uint32_t x = 0; x < max_x; x++)
        {
            uint32_t tile_index = x + y * MAX_TILE_CACHE_X;
            uint32_t curr = _tile_cache[tile_index];
            uint32_t prev = _prev_tile_cache[tile_index];
            if (curr != prev)
            {
                recti region = {
                    .x = (int32_t)x * TILE_SIZE,
                    .y = (int32_t)y * TILE_SIZE,
                    .w = TILE_SIZE,
                    .h = TILE_SIZE,
                };

                if (first_difference)
                {
                    first_difference = false;
                    dirty_rect = region;
                }
                else
                {
                    recti_union(&dirty_rect, &region, &dirty_rect); 
                }
            }

            _prev_tile_cache[tile_index] = HASH_INITIAL;
        }
    }

    if (!recti_is_empty(&dirty_rect))
    {
        blImageInit(&_bl_img);
        blImageCreateFromData(&_bl_img, (int32_t)fb.w, (int32_t)fb.h, 
                BL_FORMAT_PRGB32, fb.pixels,
                fb.pitch * sizeof(eva_pixel),
                0, 0);

        BLContextCreateInfo create_info = {0};
        create_info.threadCount = 2;
        blContextInitAs(&_bl_ctx, &_bl_img, &create_info);

        for (int32_t i = 0; i < num_cmds; i++)
        {
            render_cmd *cmd = &_render_cmd_ctx.current[i];
            switch (cmd->type)
            {
                case RENDER_COMMAND_RECT:
                    draw_rect(&cmd->rect_cmd, &dirty_rect);
                    break;
                case RENDER_COMMAND_TEXT:
                    blContextFlush(&_bl_ctx, BL_CONTEXT_FLUSH_SYNC);
                    recti clip;
                    recti_intersection(&dirty_rect, &cmd->text_cmd.clip, &clip);
                    draw_text(&cmd->text_cmd, &clip);
                    break;
            }
        }

        blContextEnd(&_bl_ctx);
        blContextDestroy(&_bl_ctx);
        blImageDestroy(&_bl_img);
    }


    // Release the reference we took when adding the render text commands.
    for (int32_t i = 0; i < num_cmds; i++)
    {
        render_cmd *cmd = &_render_cmd_ctx.current[i];
        if (cmd->type == RENDER_COMMAND_TEXT) {
            text_destroy(cmd->text_cmd.t);
        }
    }

    // Swap tile caches.
    uint32_t *tmp_tile_cache = _tile_cache;
    _tile_cache = _prev_tile_cache;
    _prev_tile_cache = tmp_tile_cache;

    // Swap the current render queue.
    render_cmd *tmp_cmds = _render_cmd_ctx.current;
    _render_cmd_ctx.current = _render_cmd_ctx.previous;
    _render_cmd_ctx.previous = tmp_cmds;

    int32_t *tmp_index = _render_cmd_ctx.curr_index;
    _render_cmd_ctx.curr_index = _render_cmd_ctx.prev_index;
    _render_cmd_ctx.prev_index = tmp_index;

    // Always reset the current render queue regardless
    *_render_cmd_ctx.curr_index = 0;

    profiler_end;
}

void render_clear(const color *c)
{
    eva_framebuffer fb = eva_get_framebuffer();
    recti r = {
        .x = 0,
        .y = 0,
        .w = (int32_t)fb.w,
        .h = (int32_t)fb.h
    };

    int32_t index = (*_render_cmd_ctx.curr_index)++;
    render_cmd *cmd = &_render_cmd_ctx.current[index];
    cmd->type = RENDER_COMMAND_RECT;
    cmd->rect_cmd.rect = r;
    cmd->rect_cmd.color = *c;
}

void render_draw_rect(const recti *r, const color *c)
{
    int32_t index = (*_render_cmd_ctx.curr_index)++;
    render_cmd *cmd = &_render_cmd_ctx.current[index];
    cmd->type = RENDER_COMMAND_RECT;
    cmd->rect_cmd.rect = *r;
    cmd->rect_cmd.color = *c;
    clip_to_framebuffer(&cmd->rect_cmd.rect);
}

void render_draw_text(text *t, const recti *bbox, const recti *clip)
{
    int32_t index = (*_render_cmd_ctx.curr_index)++;
    render_cmd *cmd = &_render_cmd_ctx.current[index];
    cmd->type = RENDER_COMMAND_TEXT;
    cmd->text_cmd.clip = *clip;
    cmd->text_cmd.bbox = *bbox;
    cmd->text_cmd.t = text_ref(t);
}

static void clip_to_framebuffer(recti *r)
{
    assert(r);

    eva_framebuffer fb = eva_get_framebuffer();

    r->x = r->x > 0 ? r->x : 0;
    r->y = r->y > 0 ? r->y : 0;
    r->w = r->w < (int32_t)fb.w ? r->w : (int32_t)fb.w;
    r->h = r->h < (int32_t)fb.h ? r->h : (int32_t)fb.h;
}
