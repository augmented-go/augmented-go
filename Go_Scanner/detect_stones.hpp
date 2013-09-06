// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once

#include <opencv2/opencv.hpp>

#include <GoSetup.h>
#include <SgSystem.h>
#include <SgPoint.h>

#include <vector>
#include <map>
#include <set>

namespace Go_Scanner {

    enum lineheading{LEFT, RIGHT};
    enum stoneColor{BLACK, WHITE};

    /**
     * @brief cv::Point2f comparison functor. Compares by y-value first and x-value second.\n
     *        Needed for example for a std::map and sorting.
     */
    struct lesserPoint2f {
        bool operator()(const cv::Point2f& left, const cv::Point2f& right) {
            if (left.y == right.y)
                return left.x < right.x;
            else
                return left.y < right.y;
        }
    };

    int getStoneDistanceAndMidpoint(const cv::Mat& warpedImgGray, int x, int y, lineheading heading, cv::Point2i& midpointLine);
    void detectBlackStones(cv::Mat& warpedImg, cv::vector<cv::Point2f> intersectionPoints, int board_size, std::map<cv::Point2f, SgPoint, lesserPoint2f> to_board_coords, float stone_diameter, SgPointSet& stones, cv::Mat& paintedWarpedImg);
    void detectAllStones(cv::Mat& warpedImg, cv::vector<cv::Point2f> intersectionPoints, std::map<cv::Point2f, SgPoint, lesserPoint2f> to_board_coords, SgPointSet& stones, float stone_diameter, cv::Mat& paintedWarpedImg);
    bool getStones(cv::Mat srcWarpedImg, cv::vector<cv::Point2f> intersectionPoints, GoSetup& setup, int& board_size, cv::Mat& paintedWarpedImg);

    /**
     * @brief       Maps pixel coordinates (intersection points) to board coordinates (SgPoint).
     * @param[in]   intersectionPoints  Found intersection points on the go board
     * @param[in]   board_size          The size of the go board
     * @returns     The map containing key-value pairs of every intersection point to an SgPoint.
     */
    std::map<cv::Point2f, SgPoint, lesserPoint2f> getBoardCoordMapFor(std::vector<cv::Point2f> intersectionPoints, int board_size);
}
