
SET(scanner_test_SOURCE
    scanner_tests.cpp
)

SET(scanner_test_HEADERS
)

include_directories (${CMAKE_SOURCE_DIR}/Go_Scanner)
link_directories (${CMAKE_BINARY_DIR}/Go_Scanner)

add_library(Go_Scanner_Test SHARED ${scanner_test_SOURCE} ${scanner_test_HEADERS})
target_link_libraries (Go_Scanner_Test Go_Scanner)
target_link_libraries(Go_Scanner_Test ${OpenCV_LIBS})
add_fuego_to_target(Go_Scanner_Test)

# visual studio testing framework
set_target_properties(Go_Scanner_Test PROPERTIES LINK_FLAGS "/LIBPATH:$(VCInstallDir)UnitTest\\lib")
set_target_properties(Go_Scanner_Test PROPERTIES COMPILE_FLAGS "/I$(VCInstallDir)UnitTest\\include")

configure_target(Go_Scanner_Test)
