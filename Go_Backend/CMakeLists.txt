
SET(backend_SOURCE
    Game.cpp
)

SET(backend_HEADERS
    Game.hpp
)

add_library (Go_Backend ${backend_SOURCE} ${backend_HEADERS})
add_fuego_to_target(Go_Backend)