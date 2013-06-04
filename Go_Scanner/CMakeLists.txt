# just for user info
set(OPENCV_DIR $ENV{OPENCV_DIR} CACHE STRING "opencv path" FORCE)
IF(NOT OPENCV_DIR)
  message(SEND_ERROR "OPENCV_DIR environment variable has to be set to proceed.")
ENDIF()

# cmake automatically looks in the OPENCV_DIR envrionment variable for the OpenCVConfig.cmake file
find_package(OpenCV REQUIRED)

SET(scanner_SOURCE
    Scanner.cpp
)

SET(scanner_HEADERS
)

#add_library (Go_Scanner ${scanner_SOURCE} ${scanner_HEADERS})
add_executable(Go_Scanner ${scanner_SOURCE} ${scanner_HEADERS})
target_link_libraries(Go_Scanner ${OpenCV_LIBS})
