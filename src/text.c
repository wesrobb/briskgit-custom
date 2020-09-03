#include "text.h"

#include <assert.h>
#include <stdio.h>

#include <CoreFoundation/CFNumber.h>
#include <CoreText/CoreText.h>

#include "eva/eva.h"

#include "common.h"
#include "color.h"
#include "hash.h"
#include "profiler.h"
#include "rect.h"
#include "ustr.h"
#include "vec2.h"

#define MAX_TEXT_ATTRS 2048

typedef struct text_attr {
    int32_t start;
    int32_t len;

    font_family_id font_family;
    double font_size;
    color color;

    text_attr *next;
} text_attr;

typedef struct text {
    ustr *str;
    text_attr *attrs; // Linked list of text attributes
    int32_t ref;

    bool cached_extents;
    vec2 extents;

    bool cached_metrics;
    double width;
    double leading;
    double ascent;
    double descent;
} text;

typedef struct text_ctx {
    // Text attribute storage.
    text_attr attrs[MAX_TEXT_ATTRS];

    // Text attribute free list.
    text_attr *free_list;

    bool initialized;
} text_ctx;

static text_ctx _ctx;

static text_attr* get_next_attr();
static void free_attr(text_attr *);
static const char * get_font_family(font_family_id f);
static void text_extents_macos(const text *t, vec2 *dst);
static void text_metrics_macos(const text *t, double *width, double *leading,
                               double *ascent, double *descent);
static double text_index_offset_macos(const text *t, size_t index);
static void text_draw_macos(const text *t, const rect *bbox, const rect *clip);
static void text_draw_macos2(const text *t, const rect *bbox, const rect *clip);
static CFMutableAttributedStringRef create_attr_str(const text *t);
static CTLineRef create_trunc_token(CFMutableAttributedStringRef attr_str);

void text_system_init()
{
    // Setup the free list.
    for (int32_t i = 0; i < MAX_TEXT_ATTRS - 1; i++) {
        _ctx.attrs[i].next = &_ctx.attrs[i + 1];
    }
    _ctx.free_list = _ctx.attrs;
}

text* text_create(void)
{
    text *t = malloc(sizeof(text));
    if (!t) {
        // TODO: Handle failure.
        return NULL;
    }

    t->str = ustr_create();
    assert(t->str);
    t->attrs = NULL;
    t->ref = 1;
    t->cached_extents = false;
    t->extents.x = 0;
    t->extents.y = 0;

    return t;
}

text* text_create_ustr(ustr *str)
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
    t->cached_extents = false;
    t->extents.x = 0;
    t->extents.y = 0;

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
    t->cached_extents = false;
    t->extents.x = 0;
    t->extents.y = 0;

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
                   font_family_id font_family, double font_size,
                   const color *c)
{
    assert(t);
    assert(c);

    text_attr *a = get_next_attr();
    assert(a);

    a->start = start;
    a->len = len;
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

    // Invalidate the cache since text attributes have changed.
    t->cached_extents = false;
}

void text_extents(const text *t, vec2 *dst)
{
    assert(t);
    assert(dst);

    if (t->cached_extents) {
        *dst = t->extents;
    }
    else {

#ifdef BG_MACOS
        text_extents_macos(t, dst);
#elif BG_WINDOWS
        assert(false);
#endif

        // Const gets in the way of opaque caching systems.
        text *txt = (text*)t;
        txt->cached_extents = true;
        txt->extents = *dst;
    }
}

void text_metrics(const text *t, double *width, double *leading,
                  double *ascent, double *descent)
{
    assert(t);
    assert(width);
    assert(leading);
    assert(ascent);
    assert(descent);

    if (t->cached_metrics) {
        *width = t->width;
        *leading = t->leading;
        *ascent = t->ascent;
        *descent = t->descent;
    }
    else {

#ifdef BG_MACOS
        text_metrics_macos(t, width, leading, ascent, descent);
#elif BG_WINDOWS
        assert(false);
#endif

        // Const gets in the way of opaque caching systems.
        text *txt = (text*)t;
        txt->cached_metrics = true;
        txt->width = *width;
        txt->leading = *leading;
        txt->ascent = *ascent;
        txt->descent = *descent;
    }
}

