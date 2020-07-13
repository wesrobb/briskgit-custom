#include "render.h"

#include <assert.h>
#include <math.h>
#include <pango/pango-font.h>
#include <pango/pango-language.h>
#include <pango/pango-layout.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>
#include <pango/pango.h>
#include <pango/pangoft2.h>
#include <glib.h>

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
    int32_t text_len;
    color color;
    int32_t pt_size;
    int32_t baseline_y;
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

static void clip_to_framebuffer(eva_rect *r);

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

static void draw_ft_bitmap(FT_Bitmap *bitmap,
                           double x_pos,
                           double y_pos,
                           color c,
                           eva_rect clip_rect)
{
    // TODO: Clean this function up. At least do the preamble.
    // TODO: This is on the hot path and it is pretty slow. Make it fast!
    profiler_begin;

    assert(bitmap);

    eva_framebuffer fb = eva_get_framebuffer();

    // Start drawing from the bitmap depending on how much of it is clipped.
    int32_t x_clipped = (int32_t)(clip_rect.x - x_pos);
    if (x_clipped <= 0) {
        x_clipped = 0;
    }
    int32_t y_clipped = (int32_t)(clip_rect.y - y_pos);
    if (y_clipped <= 0) {
        y_clipped = 0;
    }

    uint32_t start_x = (uint32_t)max(clip_rect.x, (int32_t)(x_pos + 0.5));
    uint32_t end_x   = (uint32_t)min(clip_rect.x + clip_rect.w,
                                     (int32_t)(x_pos + bitmap->width + 0.5));

    uint32_t start_y = (uint32_t)max(clip_rect.y, (int32_t)(y_pos + 0.5));
    uint32_t end_y   = (uint32_t)min(clip_rect.y + clip_rect.h,
                                     (int32_t)(y_pos + bitmap->rows + 0.5));

    end_x = min(end_x, fb.w);
    end_y = min(end_y, fb.h);

    // Move to the first pixel that resides in the rect
    eva_pixel *pxl = fb.pixels;
    pxl += start_x + start_y * fb.pitch;
    // Calculate how many pixels to jump over at
    // the end of each row in the rect.
    uint32_t next_row = fb.pitch - (end_x - start_x);

    for (uint32_t y = start_y, v = (uint32_t)y_clipped; y < end_y; y++, v++) {
        for (uint32_t x = start_x, u = (uint32_t)x_clipped; x < end_x; x++, u++) {
            float alpha = bitmap->buffer[v * bitmap->width + u] / 255.0f;
            if (alpha != 0.0f) {

                // Pre-multiply font alpha in linear space.
                color src = {
                    .r = c.r * alpha,
                    .g = c.g * alpha,
                    .b = c.b * alpha,
                    .a = alpha
                };

                // Convert to float
                color dst_srgb = {
                    .b = pxl->b / 255.0f,
                    .g = pxl->g / 255.0f,
                    .r = pxl->r / 255.0f,
                    .a = pxl->a / 255.0f,
                };

                // Exact gamma value would be 2.2 but we use 2
                // to avoid expensive pow function and use the square of x instead.
                color dst = {
                    .r = dst_srgb.r * dst_srgb.r,
                    .g = dst_srgb.g * dst_srgb.g,
                    .b = dst_srgb.b * dst_srgb.b,
                    .a = dst_srgb.a
                };

                // Do linear blend
                color blend = {
                    .r = src.r + dst.r * (1.0f - src.a),
                    .g = src.g + dst.g * (1.0f - src.a),
                    .b = src.b + dst.b * (1.0f - src.a),
                    .a = dst.a
                };

                color blend_srgb = {
                    .r = sqrtf(blend.r),
                    .g = sqrtf(blend.g),
                    .b = sqrtf(blend.b),
                    .a = blend.a
                };

                // Convert back to uint8_t
                eva_pixel final = {
                    .b = (uint8_t)(blend_srgb.b * 255.999f),
                    .g = (uint8_t)(blend_srgb.g * 255.999f),
                    .r = (uint8_t)(blend_srgb.r * 255.999f),
                    .a = (uint8_t)(blend_srgb.a * 255.999f),
                };

                *pxl = final;
            }
            pxl++;
        }

        pxl += next_row;
    }

    profiler_end;
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
    profiler_begin;

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
                (uint32_t)(96.0f * scale_x),  // horizontal dpi // TODO get the actual DPI from eva
                (uint32_t)(96.0f * scale_y)); // vertical dpi

        if (error) {
            puts("Failed to set face size");
            return;
        }
        hb_ft_font_changed(*hb_font);
        _font_cache.pt_sizes[font] = pt_size;
        _font_cache.scale_x[font] = scale_x;
        _font_cache.scale_y[font] = scale_y;
    }

    profiler_end;
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

