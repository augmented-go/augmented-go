
SET(backend_SOURCE
    Backend.cpp
)

SET(backend_HEADERS
    Backend.hpp
)

add_library (Go_Backend ${backend_SOURCE} ${backend_HEADERS})
add_fuego_to_target(Go_Backend)