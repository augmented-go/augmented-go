
SET(gui_SOURCE
    dummy.cpp
)

SET(gui_HEADERS
)

include_directories (${CMAKE_SOURCE_DIR}/Go_Backend)
link_directories (${CMAKE_BINARY_DIR}/Go_Backend)

add_library (Go_GUI ${gui_SOURCE} ${gui_HEADERS})
target_link_libraries (Go_GUI Go_Backend)
