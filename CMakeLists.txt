cmake_minimum_required(VERSION 3.21)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VCPKG_TARGET_TRIPLET x64-windows-static)
include("D:/vcpkg/scripts/buildsystems/vcpkg.cmake")

# NBA2K22.dll
project(NBA2K22)
add_library(NBA2K22 SHARED
    src/NBA2K22/DllMain.cpp
)

find_package(minhook CONFIG REQUIRED)
target_link_libraries(NBA2K22 PRIVATE minhook::minhook)

find_package(unofficial-mongoose CONFIG REQUIRED)
target_link_libraries(NBA2K22 PRIVATE unofficial::mongoose::mongoose)

# injector.exe
project(injector)
add_executable(injector src/injector.cpp)
