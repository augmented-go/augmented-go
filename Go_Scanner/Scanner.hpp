#pragma once

#include "opencv2/opencv.hpp"
#include "GoSetup.h"

#include <tuple>


namespace Go_Scanner {

class Scanner {
public:
    Scanner() {}

private:
    Scanner(const Scanner&);
    Scanner& operator=(const Scanner&);
        
public:
    /**
    * @brief        currently only sets out_image to the read camera image
    * @returns      true if a new image could be retrieved, false otherwise, may be changed to an enum or so
    */
    bool scanCamera(GoSetup& setup, int& board_size, cv::Mat& out_image);

    /**
    * @brief        Displays a window to let the user select the go board manually.
    *               This call blocks until the user is finished.
    */
    void selectBoardManually();

private:
    /**
    * @returns      true if a new image could be retrieved, false otherwise (camera disconnected)
    */
    bool readCameraFrame(cv::Mat& frame);

private:
    cv::VideoCapture _camera;
    cv::Mat _last_frame;
};

}