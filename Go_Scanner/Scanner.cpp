#include "Scanner.hpp"
#include "detect_board.hpp"
#include "detect_linies_intersections.hpp"
#include "detect_stones.hpp"
#include "overwrittenOpenCV.hpp"

#include <iostream>

#define ENABLE_DEBUG_IMAGE

namespace Go_Scanner {
using namespace cv;
using namespace std;

ScanResult Scanner::scanCamera(GoSetup& setup, int& board_size, Mat& out_image) {
    Mat frame;
    if (!readCameraFrame(frame)) {
#ifdef ENABLE_DEBUG_IMAGE
        frame = imread("res/textures/example.jpg", CV_LOAD_IMAGE_COLOR);
        if (frame.empty()) {
            std::cout << "Failed to load debug image from filesystem!" << std::endl;
            std::cout << "In " << __FUNCTION__ << std::endl;
            return ScanResult::NoCamera;
        }
#else
        return ScanResult::NoCamera;
#endif
    }

    auto success = scanner_main(frame, setup, board_size, _setDebugImg);
    out_image = frame;

    return success ? ScanResult::Success : ScanResult::Failed;
}

bool Scanner::readCameraFrame(Mat& frame) {
    if (!_camera.isOpened()) {
        // try opening camera 0
        // when one camera is connected, it will always have id 0
        bool opened = _camera.open(0);
        if (!opened)
            return false;

        // just to have _last_frame initialized with the correct size (needed for comparision)
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

void Scanner::selectBoardManually() {
    ask_for_board_contour();
}

void Scanner::selectBoardAutomatically() {
    do_auto_board_detection();
}

void Scanner::setDebugImage() {
    _setDebugImg = true;
}

void Scanner::setNormalImage() {
    _setDebugImg = false;
}

/**
 * @returns     true, if the user marked the board, and lines as well as stones could be found
 *              false, if the board wasn't marked before or if any of the operations fail (detecting stones, finding lines, etc.)
 */
bool scanner_main(const Mat& camera_frame, GoSetup& setup, int& board_size, bool& setDebugImg)
{
    // TODO: convert the warped image just once to greyscale! 
    Mat img;
    img = camera_frame; 

    if(!getWarpedImg(img))
    {
        return false;
    }

    imshow("Warped Image", img);

    Mat srcWarpedImg = img.clone();
    Mat paintedWarpedImg = img.clone();
    vector<Point2f> intersectionPoints;

    getBoardIntersections(img, 255, board_size, intersectionPoints, paintedWarpedImg);

    bool stoneResult = false;
    if (intersectionPoints.size() >= 4) {
        // Extract the board size
        // Board dimensions are quadratic, meaning width and height are the same so the sqrt(of the number of intersections) 
        // is the board size if it is a perfect square
        board_size = (int) floor( sqrt((double) intersectionPoints.size()) + 0.5 ); // The .5 is needed to round to the nearest integer
        if (board_size*board_size != intersectionPoints.size()) {
            // Got a false number of intersectionPoints
            // Stop the processing here
            return false;
        }

        stoneResult = getStones(srcWarpedImg, intersectionPoints, setup, board_size, paintedWarpedImg);
    }
    imshow("Detected Stones and Intersections", paintedWarpedImg);

    if(setDebugImg)
    {
       paintedWarpedImg.copyTo(camera_frame);
    }

    std::cout << ">>> Scanning finished <<<" << std::endl;

    return stoneResult;

}

}