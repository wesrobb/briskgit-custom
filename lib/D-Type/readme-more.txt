ADDITIONAL NOTES

Note: This file is available only to the users of custom built D-Type engines.

The following is a list of additional files that ship with your custom
version of D-Type engines:

* manuals/dt/funcs/dtImageDoOutput.htm
  A new page in D-Type Standard Engine Manual that describes the dtImageDoOutput
  function. This function can be used to render bitmap images in RGBA
  (32 bits per pixel), RGB (24 bits per pixel), GA (16 bits per pixel),
  G (8 bits per pixel) and monochrome (1 bit per pixel) format.

* examples-extra/example_bitmap_image/main.cpp
  A new source code example that shows how to use the dtImageDoOutput function.
  It renders three sample bitmap images stored in Windows BMP format.

* examples-extra/example_initialization_from_memory/main.cpp
  A new source code example that shows how to initialize D-Type Standard Engine
  entirely from memory (i.e. without the need to access any D-Type initialization
  files on disk).

* files/bmp/fish_bowl.bmp
  A bitmap image in Windows BMP format with an alpha-channel. This image is used
  in the example_bitmap_image example mentioned above. The Windows BMP file was
  generated using Pixelformer, a bitmap picture editor which supports Windows BMP
  images with an alpha channel.

* exec/examples-core_engine/example_basic_output_win.exe
  A sample program that shows the dtImageDoOutput function in action.
  Note: Select Page -> Example 18: Bitmap Images from the main menu to see the
  output of the dtImageDoOutput function. Choose the destination output Memory
  Surface (8, 16, 24 or 32 bpp) by selecting one of the Output options in the
  main menu.


# End of File
