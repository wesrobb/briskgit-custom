call msvc15-64bit.bat

set MACHINE=X64
set LIBPREFIX_DTYPE=x64-win_64-dll
set LIBPREFIX_OTHER=x64-win_64-static\msvc15-mt
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
