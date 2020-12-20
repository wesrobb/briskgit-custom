call msvc15-32bit.bat

echo -------------------------------------------------------------------------------
echo Build dtwindow.lib
echo -------------------------------------------------------------------------------

echo --- Step 1: Compile ---
cl.exe /W4 /nologo /c /TP /std:c++17 /openmp- /permissive- /Zc:forScope /Zc:inline /GS- /GR- /O2 /Ob2 /Oi /Ot /D "_CRT_SECURE_NO_DEPRECATE" /D "WIN32" /D "NDEBUG" /D "_LIB" /D "_CRT_SECURE_NO_WARNINGS" /D "_MBCS" /GF /FD /MT /Gy /I "../../../includes/core-cpp/" /Fp"dtwindow.pch" /c "..\..\..\includes\utils\dtwindow\win\DTWindow.cpp"

echo --- Step 2: Link ---
lib.exe /nologo /out:"dtwindow.lib" "dtwindow.obj"

echo -------------------------------------------------------------------------------
echo Copy dtwindow.lib to the libs directory
echo -------------------------------------------------------------------------------

copy /V /Y dtwindow.lib ..\..\..\other\libs\i386-win_32-static\msvc15-mt\

echo -------------------------------------------------------------------------------
