#include "text.h"

#include <assert.h>
#include <stdio.h>

#include <CoreFoundation/CFNumber.h>
#include <CoreText/CoreText.h>

#include "eva/eva.h"

#include "common.h"
#include "color.h"
#include "coretext.h"
#include "rect.h"
#include "ustr.h"
#include "vec2.h"

#define MAX_TEXT_ATTRS 2048

typedef struct text_attr {
    int32_t start;
    int32_t len;

    font_family_id font_family;
    float font_size;
    color color;

    text_attr *next;
} text_attr;

typedef struct text {
    ustr *str;
    text_attr *attrs; // Linked list of text attributes
    int32_t ref;
} text;

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
void text_extents_macos(const text *t, vec2i *dst);
void text_draw_macos(const text *t, const recti *bbox, const recti *clip);
CFMutableAttributedStringRef create_attr_str(const text *t);

void text_system_init()
{
    // Setup the free list.
    for (int32_t i = 0; i < MAX_TEXT_ATTRS - 1; i++) {
        _ctx.attrs[i].next = &_ctx.attrs[i + 1];
    }
    _ctx.free_list = _ctx.attrs;
}

text* text_create(ustr *str)
{
    assert(str);

    text *t = malloc(sizeof(text));
    if (!t) {
        // TODO: Handle failure.
        return NULL;
    }

    t->str = ustr_ref(str);
    t->attrs = NULL;
    t->ref = 1;

    return t;
}

text* text_create_cstr(const char *cstr)
{
    assert(cstr);

    text *t = malloc(sizeof(*t));
    if (!t) {
        // TODO: Handle failure.
        return NULL;
    }

    t->str = ustr_create_cstr(cstr);
    if (!t->str) {
        free(t);
        // TODO: Handle failure.
        return NULL;
    }

    t->attrs = NULL;
    t->ref = 1;

    return t;
}

void text_destroy(text *t)
{
    assert(t);

    ustr_destroy(t->str);
    t->ref--;
    if (t->ref == 0) {
        text_attr *attr = t->attrs;
        while (attr) {
            text_attr *tmp = attr;
            attr = attr->next;
            free_attr(tmp);
        }
        free(t);
    }
}

text* text_ref(text *t)
{
    assert(t);

    ustr_ref(t->str);
    t->ref++;
    return t;
}

void text_add_attr(text *t,
                   int32_t start, int32_t len,
                   font_family_id font_family, float font_size,
                   const color *c)
{
    assert(t);
    assert(c);

    text_attr *a = get_next_attr();
    assert(a);

    a->start = start;
    a->len = len == 0 ? ustr_len(t->str) : len;
    a->font_family = font_family;
    a->font_size = font_size;
    a->color = c ? *c : COLOR_WHITE;

    if (t->attrs) {
        text_attr *attr = t->attrs;
        while (attr->next)
        {
            attr = attr->next;
        }
        attr->next = a;
    } 
    else {
        t->attrs = a;
    }
}

void text_extents(const text *t, vec2i *dst)
{
    assert(t);
    assert(dst);

#ifdef BG_MACOS
    text_extents_macos(t, dst);
#elif BG_WINDOWS
    assert(false);
#endif
}

void text_draw(const text *t, const recti *bbox, const recti *clip)
{
    assert(t);
    assert(bbox);

#ifdef BG_MACOS
    text_draw_macos(t, bbox, clip);
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
    ta->len = 0;

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
    };
}

void text_extents_macos(const text *t, vec2i *dst)
{
    // TODO: Don't throw these results away. Cache them so they don't have to
    // be recalculated at draw time.
    CFMutableAttributedStringRef attr_str = create_attr_str(t);

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

    dst->x = (int32_t)round(frame_size.width);
    dst->y = (int32_t)round(frame_size.height);

    CFRelease(framesetter);
}

