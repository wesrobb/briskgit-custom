#pragma once

#include "common.h"

typedef struct color color;
typedef struct recti recti;
typedef struct text text;
typedef struct text_attr text_attr;
typedef struct ustr ustr;
typedef struct vec2i vec2i;

typedef enum font_family_id {
    FONT_FAMILY_MENLO,
    FONT_FAMILY_MONACO,
    FONT_FAMILY_COURIER_NEW,
    FONT_FAMILY_DEFAULT = FONT_FAMILY_MENLO,
} font_family_id;

void text_system_init();

text* text_create(ustr *);
text* text_create_cstr(const char *);

// Decrements the ref count and if the ref count == 0 then frees the memory.
void text_destroy(text *);

text* text_ref(text *t);

// Sets text attributes. Set start = 0 and len = 0
// to set the attribute for the entire text length.
void text_add_attr(text *t,
                   int32_t start, int32_t len,
                   font_family_id font_family, float font_size,
                   const color *c);

void text_extents(const text *t, vec2i *dst);
float text_index_offset(const text *t, int32_t index);

void text_draw(const text *t, const recti *bbox, const recti *clip); 

// Sets the FNV-1a hash value of the text object using the input
// hash as the initial value and rights the result bacl to the 
// same address.
void text_hash(const text *t, uint32_t *hash);

// Sets the utf16 string index of the position relative to the text's origin.
// Returns false if there is no hit, true otherwise.
bool text_hit(const text *t, const vec2i *pos, int32_t *index);
