# just for user info
set(OPENCV_DIR ${OPENCV_DIR} CACHE STRING "opencv path" FORCE)
IF(NOT OPENCV_DIR)
  message(SEND_ERROR "OPENCV_DIR environment variable has to be set to proceed.")
ENDIF()


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