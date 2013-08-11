#include "detect_stones.hpp"


namespace Go_Scanner {

int getStoneDistanceAndMidpoint(const cv::Mat& warpedImgGray, int x, int y, lineheading heading, cv::Point2f& midpointLine)
{

    /**
    *    Returns the size of a 45°(left headed) or 135°(right headed) line within a circle, 
    *    starting from any point within that circle 
    *    mainly used to get the diameters of the stones. 
    *    This functin also provides the Midpoint of that line. 
    */

    int width = warpedImgGray.cols;
    int height = warpedImgGray.rows;

    int d1=0, d2=0, distance, xTmp1, yTmp1, xTmp2, yTmp2;

    if(heading == RIGHT)
    {    
        xTmp1 = x;
        yTmp1 = y;
        while (xTmp1 > 0 && xTmp1 < width
            && yTmp1 > 0 && yTmp1 < height
            && warpedImgGray.at<uchar>(yTmp1,xTmp1) < 50)
        {
            //runs to bottom left
            xTmp1 -= 1;    
            yTmp1 += 1; 
            d1++;
        }

        xTmp2 = x;
        yTmp2 = y;
        while(xTmp2 > 0 && xTmp2 < width
            && yTmp2 > 0 && yTmp2 < height
            && warpedImgGray.at<uchar>(yTmp2,xTmp2) < 50)
        {
            //runs to top right
            xTmp2 += 1;
            yTmp2 -= 1; 
            d2++;
        }
    }
    else //heading is left
    {

        xTmp1 = x;
        yTmp1 = y;
        while(xTmp1 > 0 && xTmp1 < width
            && yTmp1 > 0 && yTmp1 < height
            && warpedImgGray.at<uchar>(yTmp1,xTmp1) < 50)
        {
            //runs to top left
            xTmp1 -= 1;    
            yTmp1 -= 1; 
            d1++;
        }

        xTmp2 = x;
        yTmp2 = y;
        while(xTmp2 > 0 && xTmp2 < width
            && yTmp2 > 0 && yTmp2 < height
            && warpedImgGray.at<uchar>(yTmp2,xTmp2) < 50)
        {
            //runs to bottom right
            xTmp2 += 1;
            yTmp2 += 1; 
            d2++;
        }
    }


    //distance of our line
    distance = d1 + d2;

    // midpoint of our first help line
    midpointLine.x = (xTmp1 + xTmp2) / 2.0f;
    midpointLine.y = (yTmp1 + yTmp2) / 2.0f;

    return distance;
}

void detectBlackStones(cv::Mat& warpedImg, cv::vector<cv::Point2f> intersectionPoints, std::map<cv::Point2f, SgPoint, lesserPoint2f> to_board_coords, SgPointSet& stones, cv::Mat& paintedWarpedImg)
{
    //TODO: use black/white image. write function for it. 
    
    cv::Mat tmp, warpedImgGray;
    cv::cvtColor(warpedImg, tmp, CV_RGB2GRAY);

    cv::threshold(tmp, warpedImgGray, 85, 255, 0);

    cv::imshow("Image for detecting black stones", warpedImgGray);
    for(int i=0; i < intersectionPoints.size(); i++)
    {
        auto& intersection_point = intersectionPoints[i];

        int x = intersectionPoints[i].x;
        int y = intersectionPoints[i].y;
        int distance, diameter45, diameter125;

        /**
        * Let's check if this is a stone :). We'll read out the diameters at 45° and 125° if they 
        * are similar -> it's a stone. 
        */

        if (warpedImgGray.at<uchar>(y,x) < 20)
        {

            /**    
            *    first we produce a help line to get the diameters at 45° and 125°.
            *    therefore we use the intersectionpoints. 
            */ 
            cv::Point2f midpointLine;
            distance = getStoneDistanceAndMidpoint(warpedImgGray, x, y, LEFT, midpointLine);

            // Get the Diameter for 125 degree
            cv::Point2f midpoint125;
            diameter125 = getStoneDistanceAndMidpoint(warpedImgGray, midpointLine.x, midpointLine.y, RIGHT, midpoint125);

            // Get the Diameter for 45 degree
            cv::Point2f midpoint45;
            diameter45 = getStoneDistanceAndMidpoint(warpedImgGray, midpoint125.x, midpoint125.y, LEFT, midpoint45);

            if(diameter125+5 >= diameter45 && diameter125-5 <= diameter45 && diameter45 >= 10 && diameter125 >= 10  )
            {
                std::cout << "Black Stone ("<< x << ", "<< y << ")" << std::endl;

                stones.Include(to_board_coords[intersection_point]);

                cv::circle( paintedWarpedImg, 
                cv::Point(midpoint125.x, midpoint125.y),
                (diameter125/2), 
                cv::Scalar(0, 255, 0), 0, 8, 0);
            }
        }
    }
}

void detectAllStones(cv::Mat& warpedImg, cv::vector<cv::Point2f> intersectionPoints, std::map<cv::Point2f, SgPoint, lesserPoint2f> to_board_coords, SgPointSet& stones, float stone_diameter, cv::Mat& paintedWarpedImg)
{   
    using namespace cv;
    cv::Mat img;

    cv::cvtColor(warpedImg, img, CV_RGB2GRAY);
    cv::Canny(img, img, 100, 150, 3);

    // After canny we get a binary image, where all detected edges are white.
    // Dilate iterates over ervery pixel, and sets the pixel to the maximum pixel value within a circle around that pixel.
    // This leaves the areas black where no edges were detected and thus could contain a stone.
    Mat element_dilate = getStructuringElement(MORPH_ELLIPSE, Size(stone_diameter*0.8+0.5f, stone_diameter*0.8+0.5f));
    cv::dilate(img, img, element_dilate);
    cv::imshow("after dilating", img);

    // Only the intersection points (later accessed by img.at<uchar>(y,x)) are relevant.
    // Erode looks for the minimum pixel value (black) within a circle around every pixel.
    // Because black pixels represent possible stones, if there is any black pixel within 
    // a stone radius around an intersection then we have found a stone.
    Mat element_erode = getStructuringElement(MORPH_ELLIPSE, Size(stone_diameter*0.6+0.5f, stone_diameter*0.6+0.5f));
    // todo(mihi314) could be optimized by not using erode and instead only doing the erode operation at intersection points
    cv::erode(img, img, element_erode);

    cv::imshow("Image for detecting white stones", img);

    for(int i=0; i < intersectionPoints.size(); i++)
    {
        auto& intersection_point = intersectionPoints[i];

        int x = intersection_point.x + 0.5f;
        int y = intersection_point.y + 0.5f;
        int distance, diameter45, diameter125;

        /**
        * Let's check if this is a stone :). 
        */
        if (img.at<uchar>(y,x) < 50)
        {
            std::cout << "White Stone ("<< x << ", "<< y << ")" << std::endl;
            stones.Include(to_board_coords[intersection_point]);

            cv::circle( paintedWarpedImg, 
            cv::Point(intersectionPoints[i].x, intersectionPoints[i].y),
            (stone_diameter/2.0f), 
            cv::Scalar(238, 238, 176), 0, 8, 0);

        }
    }
}

// Map pixel coordinates (intersection points) to board coordinates
// Begins at the left of the lowermost line and gradually moves the lines from left to right upwards.
//
// Preconditions:
// - The number of intersection points must match board_size * board_size!
// - Each line (or column if you want) must have exactly the same number of intersection points as the board_size value
// - It is required that the intersection points on a specific line all have almost equal y-values!
//   Thats the decision criteria for selecting the points on a given line.
std::map<cv::Point2f, SgPoint, lesserPoint2f> getBoardCoordMapFor(std::vector<cv::Point2f> intersectionPoints, int board_size) 
{   
    const auto& num_lines = board_size;

    // Sort points by their descending y-values to begin at the lowermost point
    std::sort(std::begin(intersectionPoints), std::end(intersectionPoints), [](const cv::Point2f& left, const cv::Point2f& right) { return left.y > right.y; });

    // Walk through each line from left to right and map a board coordinate to each point,
    // iteratively looks at the next batch (num_lines points) of intersection points
    std::map<cv::Point2f, SgPoint, lesserPoint2f> to_board_coordinates;
    int line_idx = 1;
    auto begin_iter = begin(intersectionPoints);
    for (auto i = 0; i < num_lines; ++i, begin_iter += num_lines) {
        // Get the next num_lines points (these will be the points on the i-th line from the bottom)
        decltype(intersectionPoints) points_on_this_line;
        std::copy( begin_iter,
                   begin_iter+num_lines,
                   std::back_inserter(points_on_this_line));

        // Sort by ascending x values of the points
        std::sort(std::begin(points_on_this_line), std::end(points_on_this_line), [](const cv::Point2f& left, const cv::Point2f& right) { return left.x < right.x; });

        // Add an entry for each point on this line to the map
        int column_idx = 1;
        for (const auto& pt : points_on_this_line) {
            to_board_coordinates[pt] = SgPointUtil::Pt(column_idx++, line_idx);
        }

        ++line_idx;
    }

    return to_board_coordinates;
}

bool getStones(cv::Mat srcWarpedImg, cv::vector<cv::Point2f> intersectionPoints, GoSetup& setup, int& board_size,cv::Mat& paintedWarpedImg)
{
    // Calc the minimum distance between the first intersection point to all others
    // The minimum distance is approximately the diameter of a stone
    vector<float> distances;
    const auto& ref_point_min = *begin(intersectionPoints);
    for (const auto& point : intersectionPoints) {
        if (point != ref_point_min)
            distances.push_back(cv::norm(point - ref_point_min));
    }
    auto approx_stone_diameter = *std::min_element(begin(distances), end(distances));

    // Extract the board size
    // Board dimensions are quadratic, meaning width and height are the same so the sqrt(of the number of intersections) 
    // is the board size if it is a perfect square
    board_size = (int) floor( sqrt((double) intersectionPoints.size()) + 0.5 ); // The .5 is needed to round to the nearest integer
    if (board_size*board_size != intersectionPoints.size()) {
        // Got a false number of intersectionPoints
        // Stop the processing here
        return false;
    }

    std::cerr << "Board size: " << board_size << std::endl;

    // get map from intersection points to board coordinates (SgPoint)
    auto to_board_coords = getBoardCoordMapFor(intersectionPoints, board_size);

    // detect the stones!
    SgPointSet all_stones;
    detectAllStones(srcWarpedImg, intersectionPoints, to_board_coords, all_stones, approx_stone_diameter, paintedWarpedImg);

    SgPointSet black_stones;
    detectBlackStones(srcWarpedImg, intersectionPoints, to_board_coords, black_stones, paintedWarpedImg);
    setup.m_stones[SG_BLACK] = black_stones;
    setup.m_stones[SG_WHITE] = all_stones - black_stones;

    return true;
}


}