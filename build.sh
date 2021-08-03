rm -rf ./build
mkdir ./build
cd ./build
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/build/lib
#rm CMakeCache.txt
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
cmake --build . --config Debug 