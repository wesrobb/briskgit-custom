#include "render.h"

#include <assert.h>
#include <math.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>

#include "eva/eva.h"

#include "common.h"
#include "profiler.h"

typedef enum render_cmd_type {
    RENDER_COMMAND_RECT,
    RENDER_COMMAND_FONT
} render_cmd_type;

typedef struct render_cmd_rect {
    color color;
} render_cmd_rect;

#define MAX_TEXT_LEN 1000
typedef struct render_cmd_font {
    font font;
    char text[MAX_TEXT_LEN];
    color color;
    int32_t pt_size;
} render_cmd_font;

typedef struct render_cmd {
    render_cmd_type type;
    eva_rect rect;
    union {
        render_cmd_rect rect_cmd;
        render_cmd_font font_cmd;
    };
} render_cmd;

#define HARFBUZZ_NUM_FEATURES 3
typedef struct font_cache {
    FT_Library font_library;

    FT_Face face_cache[FONT_COUNT];
    hb_font_t *hb_font_cache[FONT_COUNT];
    int32_t pt_sizes[FONT_COUNT];

    // TODO: Recache these on DPI change.
    float scale_x[FONT_COUNT];
    float scale_y[FONT_COUNT];

    hb_feature_t hb_features[HARFBUZZ_NUM_FEATURES];
} font_cache;

static font_cache _font_cache;

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

#define MAX_TILE_CACHE_X 80
#define MAX_TILE_CACHE_Y 50
#define TILE_SIZE 96
static uint32_t _tile_cache1[MAX_TILE_CACHE_X * MAX_TILE_CACHE_Y];
static uint32_t _tile_cache2[MAX_TILE_CACHE_X * MAX_TILE_CACHE_Y];
static uint32_t *_tile_cache;
static uint32_t *_prev_tile_cache;

#define HASH_INITIAL 2166136261

// See https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash
static void hash(uint32_t *v, uint8_t *buf, size_t len)
{
    assert(buf);

    for (size_t i = 0; i < len; i++)
    {
        *v ^= buf[i];
        *v *= 16777619;
    }
}

static bool text_equals(const char *a, const char *b)
{
    uint64_t len_a = strlen(a);
    uint64_t len_b = strlen(b);
    if (len_a == len_b) {
        for (uint64_t i = 0; i < len_a; i++) {
            if (a[i] != b[i]) {
                return false;
            }
        }
    }

    return true;
}

static bool render_cmds_equal(render_cmd *a, render_cmd *b)
{
    // TODO: Compare performance with memcmp.
    // It is more likely the rect changes than the type changes
    // since rendering order is unlikely to vary much.
    // So check the rect first.
    if (a->rect.x == b->rect.x && a->rect.y == b->rect.y && a->rect.w == b->rect.w &&
        a->rect.h == b->rect.h) {
        if (a->type == b->type) {
            switch (a->type) {
            case RENDER_COMMAND_RECT:
                return a->rect_cmd.color.a == b->rect_cmd.color.a &&
                       a->rect_cmd.color.b == b->rect_cmd.color.b &&
                       a->rect_cmd.color.g == b->rect_cmd.color.g &&
                       a->rect_cmd.color.r == b->rect_cmd.color.r;
            case RENDER_COMMAND_FONT:
                return a->font_cmd.color.a == b->font_cmd.color.a &&
                       a->font_cmd.color.b == b->font_cmd.color.b &&
                       a->font_cmd.color.g == b->font_cmd.color.g &&
                       a->font_cmd.color.r == b->font_cmd.color.r &&
                       a->font_cmd.pt_size == b->font_cmd.pt_size &&
                       text_equals(a->font_cmd.text, b->font_cmd.text);
            }
        }
    }

    return false;
}

static eva_pixel color_to_pixel(color c)
{
    eva_pixel result = {
        .b = (uint8_t)(c.b * 255.999f),
        .g = (uint8_t)(c.g * 255.999f),
        .r = (uint8_t)(c.r * 255.999f),
        .a = (uint8_t)(c.a * 255.999f),
    };

    return result;
}

static color pixel_to_color(eva_pixel *p)
{
    color result = {
        .b = p->b / 255.0f,
        .g = p->g / 255.0f,
        .r = p->r / 255.0f,
        .a = p->a / 255.0f,
    };

    return result;
}

