// Copyright (c) 2013 augmented-go team
// See the file LICENSE for full license and copying terms.
#pragma once

#include <opencv2/opencv.hpp>

#include <GoSetup.h>

#include <vector>
#include <map>
#include <set>

namespace Go_Scanner {
// functions from other translation units
void automatic_warp(const cv::Mat& input, cv::Point2f& p0, cv::Point2f& p1, cv::Point2f& p2, cv::Point2f& p3);

    bool scanner_main(const cv::Mat& camera_frame, GoSetup& setup, int& board_size);
    void ask_for_board_contour();
    void do_auto_board_detection();

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

}