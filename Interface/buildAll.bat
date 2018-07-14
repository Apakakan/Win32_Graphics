pwd
mkdir build
pushd build


cl /EHsc /DMATHLIB_EXPORTS /LD ..\MathLib.cpp

cl /EHsc /DGRAPHICSLIB_EXPORTS /LD ..\GraphicsLib.cpp /link MathLib.lib

cl -Zi /EHsc ..\GraphicsInterface.cpp /link GraphicsLib.lib MathLib.lib Gdi32.lib user32.lib /out:program.exe

popd