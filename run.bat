@echo off

mkdir build
pushd build

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
call cl /Zi .\..\main.cpp user32.lib gdi32.lib

popd