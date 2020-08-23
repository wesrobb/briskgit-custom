#pragma once

#include "common.h"

typedef struct vec2i {
    int32_t x, y; 
} vec2i;

typedef struct vec2f {
    float x, y;
} vec2f;

// a - b
vec2i vec2i_sub(const vec2i *a, const vec2i *b);