static color srgb_to_linear(color x)
{
    // Exact gamma value would be 2.2 but we use 2
    // to avoid expensive pow function and use the square of x instead.
    color result = {
        .r = x.r * x.r,
        .g = x.g * x.g,
        .b = x.b * x.b,
        .a = x.a
    };
    return result;
}

static color linear_to_srgb(color x)
{
    color result = {
        .r = sqrtf(x.r),
        .g = sqrtf(x.g),
        .b = sqrtf(x.b),
        .a = x.a };
    return result;
}

// Blends src onto dest assuming linear color space. Src must have
// pre-multiplied alpha.
static color linear_blend(color src, color dest)
{
    color result = {
        .r = src.r + dest.r * (1.0f - src.a),
        .g = src.g + dest.g * (1.0f - src.a),
        .b = src.b + dest.b * (1.0f - src.a),
        .a = dest.a
    };

    return result;
}

static void draw_ft_bitmap(FT_Bitmap *bitmap,
                           double x_pos,
                           double y_pos,
                           color c,
                           eva_rect clip_rect)
{
    profiler_begin;

    assert(bitmap);

    eva_framebuffer fb = eva_get_framebuffer();

    uint32_t start_x = (uint32_t)max(clip_rect.x, (int32_t)(x_pos + 0.5));
    uint32_t end_x   = (uint32_t)min(clip_rect.x + clip_rect.w,
                                     (int32_t)(x_pos + bitmap->width + 0.5));

    uint32_t start_y = (uint32_t)max(clip_rect.y, (int32_t)(y_pos + 0.5));
    uint32_t end_y   = (uint32_t)min(clip_rect.y + clip_rect.h,
                                     (int32_t)(y_pos + bitmap->rows + 0.5));

    end_x = min(end_x, fb.w);
    end_y = min(end_y, fb.h);

    // Move to the first pixel that resides in the rect
    eva_pixel *framebuffer = fb.pixels;
    framebuffer += start_x + start_y * fb.pitch;
    // Calculate how many pixels to jump over at
    // the end of each row in the rect.
    uint32_t next_row = fb.pitch - (end_x - start_x);

    for (uint32_t y = start_y, v = 0; y < end_y; y++, v++) {
        for (uint32_t x = start_x, u = 0; x < end_x; x++, u++) {
            float font_alpha_linear = 
                bitmap->buffer[v * bitmap->width + u] / 255.0f;
            // Pre-multiply font alpha in linear space.
            color font_linear_premultiplied_alpha = {
                .r = c.r * font_alpha_linear,
                .g = c.g * font_alpha_linear,
                .b = c.b * font_alpha_linear,
                .a = font_alpha_linear
            };

            color dest_color_srgb = pixel_to_color(framebuffer);
            color dest_color_linear = srgb_to_linear(dest_color_srgb);
            color blended_linear = linear_blend(
                    font_linear_premultiplied_alpha,
                    dest_color_linear
            );
            color blended_srgb = linear_to_srgb(blended_linear);

            *framebuffer = color_to_pixel(blended_srgb);
            framebuffer++;
        }

        framebuffer += next_row;
    }

    profiler_end;
}

static void scale_rect(eva_rect *r, float scale_x, float scale_y)
{
    r->x = (int32_t)(r->x * scale_x);
    r->w = (int32_t)(r->w * scale_x);
    r->y = (int32_t)(r->y * scale_y);
    r->h = (int32_t)(r->h * scale_y);
}

static const char *get_font_file(font font)
{
    // TODO: Bake these files into the exe.
    switch (font) {
    case FONT_ROBOTO_REGULAR:
        return "data/Roboto-Regular.ttf";
    case FONT_MENLO_REGULAR:
        return "data/MenloPowerline.ttf";
    default:
        return NULL;
    }
}

bool float_eq(float a, float b, float epsilon)
{
    return fabsf(a - b) < epsilon;
}

