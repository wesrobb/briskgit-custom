#include "text.h"

#include <assert.h>

#include <CoreText/CoreText.h>

#include "eva/eva.h"

#include "common.h"
#include "color.h"
#include "coretext.h"
#include "ustr.h"
#include "vec2.h"

#define MAX_TEXT_ATTRS 2048

typedef struct text_attr {
    int32_t start;
    int32_t end;

    font_family_id font_family;
    float font_size;
    color color;

    text_attr *next;
} text_attr;

typedef struct text_ctx {
    // Text attribute storage.
    text_attr attrs[MAX_TEXT_ATTRS];

    // Text attribute free list.
    text_attr *free_list;

    bool initialized;
} text_ctx;

static text_ctx _ctx;

text_attr* get_next_attr();
void free_attr(text_attr *);
const char * get_font_family(font_family_id f);

void text_system_init()
{
    // Setup the free list.
    for (int32_t i = 0; i < MAX_TEXT_ATTRS - 1; i++) {
        _ctx.attrs[i].next = &_ctx.attrs[i + 1];
    }
    _ctx.free_list = _ctx.attrs;
}

void text_add_attr(text *t,
                   int32_t start, int32_t end,
                   font_family_id font_family, float font_size,
                   const color *c)
{
    assert(t);
    assert(c);

    text_attr *a = get_next_attr();
    assert(a);

    a->start = start;
    a->end = end == -1 ? (int32_t)t->str->len : end;
    a->font_family = font_family;
    a->font_size = font_size;
    a->color = c ? *c : COLOR_WHITE;

    text_attr *last = t->attrs;
    while (last && last->next) {
        last = last->next;
    }
    if (!last) {
        t->attrs = a;
        last = t->attrs;
    }
    else {
        last->next = a;
    }

    if (t->attrs) {
        t->attrs->next = get_next_attr();
    }
}

void text_extents(const text *t, vec2i *dst)
{
    assert(t);
    assert(dst);

#ifdef BG_MACOS
    // TODO: Don't throw these results away. Cache them so they don't have to
    // be recalculated at draw time.
    
    eva_framebuffer fb = eva_get_framebuffer();

    CFStringRef str = CFStringCreateWithBytesNoCopy(
            NULL,                   // Null allocator for no allocs
            (uint8_t*)t->str->data, // Raw bytes
            (long)t->str->len,      // Num bytes
            kCFStringEncodingUTF8,  // UTF8
            false,                  // No Byte Order Marker
            kCFAllocatorNull);      // No dealloc

    // Create a mutable attributed string with a max length of len.
    long len = CFStringGetLength(str);
    CFMutableAttributedStringRef attr_str =
             CFAttributedStringCreateMutable(kCFAllocatorDefault, len);
     
    CFAttributedStringReplaceString(attr_str, CFRangeMake(0, 0), str);

    // Set default color for entire string before getting into text attrs.
    CGColorRef white = CGColorGetConstantColor(kCGColorWhite);
    CFAttributedStringSetAttribute(attr_str, CFRangeMake(0, len),
                                   kCTForegroundColorAttributeName, white);

    text_attr *attr = t->attrs;
    while (attr) {
        CFRange r = CFRangeMake(attr->start, attr->end - attr->start);

        // Font
        const char *font_family = get_font_family(attr->font_family);
        CFStringRef font_name = CFStringCreateWithBytesNoCopy(
                NULL,
                (const uint8_t*)font_family,
                (long)strlen(font_family),
                kCFStringEncodingUTF8,
                false,
                kCFAllocatorNull);
        CTFontRef font = CTFontCreateWithName(font_name,
                                         attr->font_size * fb.scale_x, 0);
        CFAttributedStringSetAttribute(attr_str,
                                       r,
                                       kCTFontAttributeName,
                                       font);
        CFRelease(font);

        // Color
        if (attr->color.r != 0.0f &&
            attr->color.g != 0.0f &&
            attr->color.b != 0.0f &&
            attr->color.a != 0.0f) {
            CGColorRef color = CGColorCreateGenericRGB(attr->color.r,
                                                       attr->color.g,
                                                       attr->color.b,
                                                       attr->color.a);
            CFAttributedStringSetAttribute(attr_str, r,
                                           kCTForegroundColorAttributeName,
                                           color);
        }

        attr = attr->next;
    }

    // Create the framesetter with the attributed string.
    CTFramesetterRef framesetter =
             CTFramesetterCreateWithAttributedString(attr_str);
    CFRelease(attr_str);

    // Find a frame that will fit the string.
    CFRange string_range = {0};
    CGSize frame_constraints = { CGFLOAT_MAX, CGFLOAT_MAX };
    CFRange fit_range = {0};
    CGSize frame_size =  CTFramesetterSuggestFrameSizeWithConstraints(
            framesetter,
            string_range,
            NULL,
            frame_constraints,
            &fit_range);

    dst->x = (int32_t)frame_size.width;
    dst->y = (int32_t)frame_size.height;

    CFRelease(framesetter);
#elif BG_WINDOWS
    assert(false);
#endif
}

text_attr* get_next_attr()
{
    text_attr *result = _ctx.free_list;
    assert(result);

    if (_ctx.free_list) {
        _ctx.free_list = _ctx.free_list->next;
    }

    // No longer point into the free list.
    result->next = NULL;
    return result;
}

void free_attr(text_attr *ta)
{
    assert(ta);

    ta->next = _ctx.free_list;
    ta->color = COLOR_ZERO;
    ta->start = 0;
    ta->end = 0;

    _ctx.free_list = ta;
}

const char * get_font_family(font_family_id f)
{
    switch (f) {
        case FONT_FAMILY_MENLO:
            return "Menlo";
        case FONT_FAMILY_MONACO:
            return "Monaco";
        case FONT_FAMILY_COURIER_NEW:
            return "Courier New";
        case FONT_FAMILY_MONOSPACE:
            return "monospace";
    };
}
