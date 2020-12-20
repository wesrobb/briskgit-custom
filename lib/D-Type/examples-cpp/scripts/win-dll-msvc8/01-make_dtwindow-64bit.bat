call msvc8-64bit.bat

echo -------------------------------------------------------------------------------
echo Build dtwindow.lib
echo -------------------------------------------------------------------------------

echo --- Step 1: Compile ---
cl.exe /W3 /Wp64 /nologo /TP /GR- /O2 /Ob1 /GF /FD /MT /Gy /D "_CRT_SECURE_NO_DEPRECATE" /D "WIN32" /D "_WINDOWS" /D "_CONSOLE" /D "NDEBUG" /D "_MBCS" /D "_LIB" /I "../../../includes/core-cpp/" /Fp"dtwindow.pch" /c "..\..\..\includes\utils\dtwindow\win\DTWindow.cpp"

echo --- Step 2: Link ---
link.exe -lib /nologo /out:"dtwindow.lib" "dtwindow.obj"

echo -------------------------------------------------------------------------------
echo Copy dtwindow.lib to the libs directory
echo -------------------------------------------------------------------------------

copy /V /Y dtwindow.lib ..\..\..\other\libs\x64-win_64-static\msvc8-mt\

echo -------------------------------------------------------------------------------
