#include "text.h"

#include <assert.h>
#include <unicode/utypes.h>
#include <unicode/ubrk.h>
#include <unicode/utext.h>

#include "common.h"

typedef struct text_attr {
    int32_t start_grapheme;
    int32_t end_grapheme;
} text_attr;

int32_t count_character_boundaries(const text *t)
{
    UText          *ut         = NULL;
    UBreakIterator *bi         = NULL;
    int             char_count = 0;
    UErrorCode      status     = U_ZERO_ERROR;

    ut = utext_openUTF8(ut, t->utf8data, t->len, &status);
    bi = ubrk_open(UBRK_CHARACTER, "en_us", NULL, 0, &status);

    ubrk_setUText(bi, ut, &status);
    while (ubrk_next(bi) != UBRK_DONE) {
        char_count++;
    }
    utext_close(ut);
    ubrk_close(bi);
    assert(U_SUCCESS(status));
    return char_count;
}
