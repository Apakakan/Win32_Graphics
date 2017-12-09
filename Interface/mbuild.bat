pwd
mkdir build
pushd build


cl /EHsc /DMATHLIB_EXPORTS /LD ..\MathLib.cpp
popd