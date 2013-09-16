#pragma once

#include <opencv2/opencv.hpp>

namespace Go_Scanner {

enum lineType{HORIZONTAL, VERTICAL};
enum LineCoord{START_X=0, START_Y=1, END_X=2, END_Y=3};

/**
* @brief    calculate angle in radians between the 2 vectors to degree
*
* @params   v1  first vector
*           v2  second vector
*
* @returns  angle in float between those 2 vectors
*/
float calcBetweenAngle(cv::Vec2f v1, cv::Vec2f v2);

/**
* @brief    Group the Lines into vertical (angle -1.0 to 1.0) and horizontal (angle 88.0 to 92.0).
*           Stretch them depening on what orientation they have to reach the borders of the picture. 
*
* @params   lines               vector of all lines detected by houghlines
*           horizontalLines     all horizontal lines from lines      
*           verticalLines       all vertical lines from lines
*/
void groupIntersectionLines(cv::vector<cv::Vec4i>& lines, cv::vector<cv::Vec4i>& horizontalLines, cv::vector<cv::Vec4i>& verticalLines);

/**
* @brief    Get all lines that are found by houghline algorithmen (vertical or horizontal) and put them into clusters.
*           The result of each cluster is one middled line which is streched vertically or horizontally 
*           from the starting picture border to the facing one. 
*
* @params   lines       vector of lines
*           type        enum lineType HORIZONTAL or VERTICAL
*           board_size  size of the go board
*
* @returns  vector with our board lines
*/
cv::vector<cv::Vec4i> getBoardLines(cv::vector<cv::Vec4i>& lines, lineType type, int board_size);

/**
* @brief    Calculates the intersection between two lines 
*
* @params   horizontalLine  a horizontal line
*           verticalLine    a vertical line
*           r               the calculated point
*
* @returns  true if the two lines have a intersection
*/
bool intersection(cv::Vec4i horizontalLine, cv::Vec4i verticalLine, cv::Point2f &r);

/**
* @brief    This function delivers the board intersections. Its the "main" function and calls 
*           all other functions within detect_lines_intersection.hpp
*
* @params   warpedImg           The warpedImg from cam or picture
*           thresholdValue      no function yet
*           board_size          size of the go board
*           intersectionPoints  The intersectionspoints 
*           paintedWarpedImg    A debug image
*/
bool getBoardIntersections(cv::Mat warpedImg, int thresholdValue, int board_size, cv::vector<cv::Point2f> &intersectionPoints, cv::Mat& paintedWarpedImg);

/**
* @brief    Uses the midpoints of circles, averages them and creates a straight line from that data.
*
* @params   circles     circles detected by houghcircle detection
*           type        enum lineType HORIZONTAL or VERTICAL
*           board_size  size of the go board
*
* @returns  vector containing the calculated lines.
*/
cv::vector<cv::Vec4i> createLinefromValue(cv::vector<int> circles, lineType type, int board_size);

/**
* @brief    Using HoughCircle to detect some stones (not all can be found).
*           make those circle completely black to erase the white borders from the stones detected by canny. 
*           Now the image contains mostly straight white lines and houghline detection cannot find those small 
*           white stoneborders. Draws straight white lines with the help of createLinefromValue.
*
* @params   houghImg            a morphed canny image (normal canny image also possible)
*           createFakeLines     true if getBoardIntersections should also be called
*           board_size          size of the go board
*/
void getBetterDetectionImage(cv::Mat& houghImg, bool createFakeLines, int board_size);
}
