
pwd
mkdir build
pushd build


cl /EHsc /DGRAPHICSLIB_EXPORTS /LD ..\GraphicsLib.cpp /link MathLib.lib
popd