bool rect_intersect(eva_rect a, eva_rect b)
{
    return !((a.x > b.x + b.w || b.x > a.x + a.w) ||
             (a.y > b.y + b.h || b.y > a.y + a.h));
}

void draw_font_pango(eva_rect rect,
                     render_cmd_font *cmd,
                     eva_rect clip_rect)
{
    (void)rect;

    profiler_begin;

    if (rect_intersect(clip_rect, rect)) {
        profiler_begin_name("draw_font_rect_intersect");
        eva_framebuffer fb = eva_get_framebuffer();

        PangoFT2FontMap *ft2_fontmap = (PangoFT2FontMap*)pango_ft2_font_map_new();
        pango_ft2_font_map_set_resolution(ft2_fontmap, 192, 192);
        PangoFontMap *fontmap = (PangoFontMap*)ft2_fontmap;

        PangoContext *context = pango_font_map_create_context(fontmap);
        pango_context_set_language(context, pango_language_from_string("en"));
        pango_context_set_base_dir(context, PANGO_DIRECTION_LTR);

        PangoFontDescription *font_desc = pango_font_description_new();
        pango_font_description_set_family (font_desc, "Noto");
        pango_font_description_set_weight (font_desc, PANGO_WEIGHT_NORMAL);
        pango_font_description_set_size(font_desc, 18 * PANGO_SCALE);

        PangoLayout *layout = pango_layout_new(context);
        pango_layout_set_text(layout, cmd->text, cmd->text_len);
        pango_layout_set_font_description(layout, font_desc);

        uint8_t *data = malloc(fb.w * fb.h);
        FT_Bitmap bitmap = {
            .width = fb.w,
            .rows = fb.h,
            .pitch = (int)fb.w,
            .buffer = data,
            .num_grays = 4,
            .pixel_mode = FT_PIXEL_MODE_GRAY
        };

        pango_ft2_render_layout(&bitmap, layout, rect.x, cmd->baseline_y);
        draw_ft_bitmap(&bitmap, 0, 0, cmd->color, clip_rect);

        free(data);

        g_object_unref(layout);
        pango_font_description_free(font_desc);
        g_object_unref(context);
        g_object_unref(ft2_fontmap);

        profiler_end;
    }

    profiler_end;
}

