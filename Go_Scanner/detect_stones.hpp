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

    /** 
    * @brief    Returns the size of a 45°(left headed) or 135°(right headed) line within a circle, 
    *           starting from any point within that circle mainly used to get the diameters of the stones. 
    *           This functin also provides the Midpoint of that line.
    *
    * @params   warpedImgGray   a image/matrix which is highly modified by morphologyEx with getStructuringElement(MORPH_ELLIPSE, Size(stone_diameter*0.38, stone_diameter*0.38)) and operation MORPH_OPEN.
    *           x               x value of the intersection
    *           y               y value of the intersection
    *           heading         enum lineheading LEFT or RIGHT
    *           midpointline    Midpoint of the line
    *
    * @returns  Size of a 45°(left headed) or 135°(right headed) line within a circle.
    */
    int getStoneDistanceAndMidpoint(const cv::Mat& warpedImgGray, int x, int y, lineheading heading, cv::Point2i& midpointLine);

    /**
    * @brief    Detect the black stones on the board
    *
    * @params   wapredImg               warpedImg of the camera image or picture
    *           intersectionsPoints     vector of the intersection points
    *           board_size              size of the go board
    *           to_board_coords         map that saves the pixel and board coordinates of the stones
    *           stone_diameter          approxiated stones_diameter
    *           stones                  the found stones
    *           paintedWarpedImg        a debug image
    */
    void detectBlackStones(cv::Mat& warpedImg, cv::vector<cv::Point2f> intersectionPoints, int board_size, std::map<cv::Point2f, SgPoint, lesserPoint2f> to_board_coords, float stone_diameter, SgPointSet& stones, cv::Mat& paintedWarpedImg);

    /**
    * @brief    Detect all stones on the board
    *
    * @params   wapredImg               warpedImg of the camera image or picture
    *           intersectionsPoints     vector of the intersection points
    *           to_board_coords         map that saves the pixel and board coordinates of the stones
    *           stone_diameter          approxiated stones_diameter
    *           stones                  the found stones
    *           paintedWarpedImg        a debug image
    */
    void detectAllStones(cv::Mat& warpedImg, cv::vector<cv::Point2f> intersectionPoints, std::map<cv::Point2f, SgPoint, lesserPoint2f> to_board_coords, SgPointSet& stones, float stone_diameter, cv::Mat& paintedWarpedImg);

    /**
    * @brief    Map pixel coordinates (intersection points) to board coordinates
    *           Begins at the left of the lowermost line and gradually moves the lines from left to right upwards.
    *           Preconditions:
    *           - The number of intersection points must match board_size * board_size!
    *           - Each line (or column if you want) must have exactly the same number of intersection points as the board_size value
    *           - It is required that the intersection points on a specific line all have almost equal y-values!
    *           Thats the decision criteria for selecting the points on a given line.
    *
    * @returns  map that contains the pixel and board coordinates of the stones
    */
    std::map<cv::Point2f, SgPoint, lesserPoint2f> getBoardCoordMapFor(std::vector<cv::Point2f> intersectionPoints, int board_size);

    /**
    * @brief    main function of detect_stones. it delivers the detected stones within setup. 
    *
    * @params   srcWarpedImg        warpedImg from webcam or picture
    *           paintedWarpedImg    a debug image
    *
    * @returns  true if the stone detection is possible
    */
    bool getStones(cv::Mat srcWarpedImg, cv::vector<cv::Point2f> intersectionPoints, GoSetup& setup, int& board_size, cv::Mat& paintedWarpedImg);

    /**
     * @brief       Maps pixel coordinates (intersection points) to board coordinates (SgPoint).
     * @param[in]   intersectionPoints  Found intersection points on the go board
     * @param[in]   board_size          The size of the go board
     * @returns     The map containing key-value pairs of every intersection point to an SgPoint.
     */
    std::map<cv::Point2f, SgPoint, lesserPoint2f> getBoardCoordMapFor(std::vector<cv::Point2f> intersectionPoints, int board_size);
}
