#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>

namespace Go_Scanner {

    /**
    * @brief        warpes an image 
    *
    * @param		img		the image given by webcam or the debug image
    *				p0		Left top point of warping area
    *				p1		right top point of warping area
    *               p2		Left bottom point of warping area
    *				p3		right bottom point of warping area
    *
    * @returns      a warped image
    */
    cv::Mat warpImage(cv::Mat img, cv::Point2f p0, cv::Point2f p1, cv::Point2f p2, cv::Point2f p3);

    /**
    * @brief        Tries to automatically detect the corner points of the go board
    *			    This function simply returns without modifying p0, .., p3 if the board couldn't be found
    *
    * @param	    input	matrix with webcam or the debug image
    *				p0		Left top point of warping area
    *				p1		right top point of warping area
    *				p2		Left bottom point of warping area
    *			    p3		right bottom point of warping area
    *
    */
    void automatic_warp(const cv::Mat& input, cv::Point2f& p0, cv::Point2f& p1, cv::Point2f& p2, cv::Point2f& p3);

    /**
    * @brief   Shows the selected points (through manual or automatic board detection) on a clone of the img0 in a new window.
    */
    void showImage();

    //Only functions for the mousehandler of opencv. used for manual selection.
    void releaseImg(cv::Mat a,int x,int y);
    void showImage(int* board_corner_X, int* board_corner_Y);
    cv::Mat holdImg(int x,int y);
    void mouseHandler(int event, int x, int y, int flags, void *param);

    /**
     * @brief   Calls the automatic board detection and shows the result in a new window.
     *          Prints an error to the console if the automatic detection couldn't find anything.
     */
    void do_auto_board_detection();

    /**
    * @brief        Calls the manual board detection and shows the result in a new window.
    */
    void ask_for_board_contour();

    /**
    * @brief        only process the image if the user selected the board with "ask_for_board_contour" or "do_auto_board_detection" once.
    *               this is triggered through the GUI (and the Scanners selectBoardManually() and selectBoardAutomatically() methods.
    *
    * @returns      true or false
    */
    bool getWarpedImg(cv::Mat& warpedImg);


}