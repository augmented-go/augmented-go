
SET(scanner_test_SOURCE
    some_tests.cpp
)

SET(scanner_test_HEADERS
)

add_library(Go_Scanner_Test SHARED ${scanner_test_SOURCE} ${scanner_test_HEADERS})

# visual studio testing framework
set_target_properties(Go_Scanner_Test PROPERTIES LINK_FLAGS "/LIBPATH:$(VCInstallDir)UnitTest\\lib")
set_target_properties(Go_Scanner_Test PROPERTIES COMPILE_FLAGS "/I$(VCInstallDir)UnitTest\\include")

configure_target(Go_Scanner_Test)