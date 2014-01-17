# Set project as C++
project( openFrameworks CXX )

cmake_minimum_required( VERSION 2.8.11 )

add_definitions(-s DISABLE_EXCEPTION_CATCHING=0)

set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0 --js-library ${CMAKE_CURRENT_SOURCE_DIR}/scripts/em/browser.js -s VERBOSE=1")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -s DISABLE_EXCEPTION_CATCHING=0")

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall" )
set( CMAKE_EXECUTABLE_SUFFIX .html )

include_directories(
	libs/libc++/include
)

add_subdirectory( libs )
#add_subdirectory( addons )
add_subdirectory( examples )


