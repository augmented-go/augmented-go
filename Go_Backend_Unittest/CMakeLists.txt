
SET(backend_unittest_SOURCE
    backend_test.cpp
)

SET(backend_unittest_HEADERS
)

include_directories (${CMAKE_SOURCE_DIR}/Go_Backend)
link_directories (${CMAKE_BINARY_DIR}/Go_Backend)

add_library(Go_Backend_Unittest SHARED ${backend_unittest_SOURCE} ${backend_unittest_HEADERS})

target_link_libraries (Go_Backend_Unittest Go_Backend)
add_fuego_to_target(Go_Backend_Unittest)

# visual studio testing framework
set_target_properties(Go_Backend_Unittest PROPERTIES LINK_FLAGS "/LIBPATH:$(VCInstallDir)UnitTest\\lib")
set_target_properties(Go_Backend_Unittest PROPERTIES COMPILE_FLAGS "/I$(VCInstallDir)UnitTest\\include")

configure_target(Go_Backend_Unittest)