# links the gui library to the specified target
function(add_opencv_to_target target)
    target_link_libraries (${target} ${OpenCV_LIBS})
endfunction()