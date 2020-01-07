cd C:\Users\TJ\Documents\programming\python\PySolar

DOSKEY gcc="C:\Program Files\mingw-w64\x86_64-8.1.0-win32-seh-rt_v6-rev0\mingw64\bin\gcc.exe" $*
DOSKEY make="C:\Program Files\mingw-w64\x86_64-8.1.0-win32-seh-rt_v6-rev0\mingw64\bin\mingw32-make.exe" $*
DOSKEY link=gcc -c $*
DOSKEY build=gcc -shared -o lib.dll $*
DOSKEY h=DOSKEY /HISTORY
DOSKEY alias=if ".$*." == ".." ( DOSKEY /MACROS ) else ( DOSKEY $* )