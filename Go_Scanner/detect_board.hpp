#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>

namespace Go_Scanner {

    cv::Mat warpImage(cv::Mat img, cv::Point2f p0, cv::Point2f p1, cv::Point2f p2, cv::Point2f p3);
    void automatic_warp(const cv::Mat& input, cv::Point2f& p0, cv::Point2f& p1, cv::Point2f& p2, cv::Point2f& p3);
    void releaseImg(cv::Mat a,int x,int y);
    void showImage();
    cv::Mat holdImg(int x,int y);
    void do_auto_board_detection();
    void ask_for_board_contour();
    bool getWarpedImg(cv::Mat& warpedImg);
    void mouseHandler(int event, int x, int y, int flags, void *param);

}