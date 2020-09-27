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
    double tbox_offset;
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
    tf->tbox_offset = 0.0;

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

    ustr *temp = ustr_create();
    ustr_append(temp, text, len);
    grapheme_iter *gi = grapheme_iter_create(temp);
    tf->cursor_index += grapheme_iter_last(gi);
    grapheme_iter_destroy(gi);
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
    else if (key == EVA_KEY_LEFT) {
        const ustr *str = text_ustr(tf->t);
        if (!ustr_empty(str) && tf->cursor_index > 0) {
            grapheme_iter *gi = grapheme_iter_create(str);
            tf->cursor_index = grapheme_iter_preceding(gi, tf->cursor_index);
            grapheme_iter_destroy(gi);
            eva_request_frame();
        }
    }
    else if (key == EVA_KEY_RIGHT) {
        const ustr *str = text_ustr(tf->t);
        if (!ustr_empty(str) && tf->cursor_index < ustr_len(str)) {
            grapheme_iter *gi = grapheme_iter_create(str);
            tf->cursor_index = grapheme_iter_following(gi, tf->cursor_index);
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

void textfield_draw(textfield *tf, const vec2 *pos)
{
    assert(tf);
    assert(pos);

    double width, leading, ascent, descent;
    text_metrics(tf->t, &width, &leading, &ascent, &descent);
    double height = ascent + descent;

    rect border = {
        .x = pos->x,
        .y = pos->y,
        .w = tf->width,
        .h = height + (tf->padding * 2)
    };

    render_draw_rect(&border, &COLOR_WHITE);

    rect textbox = {
        .x = border.x + tf->padding,
        .y = border.y + tf->padding,
        .w = border.w - (tf->padding * 2.0),
        .h = ascent
    };
    rect clip = textbox;
    clip.h = height;

    double text_pos = textbox.x + tf->tbox_offset;
    double cursor_width = 4.0;
    double cursor_half_width = cursor_width / 2.0;
    double cursor_start = text_pos +
                          text_index_offset(tf->t, tf->cursor_index) -
                          cursor_half_width;
    double text_width = max(width, tf->width);

    double offset = 0.0f;
    double cursor_end = cursor_start + cursor_width;
    double textbox_end = textbox.x + textbox.w;
    if (cursor_start <= textbox.x) {
        offset = textbox.x - cursor_start + (tf->width / 2.0);
        cursor_start += offset;
        cursor_end += offset;
    }
    if (cursor_end > textbox_end) {
        // produces a negative offset
        offset = textbox_end - cursor_end;
        cursor_start += offset;
        cursor_end += offset;
    }

    text_pos += offset;
    tf->tbox_offset += offset;
    if (tf->tbox_offset > 0.0) {
        double delta = tf->tbox_offset;
        cursor_start -= delta;
        text_pos -= delta;
        tf->tbox_offset = 0.0;
    }
    
    rect cursor = {
        .x = cursor_start,
        .y = textbox.y,
        .w = cursor_width,
        .h = height
    };

    textbox.x = text_pos;
    textbox.w = text_width;

    render_draw_rect(&cursor, &COLOR_BLUE);
    render_draw_text(tf->t, &textbox, &clip);
}

bool textfield_active(const textfield *tf);
bool textfield_hovered(const textfield *tf);