static void load_cached_font(font font,
                             FT_Face *face,
                             hb_font_t **hb_font,
                             int32_t pt_size, float scale_x, float scale_y)
{
    // Load cached font data and only set size if it wasn't set already.
    *face = _font_cache.face_cache[font];
    *hb_font = _font_cache.hb_font_cache[font];
    if (pt_size != _font_cache.pt_sizes[font] ||
        !float_eq(scale_x, _font_cache.scale_x[font], 0.1f) ||
        !float_eq(scale_y, _font_cache.scale_y[font], 0.1f)) {
        int32_t error = FT_Set_Char_Size(
                *face,       // handle to face object
                0,           // char_width in 1/64th of points
                pt_size * 64, // char_height in 1/64th of points
                (uint32_t)(72.0f * scale_x),  // horizontal dpi
                (uint32_t)(72.0f * scale_y)); // vertical dpi

        if (error) {
            puts("Failed to set face size");
            return;
        }
        hb_ft_font_changed(*hb_font);
        _font_cache.pt_sizes[font] = pt_size;
        _font_cache.scale_x[font] = scale_x;
        _font_cache.scale_y[font] = scale_y;
    }
}

void draw_rect(eva_rect rect,
              render_cmd_rect *cmd,
              eva_rect clip_rect)
{
    profiler_begin;

    eva_framebuffer fb = eva_get_framebuffer();

    uint32_t start_x = (uint32_t)max(clip_rect.x, rect.x);
    uint32_t end_x   = (uint32_t)min(clip_rect.x + clip_rect.w, rect.x + rect.w);

    uint32_t start_y = (uint32_t)max(clip_rect.y, rect.y);
    uint32_t end_y   = (uint32_t)min(clip_rect.y + clip_rect.h, rect.y + rect.h);

    end_x = min(end_x, fb.w);
    end_y = min(end_y, fb.h);

    // Move to the first pixel that resides in the rect
    eva_pixel *framebuffer = fb.pixels;
    framebuffer += start_x + start_y * fb.pitch;

    /// Calculate how many pixels to jump over
    // at the end of each row in the rect.
    uint32_t next_row = fb.pitch - (end_x - start_x);

    eva_pixel colored_pixel = color_to_pixel(cmd->color);
    for (uint32_t y = start_y; y < end_y; y++) {
        for (uint32_t x = start_x; x < end_x; x++) {
            *framebuffer = colored_pixel;
            framebuffer++;
        }

        framebuffer += next_row;
    }

    profiler_end;
}

void draw_font(eva_rect rect,
              render_cmd_font *cmd,
              eva_rect clip_rect)
{
    (void)rect;

    profiler_begin;

    eva_framebuffer fb = eva_get_framebuffer();

    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, cmd->text, -1, 0, -1);
    hb_buffer_guess_segment_properties(buf);

    FT_Face face;
    hb_font_t *hb_font;
    load_cached_font(cmd->font, &face, &hb_font,
                     cmd->pt_size, fb.scale_x, fb.scale_y);

    hb_shape(hb_font, buf, _font_cache.hb_features, HARFBUZZ_NUM_FEATURES);
    uint32_t glyphCount = 0;
    hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buf, &glyphCount);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyphCount);

    double cursor_x = rect.x;
    double cursor_y = rect.y;

    for (uint32_t i = 0; i < glyphCount; ++i) {
        hb_codepoint_t glyphid = glyph_info[i].codepoint;
        double x_offset  = glyph_pos[i].x_offset  / 64.0;
        double y_offset  = glyph_pos[i].y_offset  / 64.0;
        double x_advance = glyph_pos[i].x_advance / 64.0;
        double y_advance = glyph_pos[i].y_advance / 64.0;
        // draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset);
        // load glyph image into the slot (erase previous one)
        int32_t error = FT_Load_Glyph(face, glyphid, FT_LOAD_NO_HINTING);
        if (error) {
            printf("Error loading font glyph %d\n - continuing", error);
            continue; // ignore errors
        }
        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (error) {
            printf("Error rendering font glyph %d\n - continuing", error);
            continue; // ignore errors
        }

        // now, draw to our target surface
        double x = cursor_x + x_offset + face->glyph->bitmap_left;
        double y = cursor_y + y_offset - face->glyph->bitmap_top;
        draw_ft_bitmap(&face->glyph->bitmap, x, y, cmd->color, clip_rect);
        cursor_x += x_advance;
        cursor_y += y_advance;
    }

    hb_buffer_destroy(buf);

    profiler_end;
}

static bool render_cmd_queues_differ()
{
    if (*_render_cmd_ctx.curr_index != *_render_cmd_ctx.prev_index) {
        return true;
    }

    int32_t num_cmds = *_render_cmd_ctx.curr_index;
    for (int32_t i = 0; i < num_cmds; i++) {
        render_cmd *current = &_render_cmd_ctx.current[i];
        render_cmd *previous = &_render_cmd_ctx.previous[i];
        if (!render_cmds_equal(current, previous)) {
            return true;
        }
    }

    return false;
}

