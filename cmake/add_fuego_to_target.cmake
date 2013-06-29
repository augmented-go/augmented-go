# links the fuego library to the specified target
function(add_fuego_to_target target)
    include_directories (${CMAKE_SOURCE_DIR}/fuego/go)
    include_directories (${CMAKE_SOURCE_DIR}/fuego/gtpengine)
    include_directories (${CMAKE_SOURCE_DIR}/fuego/smartgame)
    link_directories (${CMAKE_BINARY_DIR}/fuego)
    target_link_libraries (${target} fuego)
endfunction()