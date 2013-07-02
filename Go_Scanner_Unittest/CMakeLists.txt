
SET(scanner_unittest_SOURCE
    scanner_tests.cpp
)

SET(scanner_unittest_HEADERS
)

include_directories (${CMAKE_SOURCE_DIR}/Go_Scanner)
link_directories (${CMAKE_BINARY_DIR}/Go_Scanner)

add_library(Go_Scanner_Unittest SHARED ${scanner_unittest_SOURCE} ${scanner_unittest_HEADERS})
target_link_libraries(Go_Scanner_Unittest Go_Scanner)
add_opencv_to_target(Go_Scanner_Unittest)
add_fuego_to_target(Go_Scanner_Unittest)

# visual studio testing framework
set_target_properties(Go_Scanner_Unittest PROPERTIES LINK_FLAGS "/LIBPATH:$(VCInstallDir)UnitTest\\lib")
set_target_properties(Go_Scanner_Unittest PROPERTIES COMPILE_FLAGS "/I$(VCInstallDir)UnitTest\\include")

configure_target(Go_Scanner_Unittest)
