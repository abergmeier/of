# Set project as C++
project( openFrameworks CXX )

cmake_minimum_required( VERSION 2.8.11 )

add_definitions(-s DISABLE_EXCEPTION_CATCHING=0)

set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0")

add_subdirectory( libs )
#add_subdirectory( addons )
#add_subdirectory( examples )


