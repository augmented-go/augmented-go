find_package(OpenCV REQUIRED)

SET(scanner_SOURCE
    Scanner.cpp
)

SET(scanner_HEADERS
)

#add_library (Go_Scanner ${scanner_SOURCE} ${scanner_HEADERS})
add_executable (Go_Scanner ${scanner_SOURCE} ${scanner_HEADERS})

IF(WIN32)
  IF(MSVC)
    # TODO: link against right libs depending on build configuration
    target_link_libraries( Go_Scanner ${OPENCV2_DEBUG_LIBS})
  ELSE(MSVC)
    message(error "Compiler and platform other than MSVC on Windows are not yet supported!")
  ENDIF(MSVC)
ENDIF(WIN32)