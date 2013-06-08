
SET(scanner_SOURCE
    dummy.cpp
)

SET(scanner_HEADERS
)

add_library (Go_Scanner ${scanner_SOURCE} ${scanner_HEADERS})

configure_target(Go_Scanner)