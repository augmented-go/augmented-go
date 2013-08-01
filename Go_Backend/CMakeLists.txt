set(TARGETNAME Go_Backend)

SET(backend_SOURCE
    Game.cpp
)

SET(backend_HEADERS
    Game.hpp
)

add_library (${TARGETNAME} ${backend_SOURCE} ${backend_HEADERS})
add_fuego_to_target(${TARGETNAME})

configure_target(${TARGETNAME})
