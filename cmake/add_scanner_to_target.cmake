# links the gui library to the specified target
function(add_scanner_to_target target)
    include_directories (${CMAKE_SOURCE_DIR}/Go_Scanner)
    link_directories (${CMAKE_BINARY_DIR}/Go_Scanner)
    target_link_libraries (${target} Go_Scanner)
endfunction()