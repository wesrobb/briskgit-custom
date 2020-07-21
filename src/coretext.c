#include "coretext.h"

#include <CoreText/CoreText.h>

#include "eva/eva.h"

#include "rect.h"
#include "text.h"
#include "ustr.h"
#include "vec2.h"

void coretext_draw_font(eva_framebuffer *fb, 
                        const recti *clip_rect,
                        char *text, int32_t len,
                        int32_t pt_size, const vec2i *pos)
{
    uint32_t bitmap_info = kCGImageAlphaPremultipliedFirst |
                           kCGBitmapByteOrder32Little;

    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    size_t bytes_per_row = fb->pitch * 4;
    CGContextRef context = CGBitmapContextCreate(fb->pixels,
                                                 fb->w, fb->h,
                                                 8,
                                                 bytes_per_row, 
                                                 rgbColorSpace,
                                                 bitmap_info);

    CGRect clip = CGRectMake(clip_rect->x, clip_rect->y,
                             clip_rect->w, clip_rect->h);
    CGContextClipToRect(context, clip); 

    CGColorRef white = CGColorGetConstantColor(kCGColorWhite);

    CFStringRef cf_string = CFStringCreateWithBytes(kCFAllocatorDefault,
                                                    (uint8_t*)text, len,
                                                    kCFStringEncodingUTF8,
                                                    false);

    // Create a mutable attributed string with a max length of 0.
    // The max length is a hint as to how much internal storage to reserve.
    // 0 means no hint.
    CFMutableAttributedStringRef attrString =
             CFAttributedStringCreateMutable(kCFAllocatorDefault, 0);
     
    // Copy the textString into the newly created attrString
    CFAttributedStringReplaceString(attrString, CFRangeMake(0, 0), cf_string);

    CFIndex cf_string_len = CFStringGetLength(cf_string);

    // Set color
    CFAttributedStringSetAttribute(attrString, CFRangeMake(0, cf_string_len),
                                   kCTForegroundColorAttributeName, white);
    
    // Set the font size
    CFStringRef font_name = CFStringCreateWithCString(kCFAllocatorDefault,
                                                      "Helvetica",
                                                      kCFStringEncodingUTF8);

    // Assumes scale_x == scale_y
    CTFontRef font = CTFontCreateWithName(font_name, pt_size * fb->scale_x, 0);
    CFAttributedStringSetAttribute(attrString,
                                   CFRangeMake(0, cf_string_len),
                                   kCTFontAttributeName,
                                   font);
    CFRelease(font);
    CFRelease(font_name);
    CFRelease(cf_string);

    // Create the framesetter with the attributed string.
    CTFramesetterRef framesetter =
             CTFramesetterCreateWithAttributedString(attrString);
    CFRelease(attrString);

    // Find a frame that will fit the string.
    CFRange string_range = {0};
    CGSize frame_constraints = { CGFLOAT_MAX, CGFLOAT_MAX };
    CFRange fit_range = {0};
    CGSize frame_size =  CTFramesetterSuggestFrameSizeWithConstraints(
            framesetter, string_range, NULL, frame_constraints, &fit_range);

    // Create a path which bounds the area where you will be drawing text.
    // The path need not be rectangular.
    CGMutablePathRef path = CGPathCreateMutable();

    // In this simple example, initialize a rectangular path the size of
    // the eva framebuffer
    CGRect bounds = CGRectMake(pos->x, 
                               (int32_t)fb->h - pos->y - frame_size.height,
                               frame_size.width, frame_size.height);
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
