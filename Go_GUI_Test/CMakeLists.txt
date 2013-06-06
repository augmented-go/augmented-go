set (QT_PLUGIN_PATH $ENV{QT_PLUGIN_PATH} CACHE STRING "Qt5 plugin path" FORCE)

# check if qt5 root is set
IF (NOT QT_PLUGIN_PATH)
  message(FATAL_ERROR
    "QT_PLUGIN_PATH environment variable has to be set to proceed."
  )
ENDIF (NOT QT_PLUGIN_PATH)

# necessary for finding qt5 modules
LIST(APPEND CMAKE_PREFIX_PATH ${QT_PLUGIN_PATH}/..)

# Find the Qt libraries
find_package(Qt5OpenGL REQUIRED)
# WARNING: commented out the _qt5gui_find_extra_libs command in ${QT_PLUGIN_PATH}/../lib/cmake/Qt5Gui/Qt5GuiConfigExtras.cmake
#          to make it configure, don't know if that spawns any problems later. The project builds, so whatever...
find_package(Qt5Widgets REQUIRED)
find_package(Qt53D REQUIRED)

# compile Qt Designer files
qt5_wrap_ui(ui_GUI GUI.ui)


SET(gui_test_SOURCE
    some_tests.cpp
)

SET(gui_test_HEADERS
)

# Backend module
include_directories (${CMAKE_SOURCE_DIR}/Go_Backend ${CMAKE_SOURCE_DIR}/Go_GUI)
link_directories (${CMAKE_BINARY_DIR}/Go_Backend ${CMAKE_SOURCE_DIR}/Go_GUI)

add_executable(Go_GUI_Test ${gui_test_SOURCE} ${gui_test_HEADERS})

# Use Qt5 modules
qt5_use_modules(Go_GUI_Test OpenGL 3D Widgets)