double text_index_offset(const text *t, size_t index)
{
    assert(t);
    assert(t->str);
    assert(index >= 0);
    assert(index <= ustr_len(t->str));


#ifdef BG_MACOS
    return text_index_offset_macos(t, index);
#elif BG_WINDOWS
    assert(false);
#endif
}

void text_draw(const text *t, const rect *bbox, const rect *clip)
{
    assert(t);
    assert(bbox);

#ifdef BG_MACOS
    text_draw_macos2(t, bbox, clip);
#elif BG_WINDOWS
    assert(false);
#endif
}

void text_hash(const text *t, uint32_t *v)
{
    ustr_hash(t->str, v);
    text_attr *a = t->attrs;
    while (a) {
        hash(v, (uint8_t*)a, sizeof(*a));
        a = a->next;
    }
}

bool text_hit(const text *t, const vec2 *pos, size_t *index)
{
    assert(t);
    assert(pos);

    vec2 extents;
    text_extents(t, &extents);
    if (pos->x <= extents.x && pos->y <= extents.y) {
        CFMutableAttributedStringRef attr_str = create_attr_str(t);
        CTTypesetterRef ts = CTTypesetterCreateWithAttributedString(attr_str);
        CTLineRef line = CTTypesetterCreateLine(ts, CFRangeMake(0, 0));
        CGPoint ct_pos = CGPointMake(pos->x, pos->y);

        CFIndex ct_index = CTLineGetStringIndexForPosition(line, ct_pos);

        CFRelease(line);
        CFRelease(ts);
        CFRelease(attr_str);

        if (ct_index == kCFNotFound) {
            return false;
        }

        *index = (size_t)ct_index;
        return true;
    }

    return false;
}

void text_append(text *t, const uint16_t *data, size_t len)
{
    assert(t);
    assert(data);

    ustr_append(t->str, data, len);
    t->cached_extents = false;
}

static text_attr* get_next_attr()
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

static void free_attr(text_attr *ta)
{
    assert(ta);

    ta->next = _ctx.free_list;
    ta->color = COLOR_ZERO;
    ta->start = 0;
    ta->len = 0;

    _ctx.free_list = ta;
}

static const char * get_font_family(font_family_id f)
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

static void text_extents_macos(const text *t, vec2 *dst)
{
    assert(t);
    assert(dst);

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

    dst->x = frame_size.width;
    dst->y = frame_size.height;

    CFRelease(framesetter);
}

static void text_metrics_macos(const text *t, double *width, double *leading,
                               double *ascent, double *descent)
{
    CFMutableAttributedStringRef attr_str = create_attr_str(t);
    CTTypesetterRef ts = CTTypesetterCreateWithAttributedString(attr_str);
    CTLineRef line = CTTypesetterCreateLine(ts, CFRangeMake(0, 0));

    *width = CTLineGetTypographicBounds(line, ascent, descent, leading);

    CFRelease(line);
    CFRelease(ts);
    CFRelease(attr_str);
}

static double text_index_offset_macos(const text *t, size_t index)
{
    CFMutableAttributedStringRef attr_str = create_attr_str(t);
    CTTypesetterRef ts = CTTypesetterCreateWithAttributedString(attr_str);
    CTLineRef line = CTTypesetterCreateLine(ts, CFRangeMake(0, 0));
    double offset = CTLineGetOffsetForStringIndex(line, (CFIndex)index,
                                                        NULL);

    CFRelease(line);
    CFRelease(ts);
    CFRelease(attr_str);

    return offset;
}

