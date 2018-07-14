
pwd
mkdir build
pushd build

cl -Zi ..\D3D_Demo.cpp ..\Camera.cpp ..\Model.cpp /link user32.lib Gdi32.lib  /out:program.exe
popd