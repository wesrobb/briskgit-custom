#pragma once

#include "common.h"

typedef struct ustr {
    char *data; // UTF-8 encoded
    size_t len; // in bytes
} ustr;

// Returns the number of graphemes in the string.
int32_t ustr_num_graphemes(const ustr* s);
