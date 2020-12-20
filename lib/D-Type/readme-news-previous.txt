WHAT'S NEW IN D-TYPE ENGINES

VERSION 7.0.2.2 - AUGUST 2019

- Standard Engine:

  * TrueType font driver: In same cases composite glyphs made of composite glyphs
    themselves were not rendered or were rendered partially. This issue is now
    fixed. All applications that rely on D-Type 6 or previous D-Type 7 releases
    should upgrade.

  * Two new stream types: File-Backed URL Stream (FURL) and Memory-Backed URL
    Stream (MURL). They are very similar to the standard URL Stream but much
    faster. See D-Type Manual for details.

  * Improved existing URL Stream so it supports web servers that incorrectly
    send LF+LF at the end of their HTTP response headers rather than CRLF+CRLF.

  * Improved existing URL Stream so it supports both IPv4 and IPv6.

- All engines:

  * Various small tweaks and improvements.

- Updated manuals

- PowerDoc Editor for Windows is now linked to D-Type static libraries in the
  same manner as PowerDoc Editor for Linux and macOS; consequently D-Type .dll
  libraries have been removed from the exec/tools/ folder.


VERSION 7.0.2.1 - JULY 2019

- Standard Engine:

  * OpenType CFF Font Driver: Support for incomplete OpenType CFF fonts, i.e. OpenType
    containers that include the essential CFF table but lack other required OpenType
    tables. Such fonts are sometimes found in PDF files. Note that these incomplete
    files are not bare CFF fonts (which were always supported by D-Type); these are
    incomplete/invalid OpenType fonts. D-Type Font Engine can now process them as
    fully functional OpenType CFF fonts.

    For this enhancement to work, the Flags field of the font's corresponding font
    factory (located inside the CDTFontOpenTypeCff Font Factory section of dtype.inf)
    must contain the ACCEPT_MALFORMED_FONT keyword but not the USE_OT_GLYPH_METRICS
    keyword. The font file itself must have at least the CFF table. If other OpenType
    tables are available, D-Type's OpenType CFF driver might be able to use them;
    otherwise any missing OpenType data will be generated based on the information
    found in the CFF table.

  * CFF Font Driver: CFF fonts with an empty String INDEX are now accepted.
    A few other small tweaks and improvements.

  * Small enhancement to the Type 1 Font Driver to fully support the increased
    CharString size limit (first introduced in D-Type 7.0.1.1).

- Text Engine: Format text correctly even if the text selection extends beyond the
  last character in a text flow.

- PowerDoc Engine: New filtering option added to pdDocSetFilter (bit 4) which makes
  it possible to draw frames without text.

- PowerDoc Editor:

  - Transparency supported when exporting to PNG
  - Export to PDF and SVG
  - Print to vector-based PDF
  - Updated libpng
  - Switched to libjpeg-turbo
  - More math objects

- macOS: Improvements to dtwindow, improved Retina support in PowerDoc Editor.

- macOS: PowerDoc Editor and all examples are now built as 64-bit applications
  and support Retina high-dpi resolution.

- Linux: PowerDoc Editor and all examples are now built as 64-bit applications.


VERSION 7.0.1.1 - OCTOBER 2018

- Standard Engine:

  * Type 1 Font Driver: CharString size limit increased from 30 KB
    to 96 KB per glyph.

  * dtextras.cpp: Fixed a regression in dtxConvertCharsToGlyphs.

- Power Engine: New PowerDoc property: pdLinePerimeterGrow

- PowerDoc Editor:

  * All platforms: Small improvements.

  * Mac OS X and macOS only: PowerDoc Editor is now built as a
    64-bit application and supports Retina high-dpi resolution.

- Font Utility:

  * Settings dialog: New options.

  * Other small improvements.

- Helper library for platform independent window display (dtwindow):

  * Mac OS X and macOS only: Retina support.

- Form Engine (dtform): Documented new values supported by fcOnCtrlChange,
  fcOnCtrlFocusLose, fcOnCtrlFocusReceive, fcOnCtrlHold, fcOnCtrlPush,
  fcOnCtrlRelease and fcOnCtrlScroll control handlers.

- All engines: New C++ examples improved. Old C examples removed.


VERSION 7.0.1.0 - AUGUST 2018

- All engines: Official support for OpenType 1.8 variable fonts has
  been added. Both TrueType and CFF2 outlines are supported.

- Standard Engine (dtshapes.cpp): New line start/end styles:
  DV_LINESTYLE_SQUARE_INSIDE and DV_LINESTYLE_SQUARE_OUTSIDE

- Power Engine: pdLineStart and pdLineEnd support new line start/end
  style options: 5 = Square outside, 6 = Square inside

- Power Engine: Compact polygon instructions 254 and 255

- Power Engine: New PowerDoc objects:

  * Typographic Frame
  * Text Extras
  * Text Baseline
  * Row Boundary

- Power Engine: New set of text style/formatting options

