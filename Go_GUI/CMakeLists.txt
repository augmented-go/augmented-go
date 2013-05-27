# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set (QT5_ROOT $ENV{QT5_ROOT} CACHE STRING "Qt5 root directory" FORCE)

# check if qt5 root is set
IF (NOT QT5_ROOT)
  message(FATAL_ERROR
    "QT5_ROOT environment variable has to be set to proceed."
  )
ENDIF (NOT QT5_ROOT)

# necessary for finding qt5 modules
set (CMAKE_PREFIX_PATH ${QT5_ROOT})

# Find the QtWidgets library
find_package(Qt5OpenGL REQUIRED)
find_package(Qt53D REQUIRED)
find_package(Qt5Widgets REQUIRED)


SET(gui_SOURCE
    dummy.cpp
)

SET(gui_HEADERS
)

add_executable (Go_GUI ${gui_SOURCE} ${gui_HEADERS})
#add_library (Go_GUI ${gui_SOURCE} ${gui_HEADERS})

# Use Qt5 modules
qt5_use_modules(Go_GUI OpenGL 3D Widgets)
