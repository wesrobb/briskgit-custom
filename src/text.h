#pragma once

#include "common.h"

typedef struct color color;
typedef struct text_attr text_attr;
typedef struct ustr ustr;
typedef struct vec2i vec2i;

typedef enum font_family_id {
    FONT_FAMILY_MENLO,
    FONT_FAMILY_MONACO,
    FONT_FAMILY_COURIER_NEW,
    FONT_FAMILY_MONOSPACE,
    FONT_FAMILY_DEFAULT = FONT_FAMILY_MENLO,
} font_family_id;

typedef struct text {
    ustr *str;
    text_attr *attrs; // Linked list of text attributes
} text;

void text_system_init();

// Sets text attributes. Set start_grapheme = 0 and end_grapheme = -1
// to see the attribute for the entire text length.
void text_add_attr(text *t,
                   int32_t start_grapheme, int32_t end_grapheme,
                   font_family_id font_family, float font_size,
                   const color *c);

void text_bounding_box(const text *t, vec2i *dst);
