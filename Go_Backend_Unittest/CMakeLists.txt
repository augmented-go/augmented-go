set(TARGETNAME Go_Backend_Unittest)

SET(backend_unittest_SOURCE
    backend_test.cpp
)

SET(backend_unittest_HEADERS
)


add_library(${TARGETNAME} SHARED ${backend_unittest_SOURCE} ${backend_unittest_HEADERS})

add_fuego_to_target(${TARGETNAME})
add_backend_to_target(${TARGETNAME})

# visual studio testing framework
set_target_properties(${TARGETNAME} PROPERTIES LINK_FLAGS "/LIBPATH:$(VCInstallDir)UnitTest\\lib")
set_target_properties(${TARGETNAME} PROPERTIES COMPILE_FLAGS "/I$(VCInstallDir)UnitTest\\include")

configure_target(${TARGETNAME})