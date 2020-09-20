#include "textfield.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "eva/eva.h"

#include "color.h"
#include "console.h"
#include "grapheme.h"
#include "render.h"
#include "rect.h"
#include "text.h"
#include "ustr.h"
#include "vec2.h"

typedef struct textfield {
    text *t;
    double width;
    double font_size;
    int32_t padding;
    size_t cursor_index;
    double cursor_pos;
} textfield;

textfield* textfield_create(double width, double font_size, int32_t padding)
{
    textfield *tf = malloc(sizeof(*tf));
    if (!tf) {
        console_log("Failed to alloc textfield");
        return NULL;
    }

    tf->t = text_create();
    if (!tf->t) {
        console_log("Failed to alloc textfield string");
        free(tf);
        return NULL;
    }

    tf->width = width;
    tf->font_size = font_size;
    tf->padding = padding;
    tf->cursor_index = 0;

    text_add_attr(tf->t, 0, 0,
                  FONT_FAMILY_COURIER_NEW, font_size, &COLOR_BLACK);

    return tf;
}

void textfield_destroy(textfield *tf)
{
    assert(tf);

    text_destroy(tf->t);
    free(tf);
}

void textfield_input_text(textfield *tf, const uint16_t *text, size_t len)
{
    assert(tf);
    assert(text);

    text_insert(tf->t, tf->cursor_index, text, len);
    
    // TODO: This probably isn't correct for the multichar case.
    tf->cursor_index += len;
    eva_request_frame();
}

void textfield_keydown(textfield *tf, int32_t key, uint32_t mods)
{
    (void)mods;

    assert(tf);
    
    if (key == EVA_KEY_BACKSPACE) {
        const ustr *str = text_ustr(tf->t);
        if (!ustr_empty(str) && tf->cursor_index > 0) {
            grapheme_iter *gi = grapheme_iter_create(str);
            size_t preceding = grapheme_iter_preceding(gi, tf->cursor_index);
            text_remove(tf->t, preceding, tf->cursor_index);
            tf->cursor_index = preceding;
            grapheme_iter_destroy(gi);
            eva_request_frame();
        }
    }
    else if (key == EVA_KEY_DELETE) {
        const ustr *str = text_ustr(tf->t);
        if (!ustr_empty(str) && tf->cursor_index < ustr_len(str)) {
            grapheme_iter *gi = grapheme_iter_create(str);
            size_t following = grapheme_iter_following(gi, tf->cursor_index);
            text_remove(tf->t, tf->cursor_index, following);
            grapheme_iter_destroy(gi);
            eva_request_frame();
        }
    }
}

void textfield_mouse_moved(const textfield *tf, const vec2 *pos);
void textfield_mouse_pressed(textfield *tf,
                             const vec2 *mouse_pos, const vec2 *pos)
{
    assert(tf);
    assert(mouse_pos);
    assert(pos);

    double width, leading, ascent, descent;
    text_metrics(tf->t, &width, &leading, &ascent, &descent);
    double height = ascent + descent;

    rect bbox = {
        .x = pos->x,
        .y = pos->y,
        .w = tf->width,
        .h = height + (tf->padding * 2)
    };

    if (rect_point_intersect(&bbox, mouse_pos)) {
        vec2 rel_pos = vec2_sub(mouse_pos, pos);
        text_hit(tf->t, &rel_pos, &tf->cursor_index);
        eva_request_frame();
    }
}

void textfield_mouse_released(const textfield *tf, const vec2 *pos);

void textfield_draw(const textfield *tf, const vec2 *pos)
{
    assert(tf);
    assert(pos);

    double width, leading, ascent, descent;
    text_metrics(tf->t, &width, &leading, &ascent, &descent);
    double height = ascent + descent;

    rect bbox = {
        .x = pos->x,
        .y = pos->y,
        .w = tf->width,
        .h = height + (tf->padding * 2)
    };

    render_draw_rect(&bbox, &COLOR_WHITE);

    rect tbox = {
        .x = pos->x + tf->padding,
        .y = pos->y + tf->padding,
        .w = width,
        .h = ascent 
    };

    double cursor_pos = text_index_offset(tf->t, tf->cursor_index);
    rect cursor = {
        .x = tbox.x + cursor_pos,
        .y = tbox.y,
        .w = 2.0,
        .h = height
    };
    render_draw_rect(&cursor, &COLOR_BLACK);
    render_draw_text(tf->t, &tbox, &bbox);
}

bool textfield_active(const textfield *tf);
bool textfield_hovered(const textfield *tf);
