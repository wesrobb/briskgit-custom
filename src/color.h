#pragma once

typedef struct color {
    float b, g, r, a;
} color;

#define COLOR_ZERO ((color){0.0f, 0.0f, 0.0f, 0.0f})
#define COLOR_WHITE ((color){1.0f, 1.0f, 1.0f, 1.0f})
