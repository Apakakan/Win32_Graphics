
pwd
mkdir build
pushd build

cl -Zi ..\win32_window.cpp user32.lib Gdi32.lib /link /out:program.exe
popd