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

    // Vector helper
    template <typename T, typename R>
    vector<R> filter_unique(const vector<T>& input, std::function<R(T)> pred) {
        std::set<R> filtered_results;
        for (const T& element : input)
            filtered_results.insert(pred(element));
        return vector<R>(begin(filtered_results), end(filtered_results));
    }

    template <typename T>
    vector<T> filter_vector(const vector<T>& input, std::function<bool(T)> pred) {
        vector<T> filtered_results;
        std::copy_if(
            std::begin(input),
            std::end(input),
            std::back_inserter(filtered_results),
            pred
        );
        return filtered_results;
    }

    // cv::Point2f comparison helper
    // function objects for less comparison between two cv::Point2f's
    // needed for a std::map and sorting
    struct lesserPoint2f {
        bool operator()(const cv::Point2f& left, const cv::Point2f& right) {
            if (left.y == right.y)
                return left.x < right.x;
            else
                return left.y < right.y;
        }
    };
    struct lesserPoint2fx {
        bool operator()(const cv::Point2f& left, const cv::Point2f& right) { return left.x < right.x; }
    };


    int getStoneDistanceAndMidpoint(const cv::Mat& warpedImgGray, int x, int y, lineheading heading, cv::Point2f& midpointLine);
    void detectBlackStones(cv::Mat& warpedImg, cv::vector<cv::Point2f> intersectionPoints, int board_size, std::map<cv::Point2f, SgPoint, lesserPoint2f> to_board_coords, float stone_diameter, SgPointSet& stones, cv::Mat& paintedWarpedImg);
    void detectAllStones(cv::Mat& warpedImg, cv::vector<cv::Point2f> intersectionPoints, std::map<cv::Point2f, SgPoint, lesserPoint2f> to_board_coords, SgPointSet& stones, float stone_diameter, cv::Mat& paintedWarpedImg);
    std::map<cv::Point2f, SgPoint, lesserPoint2f> getBoardCoordMapFor(std::vector<cv::Point2f> intersectionPoints, int board_size);
    bool getStones(cv::Mat srcWarpedImg, cv::vector<cv::Point2f> intersectionPoints, GoSetup& setup, int& board_size, cv::Mat& paintedWarpedImg);


}
