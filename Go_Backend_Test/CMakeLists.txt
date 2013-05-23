
SET(backend_test_SOURCE
    backend_test.cpp
)

SET(backend_test_HEADERS
)

include_directories (${CMAKE_SOURCE_DIR}/Go_Backend)
link_directories (${CMAKE_BINARY_DIR}/Go_Backend)

add_library(Go_Backend_Test SHARED ${backend_test_SOURCE} ${backend_test_HEADERS})

target_link_libraries (Go_Backend_Test Go_Backend)
add_fuego_to_target(Go_Backend_Test)

# visual studio testing framework
set_target_properties(Go_Backend_Test PROPERTIES LINK_FLAGS "/LIBPATH:$(VCInstallDir)UnitTest\\lib")
set_target_properties(Go_Backend_Test PROPERTIES COMPILE_FLAGS "/I$(VCInstallDir)UnitTest\\include")