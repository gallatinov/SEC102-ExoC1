if not exist "build\" mkdir build
pushd build
cmake -G "Visual Studio 15 2017" ../src
cmake --build . --target listdir
cmake --build . --target listproc
popd