static void text_draw_macos(const text *t, const rect *bbox,
                            const rect *clip)
{
    eva_framebuffer fb = eva_get_framebuffer();
    int32_t fb_height = (int32_t)fb.h;
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

    // Core text uses bottom-up coords but we use top down so we have to
    // invert our clip rect here.
    double inverted_clip_y = fb_height - clip->y - clip->h;
    CGRect cg_clip = CGRectMake(clip->x, inverted_clip_y, clip->w, clip->h);
    CGContextClipToRect(context, cg_clip); 

    CFMutableAttributedStringRef attr_str = create_attr_str(t);

    // Create the framesetter with the attributed string.
    CTFramesetterRef framesetter =
             CTFramesetterCreateWithAttributedString(attr_str);
    CFRelease(attr_str);

    // Create a path which bounds the area where you will be drawing text.
    // The path need not be rectangular.
    CGMutablePathRef path = CGPathCreateMutable();

    CGRect bounds = CGRectMake(bbox->x, 
                               fb_height - bbox->y - bbox->h,
                               bbox->w, bbox->h);
    CGPathAddRect(path, NULL, bounds);

    // Create a frame.
    CTFrameRef frame = CTFramesetterCreateFrame(framesetter,
              CFRangeMake(0, 0), path, NULL);
     
    profiler_begin_name("CTFrameDraw");
    // Draw the specified frame in the given context.
    CTFrameDraw(frame, context);
    profiler_end;
     
    // Release the objects we used.
    CFRelease(frame);
    CFRelease(path);
    CFRelease(framesetter);

    CGContextRelease(context);
    CGColorSpaceRelease(rgbColorSpace);
}

static void text_draw_macos2(const text *t, const rect *bbox,
                             const rect *clip)
{
    eva_framebuffer fb = eva_get_framebuffer();
    int32_t fb_height = (int32_t)fb.h;
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

    // Core text uses bottom-up coords but we use top down so we have to
    // invert our clip rect here.
    double inverted_clip_y = fb_height - clip->y - clip->h;
    CGRect cg_clip = CGRectMake(clip->x, inverted_clip_y, clip->w, clip->h);
    CGContextClipToRect(context, cg_clip); 

    CFMutableAttributedStringRef attr_str = create_attr_str(t);

    // Create the framesetter with the attributed string.
    CTTypesetterRef ts = CTTypesetterCreateWithAttributedString(attr_str);

    // Create a line.
    CTLineRef line = CTTypesetterCreateLine(ts, CFRangeMake(0, 0));

    CGRect line_bounds = CTLineGetImageBounds(line, context);
    if (line_bounds.size.width > bbox->w) {
        CTLineRef truncation_token = create_trunc_token(attr_str);

        CTLineRef trunc_line = CTLineCreateTruncatedLine(
                line, bbox->w, kCTLineTruncationEnd, truncation_token);
        CFRelease(truncation_token);
        CFRelease(line);
        line = trunc_line;
    }
    CFRelease(attr_str);

    CGContextSetTextPosition(context, bbox->x, fb_height - bbox->y - bbox->h);
     
    profiler_begin_name("CTLineDraw");
    // Draw the specified frame in the given context.
    CTLineDraw(line, context);
    profiler_end;
     
    // Release the objects we used.
    CFRelease(line);
    CFRelease(ts);

    CGContextRelease(context);
    CGColorSpaceRelease(rgbColorSpace);
}

static CFMutableAttributedStringRef create_attr_str(const text *t)
{
    profiler_begin;
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
        size_t len = attr->len == 0 ? ustr_len(t->str) : (size_t)attr->len;
        CFRange r = CFRangeMake(attr->start, (int32_t)len);

        // Font
        const char *font_family = get_font_family(attr->font_family);
        const void *keys[] = {kCTFontFamilyNameAttribute, kCTFontSizeAttribute};
        CFStringRef font_family_value = CFStringCreateWithBytesNoCopy(
                NULL, 
                (uint8_t*)font_family, (long)strlen(font_family), 
                kCFStringEncodingUTF8, 
                false, 
                kCFAllocatorNull);
        double scaled_font_size = attr->font_size * fb.scale_x;
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

    profiler_end;
    return attr_str;
}

static CTLineRef create_trunc_token(CFMutableAttributedStringRef attr_str)
{
    CFRange range;
    CFDictionaryRef existing_attrs =
        CFAttributedStringGetAttributes(attr_str, 0, &range);

    CFAttributedStringRef trunc_str =
        CFAttributedStringCreate(NULL, CFSTR("\u2026"), existing_attrs);
    CTLineRef token = CTLineCreateWithAttributedString(trunc_str);

    CFRelease(trunc_str);

    return token;
}