void draw_font(eva_rect rect,
              render_cmd_font *cmd,
              eva_rect clip_rect)
{
    (void)rect;

    profiler_begin;

    if (rect_intersect(clip_rect, rect)) {
        profiler_begin_name("draw_font_rect_intersect");
        eva_framebuffer fb = eva_get_framebuffer();

        hb_buffer_t *buf;
        buf = hb_buffer_create();
        // See: https://harfbuzz.github.io/working-with-harfbuzz-clusters.html
        hb_buffer_set_cluster_level(buf, 
                                HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);
        hb_buffer_add_utf8(buf, cmd->text, cmd->text_len, 0, -1);
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
        double cursor_y = cmd->baseline_y;

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

    profiler_end;
}

static bool render_cmd_queues_differ()
{
    profiler_begin;

    if (*_render_cmd_ctx.curr_index != *_render_cmd_ctx.prev_index) {
        profiler_end;
        return true;
    }

    int32_t num_cmds = *_render_cmd_ctx.curr_index;
    for (int32_t i = 0; i < num_cmds; i++) {
        render_cmd *current = &_render_cmd_ctx.current[i];
        render_cmd *previous = &_render_cmd_ctx.previous[i];
        if (!render_cmds_equal(current, previous)) {
            profiler_end;
            return true;
        }
    }

    profiler_end;
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

    if (render_cmd_queues_differ())
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
                        draw_font_pango(cmd->rect, &cmd->font_cmd, dirty_rect);
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
    clip_to_framebuffer(r);

    int32_t index = (*_render_cmd_ctx.curr_index)++;
    render_cmd *cmd = &_render_cmd_ctx.current[index];
    cmd->type = RENDER_COMMAND_RECT;
    cmd->rect = *r;
    cmd->rect_cmd.color = c;
}

static void add_render_font_cmd(font font,
                                const char *text, int32_t text_len,
                                int32_t x, int32_t y,
                                int32_t pt_size, color c)
{
    assert(text);
    if (text[0] == 0) {
        return;
    }

    int32_t width = render_get_text_width(font, text, text_len, pt_size);
    int32_t ascent, descent;
    render_get_font_height(font, pt_size, &ascent, &descent);
    int32_t height = ascent - descent;

    // The y coordinate is the baseline of the font but we need a rect
    // that covers the glyphs entirely.
    // Also, add 2 pixels of padding since the reported font metrics are not
    // always 100% correct. This results in a tiny bit of overdraw which may
    // slightly affect perf but actual font rendering is not clipped anywhere.
    int32_t padding = 0;
    eva_rect bounding_rect = {
        .x = x - padding,
        .y = y - ascent - padding,
        .w = width + padding,
        .h = height + padding
    };

    clip_to_framebuffer(&bounding_rect);

    int32_t index = (*_render_cmd_ctx.curr_index)++;
    render_cmd *cmd = &_render_cmd_ctx.current[index];
    cmd->type = RENDER_COMMAND_FONT;
    cmd->font_cmd.font = font;
    cmd->font_cmd.pt_size = pt_size;
    cmd->font_cmd.color = c;
    cmd->font_cmd.baseline_y = y;
    cmd->rect = bounding_rect;

    // Don't copy more than there is space for.
    int32_t len = text_len < MAX_TEXT_LEN ? text_len : MAX_TEXT_LEN;
    cmd->font_cmd.text_len = len;
    memcpy(cmd->font_cmd.text, text, len);
}

void render_clear(color color)
{
    eva_framebuffer fb = eva_get_framebuffer();
    eva_rect r = {
        .x = 0,
        .y = 0,
        .w = (int32_t)fb.w,
        .h = (int32_t)fb.h
    };
    add_render_rect_cmd(&r, color);
}

void render_draw_rect(eva_rect *rect, color color) // TODO: These should be passed by pointer
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

void render_draw_font(font font, const char *text, int32_t text_len,
                      int32_t posX, int32_t posY,
                      int32_t pt_size, color c)
{
    add_render_font_cmd(font, text, text_len, posX, posY, pt_size, c);
}

int32_t render_get_text_width(font font, const char *text, int32_t text_len, 
                              int32_t pt_size)
{
    // TODO: This is too and it is called before render caching. Optimize.
    profiler_begin;

    hb_buffer_t *buf;
    buf = hb_buffer_create();

    // See: https://harfbuzz.github.io/working-with-harfbuzz-clusters.html
    hb_buffer_set_cluster_level(buf, 
                                HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);
    hb_buffer_add_utf8(buf, text, text_len, 0, -1);
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

    return (int32_t)(cursor_x);
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

    *ascent = extents.ascender / 64;
    *descent = extents.descender / 64;

    profiler_end;
}

static void clip_to_framebuffer(eva_rect *r)
{
    assert(r);

    eva_framebuffer fb = eva_get_framebuffer();

    r->x = r->x > 0 ? r->x : 0;
    r->y = r->y > 0 ? r->y : 0;
    r->w = r->w < (int32_t)fb.w ? r->w : (int32_t)fb.w;
    r->h = r->h < (int32_t)fb.h ? r->h : (int32_t)fb.h;
}
