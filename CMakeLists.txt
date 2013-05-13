cmake_minimum_required (VERSION 2.6)

project(AugmentedGo CXX)

# CMAKE_BINARY_DIR: The path to the top level of the build tree.
# CMAKE_SOURCE_DIR: The path to the top level of the source tree.

find_package(Boost 1.52 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include(fuego/add_fuego_to_target.cmake)

add_subdirectory(fuego)
add_subdirectory(Go_AugmentedReality)
add_subdirectory(Go_Backend)
add_subdirectory(Go_GUI)
add_subdirectory(Go_Scanner)
add_subdirectory(Go_VirtualReality)


# the tests currently only run with visual studio 2012
if(MSVC11)
    add_subdirectory(Go_Backend_Test)
    add_subdirectory(Go_Scanner_Test)
endif(MSVC11)
