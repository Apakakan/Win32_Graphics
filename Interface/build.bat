pwd
mkdir build
pushd build

cl -Zi /EHsc ..\GraphicsInterface.cpp /link GraphicsLib.lib Gdi32.lib user32.lib /out:program.exe

popd