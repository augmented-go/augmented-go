#include "Scanner.hpp"
#include "detect_board.hpp"
#include "detect_linies_intersections.hpp"
#include "detect_stones.hpp"

#include <iostream>


namespace Go_Scanner {
using namespace cv;
using namespace std;

ScanResult Scanner::scanCamera(GoSetup& setup, int& board_size, cv::Mat& out_image) {
    Mat frame;
    if (!readCameraFrame(frame)) {
        // NOTICE: DEBUG STUFF!
        frame = imread("res/textures/example_pic.jpg", CV_LOAD_IMAGE_COLOR);

        if (frame.empty()) {
            std::cout << "Failed to load sample image from filesystem!" << std::endl;
            std::cout << "In " << __FUNCTION__ << std::endl;
            return ScanResult::Failed;
        }

        //out_image = frame;
        //return false;
    }

    auto result = scanner_main(frame, setup, board_size, _setDebugImg);
    
    out_image = frame;

    return result ? ScanResult::Success : ScanResult::Image_Only;
}

bool Scanner::readCameraFrame(cv::Mat& frame) {
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
bool scanner_main(const cv::Mat& camera_frame, GoSetup& setup, int& board_size, bool& setDebugImg)
{
    // TODO: convert the warped image just once to greyscale! 
    cv::Mat img;
    img = camera_frame; //cv::imread("go_bilder/01.jpg");

    if(!getWarpedImg(img))
    {
        return false;
    }

    cv::imshow("Warped Image", img);

    cv::Mat srcWarpedImg = img.clone();
    cv::Mat paintedWarpedImg = img.clone();
    cv::vector<cv::Point2f> intersectionPoints;

    getBoardIntersections(img, 255, intersectionPoints, paintedWarpedImg);

    bool stoneResult = false;
    if (intersectionPoints.size() >= 1) {
        stoneResult = getStones(srcWarpedImg, intersectionPoints, setup, board_size, paintedWarpedImg);
    }
    cv::imshow("Detected Stones and Intersections", paintedWarpedImg);

    if(setDebugImg)
    {
       paintedWarpedImg.copyTo(camera_frame);
    }


    return stoneResult;

}

}