#include "textfield.h"

#include <assert.h>
#include <stdlib.h>

#include "color.h"
#include "console.h"
#include "render.h"
#include "rect.h"
#include "text.h"
#include "vec2.h"

typedef struct textfield {
    text *t;
    bool hovered;
    bool active;
    vec2 pos;
    double width;
    double font_size;
    int32_t padding;
} textfield;

textfield* textfield_create(const vec2 *pos, double width,
                            double font_size, int32_t padding)
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

    tf->pos = *pos;
    tf->width = width;
    tf->font_size = font_size;
    tf->padding = padding;

    return tf;
}

void textfield_destroy(textfield *tf)
{
    assert(tf);

    free(tf);
}

void textfield_input_text(const textfield *tf, 
                          const uint16_t *text, size_t len)
{
    assert(tf);
    assert(text);

    text_append(tf->t, text, len);
}

void textfield_mouse_moved(const textfield *tf, const vec2 *pos);
void textfield_mouse_pressed(const textfield *tf, const vec2 *pos);
void textfield_mouse_released(const textfield *tf, const vec2 *pos);

void textfield_draw(const textfield *tf)
{
    assert(tf);

    vec2 extents;
    text_extents(tf->t, &extents);

    rect bbox = {
        .x = tf->pos.x,
        .y = tf->pos.y,
        .w = tf->width,
        .h = extents.y
    };

    render_draw_rect(&bbox, &COLOR_WHITE);

    rect tbox = {
        .x = tf->pos.x + tf->padding,
        .y = tf->pos.y + tf->padding,
        .w = extents.x,
        .h = extents.y - (tf->padding * 2) 
    };
    rect clip = tbox;
    clip.w = tf->width;
    render_draw_text(tf->t, &tbox, &clip);
}

bool textfield_active(const textfield *tf);
bool textfield_hovered(const textfield *tf);
