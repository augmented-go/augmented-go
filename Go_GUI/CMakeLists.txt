# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# The plugin path can either be passed directly to cmake via the -D switch
# or set as an environment variable
set(QT_PLUGIN_PATH ${QT_PLUGIN_PATH} $ENV{QT_PLUGIN_PATH})

# get list length (to check if we have at least one plugin path)
LIST(LENGTH QT_PLUGIN_PATH LEN)

# check if we have at keast one path
IF (LEN LESS 1)
  # let the user know that the plugin path couldn't be found
  set(QT_PLUGIN_PATH CACHE STRING "Qt5 plugin path" FORCE)

  message(FATAL_ERROR
    "QT_PLUGIN_PATH environment variable has to be set to proceed."
  )
ENDIF (LEN LESS 1)

# get the first entry of the plugin paths list
LIST(GET QT_PLUGIN_PATH 0 QT_PLUGIN_PATH)

# update cached plugin path
set(QT_PLUGIN_PATH ${QT_PLUGIN_PATH} CACHE STRING "Qt5 plugin path" FORCE)

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
qt5_wrap_ui(ui_NewGameDialog NewGameDialog.ui)
qt5_wrap_ui(ChangeScanRateDialog ChangeScanRateDialog.ui)


# writes the sha1 of the current git head into the Version.cpp file
# automatically reruns on build when the head has changed
# http://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake
include(${CMAKE_SOURCE_DIR}/cmake/GetGitRevisionDescription.cmake)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
if(GIT_SHA1 STREQUAL "GITDIR-NOTFOUND")
    message("No git directory found, GUI won't display SHA1 of the current commit.")
    set(GIT_SHA1 "unknown")
endif()

configure_file("Version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp" @ONLY)

SET(gui_SOURCE
    GUI.cpp
    VirtualView.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp
)

SET(gui_HEADERS
    GUI.hpp
    VirtualView.hpp
    CameraView.hpp
    NewGameDialog.hpp
    ChangeScanRateDialog.hpp
    Version.hpp
)

add_library (Go_GUI ${gui_SOURCE} ${gui_HEADERS} ${ui_GUI} ${ui_NewGameDialog} ${ChangeScanRateDialog})

# additional modules
add_backend_to_target(Go_GUI)
add_fuego_to_target(Go_GUI)

# Use Qt5 modules
qt5_use_modules(Go_GUI OpenGL 3D Widgets)

configure_target(Go_GUI)