bool render_init(void)
{
    _font_cache.hb_features[0].tag = HB_TAG('k', 'e', 'r', 'n');
    _font_cache.hb_features[0].value = 1;
    _font_cache.hb_features[0].start = 0;
    _font_cache.hb_features[0].end = (uint32_t)-1;
    _font_cache.hb_features[1].tag = HB_TAG('l', 'i', 'g', 'a');
    _font_cache.hb_features[1].value = 1;
    _font_cache.hb_features[1].start = 0;
    _font_cache.hb_features[1].end = (uint32_t)-1;
    _font_cache.hb_features[2].tag = HB_TAG('c', 'l', 'i', 'g');
    _font_cache.hb_features[2].value = 1;
    _font_cache.hb_features[2].start = 0;
    _font_cache.hb_features[2].end = (uint32_t)-1;

    int32_t error = FT_Init_FreeType(&_font_cache.font_library);
    if (error) {
        printf("Failed to init FreeType\n");
        return false;
    }

    for (int32_t i = 0; i < FONT_COUNT; i++) {
        const char *fontFile = get_font_file((font)i);
        FT_Face face;
        error = FT_New_Face(_font_cache.font_library, fontFile, 0, &face);
        if (error == FT_Err_Unknown_File_Format) {
            // TODO: Proper cleanup of faces and ft library
            puts("Unknown font format");
            return false;
        } else if (error) {
            printf("Unknown error %d\n", error);
            return false;
        }

        hb_font_t *hb_font = hb_ft_font_create_referenced(face);
        if (hb_font == NULL) {
            printf("Failed to create hb_font\n");
            return false;
        }
        hb_ft_font_set_load_flags(hb_font, FT_LOAD_NO_HINTING);

        _font_cache.face_cache[i] = face;
        _font_cache.hb_font_cache[i] = hb_font;
    }

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
    for (int32_t i = 0; i < FONT_COUNT; i++) {
        hb_font_destroy(_font_cache.hb_font_cache[i]);
        FT_Done_Face(_font_cache.face_cache[i]);
    }
    FT_Done_Library(_font_cache.font_library);
}

void render_begin_frame(void)
{
}

static void update_tile_cache(eva_rect *r, uint32_t hash_value)
{
    int32_t x1 = r->x / TILE_SIZE;
    int32_t y1 = r->y / TILE_SIZE;
    int32_t x2 = (r->x + r->w) / TILE_SIZE;
    int32_t y2 = (r->y + r->h) / TILE_SIZE;
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            hash(&_tile_cache[x + y * MAX_TILE_CACHE_X],
                 (uint8_t*)&hash_value, sizeof(hash_value));
        }
    }
}

void render_end_frame(void)
{
    profiler_begin;

    eva_framebuffer fb = eva_get_framebuffer();

    if (true || render_cmd_queues_differ())
    {
        eva_rect dirty_rect = {0};

        // Process current queue.
        int32_t num_cmds = *_render_cmd_ctx.curr_index;
        for (int32_t i = 0; i < num_cmds; i++)
        {
            render_cmd *cmd = &_render_cmd_ctx.current[i];
            uint32_t cmd_hash = HASH_INITIAL;
            hash(&cmd_hash, (uint8_t*)cmd, sizeof(*cmd));
            update_tile_cache(&cmd->rect, cmd_hash);
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
                    eva_rect region = {
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
                        dirty_rect = eva_rect_union(&dirty_rect, &region);
                    }
                }

                _prev_tile_cache[tile_index] = HASH_INITIAL;
            }
        }


        if (!eva_rect_empty(&dirty_rect))
        {
            for (int32_t i = 0; i < num_cmds; i++)
            {
                render_cmd *cmd = &_render_cmd_ctx.current[i];
                switch (cmd->type)
                {
                    case RENDER_COMMAND_RECT:
                        draw_rect(cmd->rect, &cmd->rect_cmd, dirty_rect);
                        break;
                    case RENDER_COMMAND_FONT:
                        draw_font(cmd->rect, &cmd->font_cmd, dirty_rect);
                        break;
                }
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
    }

    // Always reset the current render queue regardless
    *_render_cmd_ctx.curr_index = 0;

    profiler_end;
}

