
# enable loading a debugging image if no webcam is installed
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_DEBUG_IMAGE")

SET(scanner_SOURCE
    Scanner.cpp
    detect_board.cpp
    detect_linies_intersections.cpp
    detect_stones.cpp
    overwrittenOpenCV.hpp
)

SET(scanner_HEADERS
    detect_board.hpp
    Scanner.hpp
    detect_linies_intersections.hpp
    detect_stones.hpp
)

add_library(Go_Scanner ${scanner_SOURCE} ${scanner_HEADERS})
add_opencv_to_target(Go_Scanner)
add_fuego_to_target(Go_Scanner)

configure_target(Go_Scanner)