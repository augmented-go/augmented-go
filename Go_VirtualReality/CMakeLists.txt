SET(virtual_reality_SOURCE
    dummy.cpp
)

SET(virtual_reality_HEADERS
)

# Make sure the compiler can find include files from our Go_Backend library.
include_directories (${CMAKE_SOURCE_DIR}/Go_Backend ${CMAKE_SOURCE_DIR}/Go_Gui)
# Make sure the linker can find the Go_Backend library once it is built.
link_directories (${CMAKE_BINARY_DIR}/Go_Backend ${CMAKE_BINARY_DIR}/Go_Gui)

add_executable (Go_VirtualReality ${virtual_reality_SOURCE} ${virtual_reality_HEADERS})
target_link_libraries (Go_VirtualReality Go_Backend) #Go_GUI) 