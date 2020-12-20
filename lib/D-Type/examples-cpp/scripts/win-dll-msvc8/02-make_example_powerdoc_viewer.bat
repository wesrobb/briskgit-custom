echo -------------------------------------------------------------------------------
echo Build example_powerdoc_viewer.exe
echo -------------------------------------------------------------------------------

echo --- Step 1: Compile ---
cl.exe /W3 /Wp64 /nologo /TP /GR- /O2 /Ob1 /GF /FD /MT /Gy /D "_CRT_SECURE_NO_DEPRECATE" /D "WIN32" /D "_WINDOWS" /D "_CONSOLE" /D "NDEBUG" /D "_MBCS" /D "_LIB" /I "../../../includes/core-cpp/" /I "../../../includes/utils/" /Fp"example_powerdoc_viewer.pch" /c "..\..\..\examples-cpp\example_powerdoc_viewer\main.cpp"
echo --- Step 2: Link ---
link.exe kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"example_powerdoc_viewer.pdb" /machine:%MACHINE% /out:"example_powerdoc_viewer.exe" "main.obj" "..\..\..\engine\libs\%LIBPREFIX_DTYPE%\dtypestd%LIBEXTENSION_DTYPE%" "..\..\..\engine\libs\%LIBPREFIX_DTYPE%\dtypelay%LIBEXTENSION_DTYPE%" "..\..\..\engine\libs\%LIBPREFIX_DTYPE%\dtypeaux%LIBEXTENSION_DTYPE%" "..\..\..\engine\libs\%LIBPREFIX_DTYPE%\dtypepwr%LIBEXTENSION_DTYPE%" "..\..\..\other\libs\%LIBPREFIX_OTHER%\dtwindow.lib" "..\..\..\includes\utils\dtwindow\win\%VFWPREFIX%\msvfw32.lib"
echo Done.
echo -------------------------------------------------------------------------------
