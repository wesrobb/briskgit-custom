D-TYPE RENDERING ENGINES

A suite of high-performance and portable software libraries for rendering
high-quality text and resolution independent scalable 2D graphics.

Website: http://www.d-type.com
E-mail: support@d-type.com

To see what's new in this release, see the file readme-news.txt.

For additional notes, which are included with a custom built version
of D-Type engines, see readme-more.txt.

For information on how to convert existing font formats (e.g. TrueType,
OpenType, Type 1, CFF) to D-Type font format, see readme-fontconv.txt.

To review your D-Type engine license, open the readme-license.txt file.


DIRECTORY STRUCTURE OF THIS PACKAGE

* engine/libs/
  Contains D-Type rendering engines as static, shared and/or dynamic
  libraries for the selected platform(s).

* engine/system/
  Contains supporting D-Type system files that may be used during the
  initialization of D-Type engines.

* examples/ and examples-cpp/
  Contains source code examples that illustrate how to use common
  D-Type engine functions and features.

* examples-extra/
  Contains additional source code examples that illustrate how to use
  specific D-Type engine features, functions and/or APIs. Available to
  the users of custom built D-Type engines only.

* exec/com
  Contains D-Type PowerDoc for ASP/.NET COM object. Available to the
  users of D-Type PowerDoc for ASP/.NET component only.

* exec/examples-color_rasterizer
  Contains a compiled example program that demonstrates D-Type Direct
  Color Rasterizer features and capabilities.

* exec/examples-core_engine
  Contains several compiled example programs that demonstrates D-Type
  Standard Engine features and capabilities. Available to all users.

* exec/examples-power_engine
  Contains several compiled example programs that demonstrates D-Type
  Power Engine features and capabilities. Available to the users of
  D-Type Power Engine.

* exec/examples-text_engine
  Contains several compiled example programs that demonstrates D-Type
  Text Engine features and capabilities. Available to the users of
  D-Type Text Engine.

* exec/tools
  Contains supporting tools in binary format: D-Type PowerDoc Editor
  and D-Type Font Utility. See http://d-type.com/page/powerdoc_editor
  for more information on D-Type PowerDoc Editor. D-Type Font Utility
  is actually a D-Type PowerDoc Editor module, which can be used
  as a standalone tool to view, validate and convert fonts. For more
  details on the font conversion process, see readme-fontconv.txt.

* files/
  Contains supporting resource files: bitmap images (in BMP, PNG, JPEG
  and RAW format), plain text files, demo PowerDoc documents and shape
  definition tables, hyphenation dictionaries for D-Type Text Engine
  and other resources.

* fonts/
  Contains supporting fonts in different formats (e.g. D-Type, TrueType,
  OpenType, Type 1, CFF).

* includes/core
  Contains C/C++ headers for all D-Type engines (core functions).

* includes/extras
  Additional D-Type engine functions in source code format (auxiliary
  functions).

* includes/utils
  Contains utility and helper D-Type functions in source code format:
  D-Type's helper library for platform independent window display and
  D-Type's helper library for platform independent forms.

* manuals/
  Contains D-Type Engine manuals in HTML format.

* other/
  Contains miscellaneous files and libraries.


# End of File
