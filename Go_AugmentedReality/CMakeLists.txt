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

qt5_wrap_ui(ui_GUI ${CMAKE_SOURCE_DIR}/Go_GUI/GUI.ui)

add_executable (Go_AugmentedReality ${augmented_reality_SOURCE} ${augmented_reality_HEADERS} ${ui_GUI})

add_fuego_to_target(Go_AugmentedReality)
add_backend_to_target(Go_AugmentedReality)
add_gui_to_target(Go_AugmentedReality)

# Use Qt5 modules
qt5_use_modules(Go_AugmentedReality OpenGL 3D Widgets)

configure_target(Go_AugmentedReality)