void text_draw_macos(const text *t, const recti *bbox, const recti *clip_rect)
{
    eva_framebuffer fb = eva_get_framebuffer();
    uint32_t bitmap_info = kCGImageAlphaPremultipliedFirst |
                           kCGBitmapByteOrder32Little;

    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    size_t bytes_per_row = fb.pitch * 4;
    CGContextRef context = CGBitmapContextCreate(fb.pixels,
                                                 fb.w, fb.h,
                                                 8,
                                                 bytes_per_row, 
                                                 rgbColorSpace,
                                                 bitmap_info);

    CGRect clip = CGRectMake(clip_rect->x, clip_rect->y,
                             clip_rect->w, clip_rect->h);
    CGContextClipToRect(context, clip); 

    CFMutableAttributedStringRef attr_str = create_attr_str(t);

    // Create the framesetter with the attributed string.
    CTFramesetterRef framesetter =
             CTFramesetterCreateWithAttributedString(attr_str);
    CFRelease(attr_str);

    // Create a path which bounds the area where you will be drawing text.
    // The path need not be rectangular.
    CGMutablePathRef path = CGPathCreateMutable();

    // In this simple example, initialize a rectangular path the size of
    // the eva framebuffer
    CGRect bounds = CGRectMake(bbox->x, 
                               (int32_t)fb.h - bbox->y - bbox->h,
                               bbox->w, bbox->h);
    CGPathAddRect(path, NULL, bounds);

    // Create a frame.
    CTFrameRef frame = CTFramesetterCreateFrame(framesetter,
              CFRangeMake(0, 0), path, NULL);
     
    // Draw the specified frame in the given context.
    CTFrameDraw(frame, context);
     
    // Release the objects we used.
    CFRelease(frame);
    CFRelease(path);
    CFRelease(framesetter);

    CGContextRelease(context);
    CGColorSpaceRelease(rgbColorSpace);
}

CFMutableAttributedStringRef create_attr_str(const text *t)
{
    eva_framebuffer fb = eva_get_framebuffer();
    CFStringRef str = CFStringCreateWithBytesNoCopy(
            NULL,                        // Null allocator for no allocs
            (uint8_t*)ustr_data(t->str), // Raw bytes
            (long)ustr_byte_len(t->str), // Num bytes
            kCFStringEncodingUTF16,      // UTF16
            false,                       // No Byte Order Marker
            kCFAllocatorNull);           // No dealloc

    // Create a mutable attributed string with a max length of len.
    long len = CFStringGetLength(str);
    CFMutableAttributedStringRef attr_str =
             CFAttributedStringCreateMutable(kCFAllocatorDefault, len);
     
    CFAttributedStringReplaceString(attr_str, CFRangeMake(0, 0), str);
    
    CFRelease(str);

    // TODO: Can color affect text extents? Unlikely...
    // Set default color for entire string before getting into text attrs.
   // CGColorRef white = CGColorGetConstantColor(kCGColorWhite);
   // CFAttributedStringSetAttribute(attr_str, CFRangeMake(0, len),
   //                                kCTForegroundColorAttributeName, white);

    text_attr *attr = t->attrs;
    while (attr) {
        CFRange r = CFRangeMake(attr->start, attr->len);

        // Font
        const char *font_family = get_font_family(attr->font_family);
        const void *keys[] = {kCTFontFamilyNameAttribute, kCTFontSizeAttribute};
        CFStringRef font_family_value = CFStringCreateWithBytesNoCopy(
                NULL, 
                (uint8_t*)font_family, (long)strlen(font_family), 
                kCFStringEncodingUTF8, 
                false, 
                kCFAllocatorNull);
        float scaled_font_size = attr->font_size * fb.scale_x;
        CFNumberRef font_size_value = CFNumberCreate(
                NULL,
                kCFNumberFloat32Type,
                &scaled_font_size);
        const void *values[] = { font_family_value, font_size_value };
        CFDictionaryRef font_attrs = CFDictionaryCreate(
                kCFAllocatorDefault, 
                keys,
                values,
                2,
                &kCFTypeDictionaryKeyCallBacks,
                &kCFTypeDictionaryValueCallBacks);

        CTFontDescriptorRef font_desc = 
            CTFontDescriptorCreateWithAttributes(font_attrs);
        CTFontRef font = CTFontCreateWithFontDescriptor(
                font_desc,
                scaled_font_size,
                NULL);
        CFAttributedStringSetAttribute(attr_str,
                                       r,
                                       kCTFontAttributeName,
                                       font);
       // CFStringRef x = CTFontCopyFullName(font);
       // CFStringRef y = CTFontCopyFamilyName(font);

       // printf("Font name %s\n", CFStringGetCStringPtr(x, kCFStringEncodingUTF8));
       // printf("Family name %s\n", CFStringGetCStringPtr(y, kCFStringEncodingUTF8));

       // CFRelease(x);
       // CFRelease(y);

        CFRelease(font);
        CFRelease(font_desc);
        CFRelease(font_attrs);
        CFRelease(font_size_value);

        // Color
        if (attr->color.r != 0.0f ||
            attr->color.g != 0.0f ||
            attr->color.b != 0.0f ||
            attr->color.a != 0.0f) {
            CGColorRef color = CGColorCreateGenericRGB(attr->color.r,
                                                       attr->color.g,
                                                       attr->color.b,
                                                       attr->color.a);
            CFAttributedStringSetAttribute(attr_str, r,
                                           kCTForegroundColorAttributeName,
                                           color);
            CFRelease(color);
        }

        attr = attr->next;
    }

    return attr_str;
}
