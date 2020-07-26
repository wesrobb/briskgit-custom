#pragma once

#include "common.h"

typedef struct ustr ustr;

ustr* ustr_create_utf8(const char *utf8data, size_t len);
ustr* ustr_create_cstr(const char *cstr);

// Reduce the reference count and if the count == 0 then free the string.
void ustr_destroy(ustr *);

// Returns the raw data.
void* ustr_data(const ustr *);

// Returns the number of UTF16 chars that make up the string.
int32_t ustr_len(const ustr *);

// Returns the byte length of the string.
int32_t ustr_byte_len(const ustr *);

// Increments the reference count and returns the string.
ustr* ustr_ref(ustr *);

// Returns the number of codepoints in the string.
int32_t ustr_num_codepoints(const ustr *s);

// Returns the number of graphemes in the string.
int32_t ustr_num_graphemes(const ustr* s);
