#pragma once

#include "common.h"

typedef struct ustr ustr;

ustr* ustr_create();
ustr* ustr_create_utf8(const char *utf8data, size_t len);
ustr* ustr_create_cstr(const char *cstr);

// Reduce the reference count and if the count == 0 then free the string.
void ustr_destroy(ustr *);

// Returns the raw data.
void* ustr_data(const ustr *);

// Returns the number of UTF16 chars that make up the string.
size_t ustr_len(const ustr *);

// Returns true of the string has no data.
bool ustr_empty(const ustr *);

// Returns the byte length of the string.
size_t ustr_byte_len(const ustr *);

// Increments the reference count and returns the string.
ustr* ustr_ref(ustr *);

// Returns the number of codepoints in the string.
int32_t ustr_num_codepoints(const ustr *);

// Returns the number of graphemes in the string.
int32_t ustr_num_graphemes(const ustr *);

// Returns the FNV-1a hash of the ustr using the input as the initial hash 
// value.
void ustr_hash(const ustr *s, uint32_t *hash);

// Append data to the string.
void ustr_append(ustr *s, const uint16_t *data, size_t len);

// Insert data at the specified index into the string.
void ustr_insert(ustr *s, size_t index, const uint16_t *data, size_t len);

// Remove the specified range from the string.
void ustr_remove(ustr *s, size_t start, size_t end);
