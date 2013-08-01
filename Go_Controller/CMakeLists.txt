set(TARGETNAME Go_Controller)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the Qt libraries
find_package(Qt5OpenGL REQUIRED)
# WARNING: commented out the _qt5gui_find_extra_libs command in ${QT_PLUGIN_PATH}/../lib/cmake/Qt5Gui/Qt5GuiConfigExtras.cmake
#          to make it configure, don't know if that spawns any problems later. The project builds, so whatever...
find_package(Qt5Widgets REQUIRED)
find_package(Qt53D REQUIRED)

# for having access to compiled Qt Designer files (e.g. ui_GUI.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(augmented_reality_SOURCE
    BackendThread.cpp
    main.cpp
)

SET(augmented_reality_HEADERS
    BackendThread.hpp
)

wrap_gui_ui_files(ui_GUI ui_NewGameDialog)

add_executable (${TARGETNAME} ${augmented_reality_SOURCE} ${augmented_reality_HEADERS} ${ui_GUI} ${ui_NewGameDialog})

add_fuego_to_target(${TARGETNAME})
add_scanner_to_target(${TARGETNAME})
add_backend_to_target(${TARGETNAME})
add_opencv_to_target(${TARGETNAME})
add_gui_to_target(${TARGETNAME})

configure_target(${TARGETNAME})