static void add_render_rect_cmd(eva_rect *r, color c)
{
    int32_t index = (*_render_cmd_ctx.curr_index)++;
    render_cmd *cmd = &_render_cmd_ctx.current[index];
    cmd->type = RENDER_COMMAND_RECT;
    cmd->rect = *r;
    cmd->rect_cmd.color = c;

    eva_framebuffer fb = eva_get_framebuffer();
    scale_rect(&cmd->rect, fb.scale_x, fb.scale_y);
}

static void add_render_font_cmd(font font,
                                const char *text,
                                int32_t x, int32_t y,
                                int32_t pt_size, color c)
{
    int32_t index = (*_render_cmd_ctx.curr_index)++;
    render_cmd *cmd = &_render_cmd_ctx.current[index];
    cmd->type = RENDER_COMMAND_FONT;
    cmd->font_cmd.font = font;
    cmd->font_cmd.pt_size = pt_size;
    cmd->font_cmd.color = c;

    int32_t width = render_get_text_width(font, text, pt_size);
    int32_t ascent, descent;
    render_get_font_height(font, pt_size, &ascent, &descent);
    int32_t height = ascent - descent;

    // Only scale x and y since width and height are already scaled.
    cmd->rect.x = x;
    cmd->rect.y = y;
    cmd->rect.w = width;
    cmd->rect.h = height;

    eva_framebuffer fb = eva_get_framebuffer();
    scale_rect(&cmd->rect, fb.scale_x, fb.scale_y);

    size_t len = strlen(text);
    memset(cmd->font_cmd.text, 0, MAX_TEXT_LEN);
    if (len < MAX_TEXT_LEN) {
        memcpy(cmd->font_cmd.text, text, len);
    }
}

void render_clear(color color)
{
    eva_rect r = {
        .x = 0,
        .y = 0,
        .w = (int32_t)eva_get_window_width(),
        .h = (int32_t)eva_get_window_height()
    };
    add_render_rect_cmd(&r, color);
}

void render_draw_rect(eva_rect *rect,
                     color color) // TODO: These should be passed by pointer
{
    add_render_rect_cmd(rect, color);
}

// void Render_DrawHollowRect(
//    eva_rect    rect,
//    color   color,
//    int32_t borderThickness) // TODO: These should be passed by pointer
//{
//    profiler_begin;
//
//    assert(_render_ctx.initialized);
//
//    scale_rect(&rect);
//    DrawHollowRectUnscaled(rect, color, borderThickness);
//
//    profiler_end;
//}

void render_draw_font(
    font font, const char *text, int32_t posX, int32_t posY, int32_t pt_size, color c)
{
    add_render_font_cmd(font, text, posX, posY, pt_size, c);
}

int32_t render_get_text_width(font font, const char *text, int32_t pt_size)
{
    profiler_begin;

    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);
    hb_buffer_guess_segment_properties(buf);

    FT_Face face;
    hb_font_t *hb_font;
    eva_framebuffer fb = eva_get_framebuffer();
    load_cached_font(font, &face, &hb_font, pt_size, fb.scale_x, fb.scale_y);

    hb_shape(hb_font, buf, _font_cache.hb_features, HARFBUZZ_NUM_FEATURES);
    uint32_t glyphCount = 0;
    hb_glyph_position_t *glyph_pos = 
        hb_buffer_get_glyph_positions(buf, &glyphCount);

    double cursor_x = 0;
    for (uint32_t i = 0; i < glyphCount; ++i) {
        double x_advance = glyph_pos[i].x_advance / 64.0;

        cursor_x += x_advance;
    }

    hb_buffer_destroy(buf);

    profiler_end;

    return (int32_t)(cursor_x / fb.scale_x);
}

void render_get_font_height(font font, int32_t pt_size,
                            int32_t *ascent, int32_t *descent)
{
    profiler_begin;

    FT_Face face;
    hb_font_t *hb_font;
    eva_framebuffer fb = eva_get_framebuffer();
    load_cached_font(font, &face, &hb_font, pt_size, fb.scale_x, fb.scale_y);

    hb_font_extents_t extents;
    hb_font_get_h_extents(hb_font, &extents);

    *ascent = extents.ascender / 64 / (int32_t)fb.scale_y;
    *descent = extents.descender / 64 / (int32_t)fb.scale_y;

    profiler_end;
}
