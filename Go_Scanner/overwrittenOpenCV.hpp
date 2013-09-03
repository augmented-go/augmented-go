#pragma once

#include <opencv2/opencv.hpp>

namespace Go_Scanner {

    inline void imshow(const cv::string& winname, cv::Mat mat)
    {
        bool debug = false;

        if(debug)
            cv::imshow(winname, mat);

    };

}