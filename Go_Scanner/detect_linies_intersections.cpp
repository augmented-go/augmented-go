#include "detect_linies_intersections.hpp"

#include <cmath>

namespace Go_Scanner {

using namespace cv;
using namespace std;

int imgwidth;
int imgheight;

struct PartitionOperator
{
    int sidesize;

    PartitionOperator(int size)
    {
        sidesize = size;
    }

    bool operator()(const int &a, const int &b) const 
    {
        float distance = a - b;
        distance = abs(distance);

        float boardfield = ((1.0f/18.0f) * sidesize)/3.0f;

        return distance < boardfield;
    }
};

float calcBetweenAngle(Vec2f v1, Vec2f v2)
{
    float angle;

    //calculate angle in radians between the 2 vectors
    angle = acosf((v1[0]*v2[0] + v1[1]*v2[1])/(sqrtf((v1[0]*v1[0])+(v1[1]*v1[1]))*sqrtf((v2[0]*v2[0])+(v2[1]*v2[1]))));
    
    //to degree
    angle = angle * (180.0f/3.14159f);

    return angle;
}

void groupIntersectionLines(vector<Vec4i>& lines, vector<Vec4i>& horizontalLines, vector<Vec4i>& verticalLines)
{
    Vec2f baseVector, lineVector;

    //baseVector
    baseVector[0] = imgwidth;
    baseVector[1] = 0;

    for (int i = 0; i < lines.size(); i++)
    {
        lineVector[0] = lines[i][2] - lines[i][0];
        lineVector[1] = lines[i][3] - lines[i][1];

        float angle = calcBetweenAngle(baseVector, lineVector);
        if(angle != 0.0f && angle != 90.0f)
            cout << angle << endl;

        //horizontal lines
        if(angle <= 1.0f && angle >= -1.0f)
        {
            Vec4i v = lines[i];
            lines[i][0] = 0;
            lines[i][1] = ((float)v[1] - v[3]) / (v[0] - v[2]) * -v[0] + v[1]; 
            lines[i][2] = imgwidth; 
            lines[i][3] = ((float)v[1] - v[3]) / (v[0] - v[2]) * (imgwidth - v[2]) + v[3];
            
            horizontalLines.push_back(lines[i]);
        }

        //vertical lines
        else if(angle >= 88.0f && angle <= 92.0f)
        {
            Vec4i v = lines[i];

            //Problem: For completely vertical lines, there is no m 
            // y = m*x+n

            if((v[0] - v[2]) != 0)
            {
                float m = (v[1] - v[3]) / (v[0] - v[2]);
                float n = v[1] - m * v[0];

                lines[i][0] = (-n)/m;               //x_start         
                lines[i][1] = 0;                    //y_start
                lines[i][2] = (imgheight-n)/m;    //x_end
                lines[i][3] = imgheight;          //y_end
            }
            else
            {
                lines[i][0] = v[0];                 //x_start         
                lines[i][1] = 0;                    //y_start
                lines[i][2] = v[2];                 //x_end
                lines[i][3] = imgheight;          //y_end
            }
            verticalLines.push_back(lines[i]);
        }
        //other lines
        else
        {
            lines.erase(lines.begin()+i);
            cout << "This Line is deleted. Muahaha" << endl;
            //Delete that Line. Its a false line :)
        }
    }

}

vector<Vec4i> getBoardLines(vector<Vec4i>& lines, lineType type)
{
    int valueIndex1, valueIndex2, imagesizeIndex, zeroIndex, imagesize;

    if(type == VERTICAL)
    {
        valueIndex1 = 0;            //0 = line[0] = x_start
        valueIndex2 = 2;            //2 = line[2] = x_end
        imagesizeIndex = 3;         //3 = line[3] = y_end
        zeroIndex = 1;              //1 = line[1] = y_start
        imagesize = imgheight;
    }
    else if (type == HORIZONTAL)
    {
        valueIndex1 = 1;
        valueIndex2 = 3;        
        imagesizeIndex = 2;
        zeroIndex = 0;
        imagesize = imgwidth;
    }

    //Put the Starting Points and Ending Points of a Line into the lineStarts and lineEnds Vector
    vector<int> lineStarts(lines.size());
    vector<int> lineEnds(lines.size());

    for(size_t i=0; i<lines.size(); i++)
    {
        lineStarts[i] = lines[i][valueIndex1];
        lineEnds[i] = lines[i][valueIndex2];
    }

    //clustering of linedata. Creating the Clusters with the PartitionOperator and store them into ClusterNum.
    vector<int> clusterNum(lines.size());
    PartitionOperator Oper(imagesize);
    int clusterSize = partition<int, PartitionOperator>(lineStarts, clusterNum, Oper);


    //put the lines in there cluster
    vector<vector<int>> clusteredLineStarts(clusterSize);
    vector<vector<int>> clusteredLineEnds(clusterSize);

    for(size_t i = 0; i < clusterNum.size(); i++)
    {
        int j = clusterNum[i];

        clusteredLineStarts[j].push_back(lineStarts[i]);
        clusteredLineEnds[j].push_back(lineEnds[i]);
    }

    // middle the lines
    vector<Vec4i> middledLines;
    Vec4i middle;

    for(int i=0; i < clusterSize; i++)
    {
        int numLines = clusteredLineStarts[i].size();
        int sumStarts=0;
        int sumEnds=0;

        for(int j=0; j < numLines; j++)
        {
            sumStarts += clusteredLineStarts[i][j];
            sumEnds += clusteredLineEnds[i][j];
        }

        int midStarts = sumStarts/numLines;
        int midEnds = sumEnds/numLines;

        middle[zeroIndex] = 0;
        middle[valueIndex1] = midStarts;
        middle[imagesizeIndex] = imagesize;
        middle[valueIndex2] = midEnds;

        middledLines.push_back(middle);
    }

    return middledLines;
}

bool intersection(Vec4i horizontalLine, Vec4i verticalLine, Point2f &r)
{
    Point2f o1;
    o1.x = horizontalLine[0];
    o1.y = horizontalLine[1];
    Point2f p1;
    p1.x = horizontalLine[2];
    p1.y = horizontalLine[3];
    Point2f o2;
    o2.x = verticalLine[0];
    o2.y = verticalLine[1];
    Point2f p2;
    p2.x = verticalLine[2];
    p2.y = verticalLine[3];

    Point2f x = o2 - o1;
    Point2f d1 = p1 - o1;
    Point2f d2 = p2 - o2;

    float cross = d1.x*d2.y - d1.y*d2.x;
    if (abs(cross) < /*EPS*/1e-8)
        return false;

    double t1 = (x.x * d2.y - x.y * d2.x)/cross;
    r = o1 + d1 * t1;
    if(r.x>=imgwidth || r.y>=imgheight)
        return false;

    return true;
}


bool getBoardIntersections(Mat warpedImg, int thresholdValue, vector<Point2f> &intersectionPoints, Mat& paintedWarpedImg)
{
    imgheight = warpedImg.rows;
    imgwidth = warpedImg.cols;

    //Set the right Threshold for the image
    Mat warpedImgGray, threshedImg, sobelImg, cannyImg;
    int const maxValue = 255;
    int thresholdType = 4;

    //reduce the noise
    //blur(warpedImg, warpedImg , Size(3,3));
    cvtColor(warpedImg, warpedImgGray, CV_RGB2GRAY);
    imshow("blur", warpedImgGray);

    Canny(warpedImgGray, cannyImg, 100, 150, 3);
    imshow("Canny", cannyImg);

    //sobelImg = sobelFiltering(cannyImg);
    threshold(cannyImg, threshedImg, 255, maxValue, thresholdType );

    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
    morphologyEx(threshedImg, threshedImg, MORPH_CLOSE, element); // Apply the specified morphology operation

    imshow("Threshed Image for HoughLinesP", threshedImg);

    vector<Vec4i> lines, horizontalLines, verticalLines;
    HoughLinesP(threshedImg, lines, 1, CV_PI/180, 80, 10, 5);

    Mat houghimage = warpedImg.clone();

    //Draw the lines
    /*
        Structure of line Vector:

        lines[i][0] = x_start         
        lines[i][1] = y_start
        lines[i][2] = x_end
        lines[i][3] = y_end      
    */
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line(houghimage, Point(lines[i][0], lines[i][1]),
            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 1, 8 );
    }

