echo -------------------------------------------------------------------------------
echo Copy executables to the appropriate exec directory
echo -------------------------------------------------------------------------------

copy /V /Y example_color_rasterizer.exe ..\..\..\exec\a-rasterizer\
copy /V /Y example_basic_animation.exe ..\..\..\exec\b-foundation\
copy /V /Y example_basic_output.exe ..\..\..\exec\b-foundation\
copy /V /Y example_font_converter.exe ..\..\..\exec\b-foundation\
copy /V /Y example_layout_extension.exe ..\..\..\exec\b-foundation\
copy /V /Y example_powerdoc_output.exe ..\..\..\exec\c-powerdoc\
copy /V /Y example_powerdoc_frames.exe ..\..\..\exec\c-powerdoc\
copy /V /Y example_powerdoc_viewer.exe ..\..\..\exec\c-powerdoc\
copy /V /Y example_text_animation.exe ..\..\..\exec\d-text\
copy /V /Y example_text_output.exe ..\..\..\exec\d-text\
copy /V /Y example_text_application.exe ..\..\..\exec\d-text\
copy /V /Y example_textpro_builder1.exe ..\..\..\exec\e-textpro\
copy /V /Y example_textpro_builder2.exe ..\..\..\exec\e-textpro\

echo -------------------------------------------------------------------------------
