#pragma once

#include "common.h"

typedef struct text_attr text_attr;

typedef struct text {
    char *utf8data;
    size_t len;

    text_attr *attrs; // Linked list of text attributes
} text;

int32_t count_character_boundaries(const text *t);
