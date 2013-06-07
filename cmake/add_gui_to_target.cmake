# links the gui library to the specified target
function(add_gui_to_target target)
    include_directories (${CMAKE_SOURCE_DIR}/Go_GUI)
    link_directories (${CMAKE_BINARY_DIR}/Go_GUI)
    target_link_libraries (${target} Go_GUI)
endfunction()