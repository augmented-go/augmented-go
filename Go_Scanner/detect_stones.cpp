#include "detect_stones.hpp"
#include "overwrittenOpenCV.hpp"

namespace Go_Scanner {

using namespace cv;
using namespace std;

//  Returns the size of a 45°(left headed) or 135°(right headed) line within a circle, 
//  starting from any point within that circle 
//  mainly used to get the diameters of the stones. 
//  This functin also provides the Midpoint of that line. 
int getStoneDistanceAndMidpoint(const Mat& warpedImgGray, int x, int y, lineheading heading, Point2i& midpointLine)
{

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
    midpointLine.x = (xTmp1 + xTmp2) / 2;
    midpointLine.y = (yTmp1 + yTmp2) / 2;

    return distance;
}

void detectBlackStones(Mat& warpedImg, vector<Point2f> intersectionPoints, int board_size, map<Point2f, SgPoint, lesserPoint2f> to_board_coords, float stone_diameter, SgPointSet& stones, Mat& paintedWarpedImg)
{  
    Mat tmp, warpedImgGray, canny;
    cvtColor(warpedImg, tmp, CV_RGB2GRAY);
    imshow("warped black stones", tmp);

    threshold(tmp, warpedImgGray, 85, 255, 0);

    //we morph the thresholded image to get rid of light spots from reflacting light on the black stones. 
    int element_value = static_cast<int>(stone_diameter*0.38);
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(element_value, element_value));
    morphologyEx(warpedImgGray, warpedImgGray, MORPH_OPEN, element); // Apply the specified morphology operation

    //Draw circles around the intersections for easy detection of black stones
    for( size_t i = 0; i < intersectionPoints.size(); i++ )
    {
         Point center(cvRound(intersectionPoints[i].x), cvRound(intersectionPoints[i].y));
         int radius = cvRound(stone_diameter/2);
         // draw the circle outline
         circle(warpedImgGray, center, radius, Scalar(255), 2, 8, 0 );
    }


    imshow("Image for detecting black stones", warpedImgGray);


    for(size_t i=0; i < intersectionPoints.size(); i++)
    {
        auto& intersection_point = intersectionPoints[i];

        int x = cvRound(intersectionPoints[i].x);
        int y = cvRound(intersectionPoints[i].y);
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
            Point2i midpointLine;
            distance = getStoneDistanceAndMidpoint(warpedImgGray, x, y, LEFT, midpointLine);

            // Get the Diameter for 125 degree
            Point2i midpoint125;
            diameter125 = getStoneDistanceAndMidpoint(warpedImgGray, midpointLine.x, midpointLine.y, RIGHT, midpoint125);

            // Get the Diameter for 45 degree
            Point2i midpoint45;
            diameter45 = getStoneDistanceAndMidpoint(warpedImgGray, midpoint125.x, midpoint125.y, LEFT, midpoint45);


            //it's a stone if the diameters are similiar, not to small and not to big.

            int min_stone_size;
            int max_stone_size;

            //9x9
            if(board_size <= 9)
            {
                min_stone_size = 30;
                max_stone_size = 80;
            }
            //13x13
            else if (board_size <= 13)
            {
                min_stone_size = 20;
                max_stone_size = 60;
            }
            //19x19 ++
            else
            {
                min_stone_size = 12;
                max_stone_size = 50;
            }


            if(diameter125+5 >= diameter45 && diameter125-5 <= diameter45 && diameter45 >= min_stone_size && diameter125 >= min_stone_size  && diameter45 <= max_stone_size && diameter125 <= max_stone_size)
            {
                //cout << "Black Stone ("<< x << ", "<< y << ")" << endl;

                stones.Include(to_board_coords[intersection_point]);

                circle( paintedWarpedImg, 
                Point(midpoint125.x, midpoint125.y),
                (diameter125/2), 
                Scalar(0, 255, 0), 0, 8, 0);
            }
        }
    }
}

