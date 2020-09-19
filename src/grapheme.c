#include "grapheme.h"

#include <assert.h>

#include <unicode/urename.h>
#include <unicode/utypes.h>
#include <unicode/ubrk.h>
#include <unicode/utext.h>
#include <unicode/ustring.h>

#include "ustr.h"

typedef struct grapheme_iter {
    UText          *ut;
    UBreakIterator *bi;

    bool in_use; // true if this iter is currently in use.
} grapheme_iter;

#define MAX_GRAPHEME_ITERS 32
static grapheme_iter _iters[MAX_GRAPHEME_ITERS];

grapheme_iter* grapheme_iter_create(const ustr *s)
{
    assert(s);

    for (size_t i = 0; i < MAX_GRAPHEME_ITERS; i++) {
        grapheme_iter *gi = _iters + i;
        if (!gi->in_use) {
            // TODO: Is it ok to always use the en_us locale?
            // Maybe this should be configurable?
            UErrorCode status = U_ZERO_ERROR;
            
            gi->ut = utext_openUChars(gi->ut, ustr_data(s), ustr_len(s), 
                                      &status);
            if (U_SUCCESS(status)) {
                gi->bi = ubrk_open(UBRK_CHARACTER, "en_us", NULL, 0, &status); 
                ubrk_setUText(gi->bi, gi->ut, &status);

                if (U_SUCCESS(status)) {
                    gi->in_use = true;
                    return gi;
                }
                else {
                    assert(false);
                    utext_close(gi->ut);
                    ubrk_close(gi->bi);
                }
            }
            else {
                assert(false);
            }
        }
    }

    return NULL;
}

void grapheme_iter_destroy(grapheme_iter *gi)
{
    assert(gi);
    utext_close(gi->ut);
    ubrk_close(gi->bi);
    gi->ut = NULL;
    gi->bi = NULL;
    gi->in_use = false;
}

int32_t grapheme_iter_first(grapheme_iter *gi)
{
    assert(gi);
    assert(gi->in_use);

    return ubrk_first(gi->bi);
}

int32_t grapheme_iter_last(grapheme_iter *gi)
{
    assert(gi);
    assert(gi->in_use);

    return ubrk_last(gi->bi);
}

int32_t grapheme_iter_next(grapheme_iter *gi)
{
    assert(gi);
    assert(gi->in_use);

    return ubrk_next(gi->bi);
}

int32_t grapheme_iter_previous(grapheme_iter *gi)
{
    assert(gi);
    assert(gi->in_use);

    return ubrk_previous(gi->bi);
}
