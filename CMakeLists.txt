cmake_minimum_required(VERSION 3.10)
project(HelloWorldSDL2 CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDL2_INCLUDE_DIR /opt/SDL2-2.26.1/include)
set(SDL2_LIBRARY /opt/aarch64-linux-gnu-7.5.0-linaro/sysroot/usr/lib/libSDL2-2.0.so.0.2600.1)
set(SDL2MAIN_LIBRARY /opt/aarch64-linux-gnu-7.5.0-linaro/sysroot/usr/lib/libSDL2main.a)

include_directories(${SDL2_INCLUDE_DIR} /opt/aarch64-linux-gnu-7.5.0-linaro/sysroot/usr/include/SDL2)

add_executable(calculator main.cpp)
target_link_libraries(calculator ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY} /opt/aarch64-linux-gnu-7.5.0-linaro/sysroot/usr/lib/libSDL2_ttf.a /opt/aarch64-linux-gnu-7.5.0-linaro/sysroot/usr/lib/libSDL2_ttf-2.0.so.0.10.3 /opt/aarch64-linux-gnu-7.5.0-linaro/sysroot/usr/lib/libfreetype.so -pthread)