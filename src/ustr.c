#include "ustr.h"

#include <assert.h>
#include <unicode/urename.h>
#include <unicode/utypes.h>
#include <unicode/ubrk.h>
#include <unicode/utext.h>

int32_t ustr_num_graphemes(const ustr* s)
{
    assert(s);
    UText          *ut     = NULL;
    UBreakIterator *bi     = NULL;
    int32_t         count  = 0;
    UErrorCode      status = U_ZERO_ERROR;

    ut = utext_openUTF8(ut, s->data, s->len, &status);

    // TODO: Is it ok to always use the en_us locale?
    // Maybe this should be configurable?
    bi = ubrk_open(UBRK_CHARACTER, "en_us", NULL, 0, &status);

    ubrk_setUText(bi, ut, &status);
    while (ubrk_next(bi) != UBRK_DONE) {
        count++;
    }
    utext_close(ut);
    ubrk_close(bi);
    assert(U_SUCCESS(status));
    return count;
}