- Text Engine: New set of text style/formatting attributes

- PowerDoc Editor: Text Format dialog supports many new text style
  options.

- All engines:

  * Many other internal enhancements
  * On Windows dropped support for MSVC6
  * Removed obsolete functions


VERSION 6.5.0.2 - JUNE 2018

- D-Type Standard Engine: CFF Font Driver: Improved precision when
processing CFF fonts that store point coordinates in 16.16 fixed
format


VERSION 6.5.0.1 - MAY 2018

- D-Type Standard Engine: New functions: dtTypesetterSetTypeEffects,
  dtTypesetterGetTypeEffects, dtTypesetterSetTypeEffectsTransformed,
  dtFontGetMMAxisInfo, dtFontAddAsMMInstancePlus, dtShapesDoOutput,
  dtShapesDoOutputFrac

- D-Type Standard Engine: Obsolete functions: dtShapeDoOutput,
  dtShapeDoOutputFrac, dtShapeDoOutputPlus, dtShapeDoOutputFracPlus,
  dtFontGetDim

- All D-Type Engines:

  * Internal enhancements

  * Static libraries for Linux are now compiled as position independent
    code (see the -fPIC flag)

  * Added 32 and 64-bit static libraries for MS Visual Studio 2017:

    engine\libs\i386-win_32-static\msvc15-mt\
    engine\libs\x64-win_64-static\msvc15-mt\

- D-Type Examples:

  * New, simplified examples (see the examples-cpp\ folder)

  * Added MS Visual Studio 2017 projects:

    examples\projects\msvc15\
    examples-cpp\projects\msvc15\

- Shading Rasterizer: Two new color interpolation algorithms (Distance Field)

- PowerDoc Editor: Added a new PowerDoc document that demonstrates various
  color interpolation algorithms supported by D-Type Shading Rasterizer

- PowerDoc Editor: Small improvements to the Text Format dialog

- libpng upgraded to 1.6.34

- Updated manuals


VERSION 6.0.3.8 - FEBRUARY 2018

- Standard Engine, Power Engine: Added APIs to access the new Shading Rasterizer
  (still not documented).

- Standard Engine: Fixed a bug introduced in D-Type release 6 that was responsible
  for not properly closing temporary files that are created in D-Type's system/temp
  folder during font processing. This bug could only be triggered in a situation
  when the operating system exceeded the "maximum number of open files per process"
  limit.

- Text Engine: Minor internal improvements.


VERSION 6.0.3.3 - DECEMBER 2017

- Standard Engine: The dtFontGetStringValue function supports several new
  string values:

  DV_SVAL_ASC_MANUFACTURER, DV_SVAL_UNI_MANUFACTURER,
  DV_SVAL_ASC_DESIGNER, DV_SVAL_UNI_DESIGNER,
  DV_SVAL_ASC_DESCRIPTION, DV_SVAL_UNI_DESCRIPTION,
  DV_SVAL_ASC_SAMPLE, DV_SVAL_UNI_SAMPLE,
  DV_SVAL_ASC_TYPOFAMILYNAME, DV_SVAL_UNI_TYPOFAMILYNAME,
  DV_SVAL_ASC_TYPOSUBFAMILYNAME, DV_SVAL_UNI_TYPOSUBFAMILYNAME,
  DV_SVAL_ASC_WWSFAMILYNAME, DV_SVAL_UNI_WWSFAMILYNAME,
  DV_SVAL_ASC_WWSSUBFAMILYNAME, DV_SVAL_UNI_WWSSUBFAMILYNAME

- Standard Engine: The dtFontGetNumericValue function supports several new
  numeric values:

  DV_NVAL_SELECTION_FLAGS,
  DV_NVAL_WEIGHT_CLASS,
  DV_NVAL_WIDTH_CLASS,
  DV_NVAL_FAMILY_CLASS

- Standard Engine: Various internal changes and enhancements

- Power Engine: Support for breakable tables and matrices

- Power Engine: Improved precision when rendering Text Path and Rich Text
  Path objects

- Power Engine: Various internal changes and enhancements

- PowerDoc Editor: New menu options:

  Text -> Insert Matrix/Table Column - Before
  Text -> Insert Matrix/Table Column - After
  Text -> Insert Matrix/Table Row - Before
  Text -> Insert Matrix/Table Row - After
  Text -> Matrix/Table Settings...

- PowerDoc Editor: Various improvements to the Text Format dialog

- Updated manuals


VERSION 6.0.2.1 - MAY 2017

- Standard Engine: New styles:

  46 = Stack Blur - only horizontal
  47 = Stack Blur - only vertical
  48 = Stack Blur - horizontal and vertical
  49 = Horizontal bold with vertical Gaussian blur
  50 = Vertical bold with horizontal Gaussian blur
  51 = Horizontal bold with vertical stack blur
  52 = Vertical bold with horizontal stack blur
  55 = Outline - only horizontal
  56 = Outline - only vertical
  57 = Outline - horizontal and vertical

