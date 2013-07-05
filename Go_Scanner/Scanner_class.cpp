#include "Scanner.hpp"
#include <iostream>


namespace Go_Scanner {
using namespace cv;
using namespace std;
using SgPointUtil::Pt;

bool Scanner::scanCamera(GoSetup& setup, int& board_size, cv::Mat& out_image) {
    Mat frame;
    if (!readCameraFrame(frame)) {
        // NOTICE: DEBUG STUFF!
        frame = imread("res/textures/example_pic.jpg", CV_LOAD_IMAGE_COLOR);
        //return true;
    }

    // do stuff with frame
    
    // NOTICE: DEBUG STUFF!
    board_size = 19;
    
    setup.AddBlack(Pt(2, 2));
    setup.AddBlack(Pt(2, 3));
    setup.AddBlack(Pt(2, 4));
    setup.AddWhite(Pt(3, 2));
    setup.AddWhite(Pt(3, 3));
    //

    out_image = frame;
    return true;
}

bool Scanner::readCameraFrame(cv::Mat& frame) {
    if (!_camera.isOpened()) {
        // try opening camera 0
        // when one camera is connected, it will always have id 0
        bool opened = _camera.open(0);
        if (!opened)
            return false;

        // just to have _last_frame initialized with the correct size (needed for compaision)
        _camera.read(_last_frame);
        // successive calls to read give the same data pointer
        // setting the matrix to zero would then also set it to zero for the frame read below
        _last_frame = _last_frame.clone();
        _last_frame *= 0;
    }

    bool success = _camera.read(frame);

    bool same_as_last_frame = false;
    if (success) {
        // success doesn't seem to indicate whether the camera has been disconnected or not,
        // but read continuously returns the same image if it has been disconnected

        // mask has 3 channels (rgb), but countNonZero only works on 1 channel matrices
        // .reshape reinterprets the data as one channel
        // it's a O(1) operation, as the new matrix points to the same data
        Mat mask = _last_frame != frame;
        same_as_last_frame = countNonZero(mask.reshape(1)) == 0;

        _last_frame = frame.clone();
    }

    if (!success || same_as_last_frame) {
        // camera has been disconnected
        _camera.release();
        return false;
    }

    return true;
}

}