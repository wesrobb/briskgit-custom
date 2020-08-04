#include "ustr.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <unicode/urename.h>
#include <unicode/utypes.h>
#include <unicode/ubrk.h>
#include <unicode/utext.h>
#include <unicode/ustring.h>

#include "console.h"
#include "hash.h"

typedef struct ustr {
    int32_t len; // num UChar's. NOT a byte length
    int32_t ref;
    UChar *data; // UTF-16 encoded
} ustr;

ustr* ustr_create_utf8(const char *utf8data, size_t len)
{
    assert(utf8data);

    ustr *result = malloc(sizeof(ustr));
    if (!result) {
        console_log("Failed to alloc ustr");
        return NULL;
    }

    UErrorCode error_code = 0;

    int32_t dst_len = 0;
    u_strFromUTF8(NULL, 0, &dst_len, utf8data, (int32_t)len, &error_code);
    // Buffer overflow is expected when looking for dst length
    if (error_code != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(error_code)) {
        // TODO: Failure handling
        free(result);
        return NULL;
    }
    // Reset the buffer overflow error.
    error_code = U_ZERO_ERROR;

    result->data = malloc((size_t)(dst_len) * sizeof(UChar));
    if (!result) {
        // TODO: Failure handling
        free(result);
        return NULL;
    }
    
    result->data = u_strFromUTF8(result->data, dst_len, &dst_len, 
                                 utf8data, len, &error_code);
    if (U_FAILURE(error_code)) {
        // TODO: Failure handling
        free(result->data);
        free(result);
        return NULL;
    }

    result->len = dst_len;
    result->ref = 1;

    return result;
}

ustr* ustr_create_cstr(const char * cstr)
{
    assert(cstr);

    size_t len = strlen(cstr);
    return ustr_create_utf8((void*)cstr, len);
}

void ustr_destroy(ustr *str)
{
    assert(str);
    assert(str->ref > 0);

    str->ref--;
    if (str->ref == 0) {
        free(str->data);
        free(str);
    }
}

void* ustr_data(const ustr *s)
{
    assert(s);

    return s->data;
}

int32_t ustr_len(const ustr *s)
{
    assert(s);

    return s->len;
}

int32_t ustr_byte_len(const ustr *s)
{
    assert(s);

    return s->len * (int32_t)sizeof(UChar);
}

ustr* ustr_ref(ustr *str)
{
    assert(str);
    assert(str->ref > 0);

    str->ref++;
    return str;
}

int32_t ustr_num_codepoints(const ustr *s)
{
    assert(s);
    assert(s->data);

    return u_countChar32(s->data, s->len);
}

int32_t ustr_num_graphemes(const ustr* s)
{
    assert(s);
    UText          *ut     = NULL;
    UBreakIterator *bi     = NULL;
    int32_t         count  = 0;
    UErrorCode      status = U_ZERO_ERROR;

    ut = utext_openUChars(ut, s->data, s->len, &status);

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

void ustr_hash(const ustr *s, uint32_t *v)
{
    hash(v, (uint8_t*)s->data, (size_t)ustr_byte_len(s));
}
