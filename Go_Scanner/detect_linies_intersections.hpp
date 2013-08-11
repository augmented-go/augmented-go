#pragma once

#include <opencv2/opencv.hpp>

namespace Go_Scanner {

enum lineType{HORIZONTAL, VERTICAL};
enum LineCoord{START_X=0, START_Y=1, END_X=2, END_Y=3};

float calcBetweenAngle(cv::Vec2f v1, cv::Vec2f v2);
void groupIntersectionLines(cv::vector<cv::Vec4i>& lines, cv::vector<cv::Vec4i>& horizontalLines, cv::vector<cv::Vec4i>& verticalLines);
cv::vector<cv::Vec4i> getBoardLines(cv::vector<cv::Vec4i>& lines, lineType type);
bool intersection(cv::Vec4i horizontalLine, cv::Vec4i verticalLine, cv::Point2f &r);
bool getBoardIntersections(cv::Mat warpedImg, int thresholdValue, cv::vector<cv::Point2f> &intersectionPoints, cv::Mat& paintedWarpedImg);

}