void detectAllStones(Mat& warpedImg, vector<Point2f> intersectionPoints, map<Point2f, SgPoint, lesserPoint2f> to_board_coords, SgPointSet& stones, float stone_diameter, Mat& paintedWarpedImg)
{   
    using namespace cv;
    Mat img;

    cvtColor(warpedImg, img, CV_RGB2GRAY);
    Canny(img, img, 100, 150, 3);

    // After canny we get a binary image, where all detected edges are white.
    // Dilate iterates over ervery pixel, and sets the pixel to the maximum pixel value within a circle around that pixel.
    // This leaves the areas black where no edges were detected and thus could contain a stone.
    int dilate_value = static_cast<int>(stone_diameter*0.8+0.5f);
    Mat element_dilate = getStructuringElement(MORPH_ELLIPSE, Size(dilate_value, dilate_value));
    dilate(img, img, element_dilate);
    imshow("after dilating", img);

    // Only the intersection points (later accessed by img.at<uchar>(y,x)) are relevant.
    // Erode looks for the minimum pixel value (black) within a circle around every pixel.
    // Because black pixels represent possible stones, if there is any black pixel within 
    // a stone radius around an intersection then we have found a stone.
    int erode_value = static_cast<int>(stone_diameter*0.6+0.5f);
    Mat element_erode = getStructuringElement(MORPH_ELLIPSE, Size(erode_value, erode_value));
    // todo(mihi314) could be optimized by not using erode and instead only doing the erode operation at intersection points
    erode(img, img, element_erode);

    imshow("Image for detecting all stones", img);

    for(size_t i=0; i < intersectionPoints.size(); i++)
    {
        auto& intersection_point = intersectionPoints[i];

        int x = cvRound(intersection_point.x);
        int y = cvRound(intersection_point.y);

        /**
        * Let's check if this is a stone :). 
        */
        if (img.at<uchar>(y,x) < 50)
        {
            //cout << "Stone ("<< x << ", "<< y << ")" << endl;
            stones.Include(to_board_coords[intersection_point]);

            circle( paintedWarpedImg, 
            Point(cvRound(intersectionPoints[i].x), cvRound(intersectionPoints[i].y)),
            cvRound(stone_diameter/2.0f), 
            Scalar(238, 238, 176), 0, 8, 0);

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
map<Point2f, SgPoint, lesserPoint2f> getBoardCoordMapFor(vector<Point2f> intersectionPoints, int board_size) 
{   
    const auto& num_lines = board_size;

    // Sort points by their descending y-values to begin at the lowermost point
    sort(begin(intersectionPoints), end(intersectionPoints), [](const Point2f& left, const Point2f& right) { return left.y > right.y; });

    // Walk through each line from left to right and map a board coordinate to each point,
    // iteratively looks at the next batch (num_lines points) of intersection points
    map<Point2f, SgPoint, lesserPoint2f> to_board_coordinates;
    int line_idx = 1;
    auto begin_iter = begin(intersectionPoints);
    for (auto i = 0; i < num_lines; ++i, begin_iter += num_lines) {
        // Get the next num_lines points (these will be the points on the i-th line from the bottom)
        decltype(intersectionPoints) points_on_this_line;
        copy( begin_iter,
                   begin_iter+num_lines,
                   back_inserter(points_on_this_line));

        // Sort by ascending x values of the points
        sort(begin(points_on_this_line), end(points_on_this_line), [](const Point2f& left, const Point2f& right) { return left.x < right.x; });

        // Add an entry for each point on this line to the map
        int column_idx = 1;
        for (const auto& pt : points_on_this_line) {
            to_board_coordinates[pt] = SgPointUtil::Pt(column_idx++, line_idx);
        }

        ++line_idx;
    }

    return to_board_coordinates;
}

bool getStones(Mat srcWarpedImg, vector<Point2f> intersectionPoints, GoSetup& setup, int& board_size,Mat& paintedWarpedImg)
{
    // Calc the minimum distance between the first intersection point to all others
    // The minimum distance is approximately the diameter of a stone
    vector<double> distances;
    const auto& ref_point_min = *begin(intersectionPoints);
    for (const auto& point : intersectionPoints) {
        if (point != ref_point_min)
            distances.push_back(norm(point - ref_point_min));
    }
    auto approx_stone_diameter = static_cast<float>(*min_element(begin(distances), end(distances)));

    // get map from intersection points to board coordinates (SgPoint)
    auto to_board_coords = getBoardCoordMapFor(intersectionPoints, board_size);

    // detect the stones!
    SgPointSet all_stones;
    detectAllStones(srcWarpedImg, intersectionPoints, to_board_coords, all_stones, approx_stone_diameter, paintedWarpedImg);

    SgPointSet black_stones;
    detectBlackStones(srcWarpedImg, intersectionPoints, board_size, to_board_coords, approx_stone_diameter, black_stones, paintedWarpedImg);
    setup.m_stones[SG_BLACK] = black_stones;
    setup.m_stones[SG_WHITE] = all_stones - black_stones;

    return true;
}


}