#pragma once

#include "common.h"

typedef struct vec2i {
    int32_t x, y; 
} vec2i;

typedef struct vec2 {
    double x, y;
} vec2;

// a - b
vec2i vec2i_sub(const vec2i *a, const vec2i *b);
vec2 vec2_sub(const vec2 *a, const vec2 *b);
