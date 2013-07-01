# links the gui library to the specified target
function(add_gui_to_target target)
    include_directories (${CMAKE_SOURCE_DIR}/Go_GUI)
    link_directories (${CMAKE_BINARY_DIR}/Go_GUI)
    target_link_libraries (${target} Go_GUI)
    
    # Use Qt5 modules
    qt5_use_modules(${target} OpenGL 3D Widgets)
endfunction()

function(wrap_gui_ui_files var)
    qt5_wrap_ui(ui_GUI ${CMAKE_SOURCE_DIR}/Go_GUI/GUI.ui)
    qt5_wrap_ui(ui_NewGameDialog ${CMAKE_SOURCE_DIR}/Go_GUI/NewGameDialog.ui)
    set(${var} ${ui_GUI} ${ui_NewGameDialog} PARENT_SCOPE)
endfunction()