# links the GoBackend library to the specified target
function(add_game_to_target target)
    include_directories (${CMAKE_SOURCE_DIR}/Go_Game)
    link_directories (${CMAKE_BINARY_DIR}/Go_Game)
    target_link_libraries (${target} Go_Game)
endfunction()