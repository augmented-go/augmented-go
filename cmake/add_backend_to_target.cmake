# links the GoBackend library to the specified target
function(add_backend_to_target target)
    include_directories (${CMAKE_SOURCE_DIR}/Go_Backend)
    link_directories (${CMAKE_BINARY_DIR}/Go_Backend)
    target_link_libraries (${target} Go_Backend)
endfunction()