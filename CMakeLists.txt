cmake_minimum_required (VERSION 2.6)

project(AugmentedGo CXX)

# CMAKE_BINARY_DIR: The path to the top level of the build tree.
# CMAKE_SOURCE_DIR: The path to the top level of the source tree.

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
find_package( Boost 1.52 COMPONENTS date_time)

# boost not found variables aren't displayed anymore
# boost is found anyway, seems to be a bug or some remnant of older find_package versions
mark_as_advanced(Boost_DIR)
mark_as_advanced(Boost_LIB_DIAGNOSTIC_DEFINITIONS)

include_directories(${Boost_INCLUDE_DIRS})

# our custom cmake functions/macros
include(cmake/add_fuego_to_target.cmake)
include(cmake/add_backend_to_target.cmake)
include(cmake/add_gui_to_target.cmake)

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
