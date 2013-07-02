
SET(scanner_test_SOURCE
    main.cpp
)

SET(scanner_test_HEADERS
)

include_directories (${CMAKE_SOURCE_DIR}/Go_Scanner)
link_directories (${CMAKE_BINARY_DIR}/Go_Scanner)

add_executable(Go_Scanner_Test ${scanner_test_SOURCE} ${scanner_test_HEADERS})
target_link_libraries(Go_Scanner_Test Go_Scanner)
add_opencv_to_target(Go_Scanner_Test)
add_fuego_to_target(Go_Scanner_Test)
configure_target(Go_Scanner_Test)
