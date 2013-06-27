cmake_minimum_required (VERSION 2.8.9)

project(AugmentedGo CXX)

if (MSVC)
	add_definitions(/MP)
endif(MSVC)

# set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin) # executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin) # shared libs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib) # static libs

# This is for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/bin )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

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

# cmake automatically looks in the OPENCV_DIR envrionment variable for the OpenCVConfig.cmake file
find_package(OpenCV REQUIRED)

# our custom cmake functions/macros
include(cmake/add_fuego_to_target.cmake)
include(cmake/add_backend_to_target.cmake)
include(cmake/add_scanner_to_target.cmake)
include(cmake/add_gui_to_target.cmake)
include(cmake/add_opencv_to_target.cmake)
include(cmake/configure_target.cmake)

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

# copies res folder to output directory
file(COPY res DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

