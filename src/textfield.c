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
    bool hovered;
    bool active;
    double width;
    double font_size;
    int32_t padding;
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

void textfield_input_text(const textfield *tf, 
                          const uint16_t *text, size_t len)
{
    assert(tf);
    assert(text);

    text_append(tf->t, text, len);
}

void textfield_keydown(const textfield *tf, int32_t key, uint32_t mods)
{
    assert(tf);

    if (key == EVA_KEY_BACKSPACE) {
        const ustr *str = text_ustr(tf->t);
        if (!ustr_empty(str)) {
            grapheme_iter *gi = grapheme_iter_create(str);
            int32_t last = grapheme_iter_last(gi);
            int32_t previous = grapheme_iter_previous(gi);
            printf("removing %d %d\n", previous, last);
            text_remove(tf->t, (size_t)previous, (size_t)last);
            eva_request_frame();
        }
    }
}

void textfield_mouse_moved(const textfield *tf, const vec2 *pos);
void textfield_mouse_pressed(const textfield *tf, const vec2 *pos);
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
    render_draw_text(tf->t, &tbox, &bbox);
}

bool textfield_active(const textfield *tf);
bool textfield_hovered(const textfield *tf);
