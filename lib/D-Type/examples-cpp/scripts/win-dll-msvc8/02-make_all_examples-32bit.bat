call msvc8-32bit.bat

set MACHINE=I386
set LIBPREFIX_DTYPE=i386-win_32-dll
set LIBPREFIX_OTHER=i386-win_32-static\msvc8-mt
set LIBEXTENSION_DTYPE=.lib
set VFWPREFIX=x86

echo -------------------------------------------------------------------------------
echo Build all examples
echo -------------------------------------------------------------------------------
call 02-make_example_color_rasterizer.bat
call 02-make_example_basic_animation.bat
call 02-make_example_basic_output.bat
call 02-make_example_font_converter.bat
call 02-make_example_layout_extension.bat
call 02-make_example_powerdoc_output.bat
call 02-make_example_powerdoc_frames.bat
call 02-make_example_powerdoc_viewer.bat
call 02-make_example_text_animation.bat
call 02-make_example_text_output.bat
call 02-make_example_text_application.bat
call 02-make_example_textpro_builder1.bat
call 02-make_example_textpro_builder2.bat
echo -------------------------------------------------------------------------------
