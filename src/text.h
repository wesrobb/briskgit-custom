#pragma once

#include "common.h"

typedef struct color color;
typedef struct rect rect;
typedef struct text text;
typedef struct text_attr text_attr;
typedef struct ustr ustr;
typedef struct vec2 vec2;

typedef enum font_family_id {
    FONT_FAMILY_MENLO,
    FONT_FAMILY_MONACO,
    FONT_FAMILY_COURIER_NEW,
    FONT_FAMILY_DEFAULT = FONT_FAMILY_MENLO,
} font_family_id;

void text_system_init();

text* text_create(void);
text* text_create_ustr(ustr *);
text* text_create_cstr(const char *);

// Decrements the ref count and if the ref count == 0 then frees the memory.
void text_destroy(text *);

text* text_ref(text *t);

// Returns the ustr string contained within the text object. This string
// should not be modified since it will break the caching system inside the
// text object.
const ustr* text_ustr(const text *t);

// Sets text attributes. Set start = 0 and len = 0
// to set the attribute for the entire text length.
void text_add_attr(text *t,
                   int32_t start, int32_t len,
                   font_family_id font_family, double font_size,
                   const color *c);

void text_extents(const text *t, vec2 *dst);
void text_metrics(const text *t, double *width, double *leading,
                  double *ascent, double *descent);
double text_index_offset(const text *t, size_t index);

void text_draw(const text *t, const rect *bbox, const rect *clip); 

// Sets the FNV-1a hash value of the text object using the input
// hash as the initial value and rights the result bacl to the 
// same address.
void text_hash(const text *t, uint32_t *hash);

// Sets the utf16 string index of the position relative to the text's origin.
// Returns false if there is no hit, true otherwise.
bool text_hit(const text *t, const vec2 *pos, size_t *index);

// Returns the visual position for the specified character index.
// The position is relative to the origin of the text.
double text_index_pos(const text *t, size_t index);

void text_append(text *t, const uint16_t *data, size_t len);
void text_remove(text *t, size_t start, size_t end);