- Standard Engine: a new algorithm that significantly improves the
  appearance of non-hinted fonts at small and medium sizes (use the
  latest dtype.inf file to activate this algorithm)

- Standard Engine: another new algorithm that improves the appearance of
  certain classes of auto-hinted glyphs at medium sizes (e.g. Palatino)

- Standard Engine: Increased maximum number of allowed glyph points and
  contours in the TrueType font driver to support some extremely complex
  glyph outlines

- Standard Engine: Added support for an obsolete and deprecated "endchar"
  operator function (corresponding to the Type 1 charstring command "seac"),
  which can be found in some OpenType CFF fonts

- Power Engine: New styles:

  46 = Stack Blur - only horizontal
  47 = Stack Blur - only vertical
  48 = Stack Blur - horizontal and vertical
  49 = Horizontal bold with vertical Gaussian blur
  50 = Vertical bold with horizontal Gaussian blur
  51 = Horizontal bold with vertical stack blur
  52 = Vertical bold with horizontal stack blur
  55 = Outline - only horizontal
  56 = Outline - only vertical
  57 = Outline - horizontal and vertical

- Power Engine: 16bpp surfaces added to Key Memory Surfaces

- Power Engine: Two new PowerDoc objects: pdDrawSysGuide and
  pdDrawSysGuideDashed

- Unicode Text Engine: Fixed a glitch in CJK line breaker which in some
  rare situations could cause line breaks to occur after a specific set of
  characters rather than before. For example, in CJK scripts a line break
  should occur before fullwidth numerals (0xFF10 - 0xFF19) and not after.
  This glitch was affecting only a very small set of characters associated
  with CJK (or Inherited) scripts.

- PowerDoc Editor: All dialogs are now resizable

- PowerDoc Editor: New templates added to the Place PowerDoc Template dialog:

  * Guides - Solid
  * Guides - Dashed

- All engines: Internal code improvements, optimizations and enhancements

- Examples: Added MS Visual Studio 2017 solution (examples\projects\msvc15)

- Latest version of HarfBuzz (1.4.6), libpng (1.6.29) and zlib (1.2.11)

- Added readme files re: Directory Structure, Font Tool and font conversion

- Updated manuals


VERSION 6.0.1.0 - NOVEMBER 2016

General Highlights

- Features and enhancements designed to simplify common tasks, boost
  performance and enable additional capabilities.
- Overall modernization of the engine. Simplified folders/includes and
  configuration files.
- New functions; obsolete or non-documented features removed. All existing
  APIs are still fully backward compatible with Version 5.
- Performance boost; parallel rendering in several threads is now possible.
  On Linux this requires linking to POSIX Threads (Pthreads).

Standard Engine

- The ability to automatically recognize the font format from the supplied
  file/stream.
- New parameters added to the dtype.inf initialization file: C8) Different
  fonts can share the same .ccv file, C9) Maximum size of font cache, and
  D2) Default Font Format Detection Order
- Support for vertical metrics found in TrueType/OpenType fonts.
- A select group of direct color and grayscale rasterizers support parallel
  rasterization.
- Internal improvements to the generic stream management subsystem.
- Internal improvements to the temporary file management subsystem.
- The API has been split into two parts: low-level and high-level.
- Updated all Char Translation and Alignment (.ccv) files that are used by
  font factories.

Text Layout Extension

- HarfBuzz text shaping engine has been upgraded to the latest version
  (1.3.2).
- Text Layout Extension based on the ICU LayoutEngine no longer ships with
  D-Type Engines.

Power Engine

- Performance boost thanks to parallel rasterization provided by Standard
  Engine. Additionally, a select group of PowerDoc objects also support
  parallel rasterization.
- New parameter in the dtype.inf initialization file: Maximum number of
  auxiliary worker threads for D-Type Power Engine to execute asynchronous
  routines. This makes it possible to configure the number of threads for
  parallel rasterization.
- Internal improvements.

Text Engine

- Performance boost thanks to parallel rasterization provided by Standard
  Engine.
- New capabilities added to the font map file (fontmap.inf).
- The API has been split into two parts: low-level and high-level.

PowerDoc Editor

- New features and capabilities, performance enhancements and many internal
  improvements.

PowerDoc For ASP/.NET

- Added new functions and examples (TextAppendFromString, TextSetAttribs).

Miscellaneous

- D-Type Font Viewer, which used to be a standalone Windows-only program, is
  now integrated into D-Type PowerDoc Editor as a module called D-Type Font
  Utility. Other than being a fully portable program, it provides many
  additional functions that were not available in the old Font Viewer.
- D-Type Font Converter has been rebuilt to take advantage of the latest
  Standard Engine enhancements.
- Internal improvements to D-Type's helper library for platform independent
  window display (dtwindow).
- libpng library has been updated to the latest version (1.6.26).
- Updates to D-Type manuals.


# End of File
