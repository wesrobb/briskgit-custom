#pragma once

#include "common.h"

typedef struct grapheme_iter grapheme_iter;
typedef struct ustr ustr;

grapheme_iter* grapheme_iter_create(const ustr *s);
void grapheme_iter_destroy(grapheme_iter *);

size_t grapheme_iter_first(grapheme_iter *);
size_t grapheme_iter_last(grapheme_iter *);
size_t grapheme_iter_next(grapheme_iter *);
size_t grapheme_iter_previous(grapheme_iter *);
size_t grapheme_iter_preceding(grapheme_iter *, size_t index);
size_t grapheme_iter_following(grapheme_iter *, size_t index);
