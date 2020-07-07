#pragma once

#include "common.h"

typedef struct text {
    char *utf8data;
    size_t len;
} text;

int32_t count_character_boundaries(const text *t);