    imshow("HoughLines Image", houghimage);

    groupIntersectionLines(lines, horizontalLines, verticalLines);


    vector<Vec4i> newhorizontalLines;
    vector<Vec4i> newverticalLines;
    if (horizontalLines.size() != 0) {
        newhorizontalLines = getBoardLines(horizontalLines, HORIZONTAL);
    }
    if (verticalLines.size() != 0) {
        newverticalLines = getBoardLines(verticalLines, VERTICAL);
    }


    //get the intersections
    for(size_t i=0; i < newhorizontalLines.size();  i++)
    {
        for(size_t j=0; j < newverticalLines.size(); j++)
        {
            Point2f intersectedPoint;
            bool result = intersection(newhorizontalLines[i], newverticalLines[j], intersectedPoint);

            if(result == true)
                intersectionPoints.push_back(intersectedPoint);
        }
    }

    vector<Vec4i> newLines; 

    newLines.insert(newLines.begin(), newhorizontalLines.begin(), newhorizontalLines.end());
    newLines.insert(newLines.end(), newverticalLines.begin(), newverticalLines.end());

    //Draw the lines
    for( size_t i = 0; i < newLines.size(); i++ )
    {
        line(paintedWarpedImg, Point(newLines[i][0], newLines[i][1]),
            Point(newLines[i][2], newLines[i][3]), Scalar(0,0,255), 1, 8 );
    }

    //Draw the intersections
    for(size_t i= 0; i < intersectionPoints.size(); i++)
    {
        rectangle(paintedWarpedImg, 
        Point(intersectionPoints[i].x-1, intersectionPoints[i].y-1),
        Point(intersectionPoints[i].x+1, intersectionPoints[i].y+1), 
        Scalar(255, 0,  0, 0), 2, 8, 0);
    }


    //imshow("Intersections", warpedImg);
    
    return true;
}

}