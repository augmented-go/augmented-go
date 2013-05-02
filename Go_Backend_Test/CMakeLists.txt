
SET(backend_test_SOURCE
    some_tests.cpp
)

SET(backend_test_HEADERS
)

add_library(Go_Backend_Test SHARED ${backend_test_SOURCE} ${backend_test_HEADERS})

# visual studio testing framework
set_target_properties(Go_Backend_Test PROPERTIES LINK_FLAGS "/LIBPATH:$(VCInstallDir)UnitTest\\lib")
set_target_properties(Go_Backend_Test PROPERTIES COMPILE_FLAGS "/I$(VCInstallDir)UnitTest\\include")