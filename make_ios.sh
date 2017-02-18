# build projects
cmake -DCMAKE_TOOLCHAIN_FILE=../Project/ios.cmake -DIOS_PLATFORM=OS -HSource -BBuildIOS -GXcode -DCMAKE_BUILD_TYPE=Debug
cmake --build BuildIOS --config Debug