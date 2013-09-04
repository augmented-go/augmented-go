#include "detect_board.hpp"

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include <numeric>

namespace Go_Scanner {

    const char* thresh_window = "Thresh";
    const char* morph_window = "Morph";
    bool asked_for_board_contour = false;

    using namespace cv;
    using namespace std;


    //point coordinates
    int boardCornerX[]={80, 443, 460, 157};    
    int boardCornerY[]={80, 87, 430, 325};
    Mat img0, selectedImg, temp;
    string windowName = "Manual Selection Window";


     /**
     * @brief   Calls the manual board detection and shows the result in a new window.
     */
    void ask_for_board_contour() {
        namedWindow(windowName, CV_WINDOW_AUTOSIZE);
        setMouseCallback(windowName, mouseHandler, NULL);
        putText(img0, "Mark the Go board with the blue rectangle. Press any Key when finished.", 
                    cvPoint(20,20), 1, 0.8, Scalar(64, 64, 255), 1, CV_AA);
        showImage();
        waitKey(0);
        destroyWindow(windowName);

        asked_for_board_contour = true;
    }

    /**
     * @brief   Calls the automatic board detection and shows the result in a new window.
     *          Prints an error to the console if the automatic detection couldn't find anything.
     */
    void do_auto_board_detection() {
        Point2f p0, p1, p2, p3;
        automatic_warp(img0, p0, p1, p2, p3);

        // automatic_warp failed and board wasn't found
        // if one of the points wasn't set
        if (p0 == Point2f()) {
            cout << "!!ERROR >> Failed to automatically detect the go board!" << endl;
            return;
        }

        // HACK: adjust the coordinates a bit to snap them closer to the board grid
        const auto gap = .5f;
        p0 += Point2f(gap, gap);
        p1 += Point2f(-gap, gap);
        p2 += Point2f(-gap, -gap);
        p3 += Point2f(gap, -gap);

        boardCornerX[0] = cvRound(p0.x);
        boardCornerX[1] = cvRound(p1.x);
        boardCornerX[2] = cvRound(p2.x);
        boardCornerX[3] = cvRound(p3.x);

        boardCornerY[0] = cvRound(p0.y);
        boardCornerY[1] = cvRound(p1.y);
        boardCornerY[2] = cvRound(p2.y);
        boardCornerY[3] = cvRound(p3.y);

        namedWindow(windowName, CV_WINDOW_AUTOSIZE);
        putText(img0, "Result of automatically detecting the Go board.", 
                    cvPoint(20,20), 1, 0.8, Scalar(64, 64, 255), 1, CV_AA);
        showImage();
        waitKey(0);
        destroyWindow(windowName);

        asked_for_board_contour = true;
    }

    Mat warpImage(Mat img, Point2f p0, Point2f p1, Point2f p2, Point2f p3)
    {
        /*
        Rectangle Order: 
        0-------1
        |        |
        |        |
        2-------3
        */
        Point2f selCorners[4];        
        selCorners[0] = p0;
        selCorners[1] = p1;
        selCorners[2] = p2;
        selCorners[3] = p3;

        Point2f dstCorners[4]; 
        dstCorners[0] = Point2f(0.0, 0.0);
        dstCorners[1] = Point2f((float)img.cols, 0.0);
        dstCorners[2] = Point2f(0.0, (float)img.rows);
        dstCorners[3] = Point2f((float)img.cols, (float)img.rows);

        Mat transformationMatrix;
        transformationMatrix = getPerspectiveTransform(selCorners, dstCorners);

        Mat warpedImg;
        warpPerspective(img, warpedImg, transformationMatrix, warpedImg.size(), 1, 0 ,0);

        return warpedImg;
    }


    RNG rng(12345);

    // Tries to automatically detect the corner points of the go board
    // This function simply returns without modifying p0, .., p3 if the board couldn't be found
    void automatic_warp(const Mat& input, Point2f& p0, Point2f& p1, Point2f& p2, Point2f& p3)
    {
        // CONVERT TO HSV
        Mat imgHSV;
        cvtColor( input, imgHSV, CV_BGR2HSV );

        // SPLITTING CHANNELS
        vector<Mat> v_channel;
        split(imgHSV, v_channel);          //split into three channels

        // SELECT CHANNEL FOR FURTHER PROCEEDING: SATURATION
        auto& source_channel = v_channel[1];

        // SMOOTHING IMAGE
        medianBlur(source_channel, source_channel, 3);

        // BINARY THRESH THE IMAGE
        const auto threshold = cv::threshold(source_channel, source_channel, 0, 255, THRESH_BINARY | THRESH_OTSU);

        // FINDING CONTOURS
        Mat clone = source_channel.clone();
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(clone, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));

        // STOP IF THERE AREN'T ANY CONTOURS
        if (contours.size() == 0)
            return;

        // APPROX. CONTOURS TO RECTANGLES
        for (auto& contour : contours) {
            // approximating each contour to get a rectangle with 4 points!
            approxPolyDP(Mat(contour), contour, arcLength(Mat(contour), true)*0.02, true);
        }

        // CALCULATING BBOXES
        vector<Rect> bboxes;
        for (const auto& contour : contours)
            bboxes.push_back(boundingRect(contour));

        // CALCULATING MEAN BBOX AREA
        int bbox_area_sum = accumulate(begin(bboxes), end(bboxes), 0, [](int base, const Rect& r) { return base + r.area(); });
        int mean_area = bbox_area_sum/contours.size();

        // DELETING IMPROPER CONTOURS/BBOXES
        // that are smaller than the mean area
        // or almost as big as the whole image
        // or don't have exactly 4 corner points after approximation ( == rectangle)
        assert(bboxes.size() == contours.size());

        const float edge_factor = .95f;
        const float max_width   = input.cols * edge_factor;
        const float max_height  = input.rows * edge_factor;

        // DEBUG: DRAWING ALL LEFTOVER CONTOURS
        Mat drawing = input.clone();
        for (size_t i = 0; i < contours.size(); ++i) {
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
            drawContours(drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point());
        }

        // DEBUG: DRAWING BBOXES FOR ALL CONTOURS
        for(auto& rect : bboxes) {
            rectangle(drawing, rect.tl(), rect.br(), Scalar(255, 255, 255), 2, 8);
        }

        auto cont_it = begin(contours);
        auto bbox_it = begin(bboxes);
        while (cont_it != end(contours) && bbox_it != end(bboxes)) {
            auto& contour = *cont_it;
            auto& bbox = *bbox_it;
            const auto ar = bbox.area();

            // delete bboxes and its corresponding contour if the bbox is almost as large as the window
            if (bbox.width >= max_width && bbox.height >= max_height) {
                cont_it = contours.erase(cont_it);
                bbox_it = bboxes.erase(bbox_it);
            }
            // or if the bbox area is smaller than the mean area
            else if (bbox.area() <= mean_area) {
                cont_it = contours.erase(cont_it);
                bbox_it = bboxes.erase(bbox_it);
            }
            else if (contour.size() != 4) {
                cont_it = contours.erase(cont_it);
                bbox_it = bboxes.erase(bbox_it);
            }
            else {
                ++cont_it;
                ++bbox_it;
            }
        }

        // STOP IF WE DON'T HAVE ANY CONTOURS LEFT -> NO BOARD FOUND
        if (contours.size() == 0)
            return;

        // SELECTING SUITABLE CONTOUR
        // sort contours and bboxes by area
        // that means we're selecting biggest contour for further processing
        if (contours.size() > 1) {
            sort(begin(contours), end(contours),
                [](const vector<Point>& cont1, const vector<Point>& cont2) { return contourArea(cont1) > contourArea(cont2); }
            );
            sort(begin(bboxes), end(bboxes),
                [](const Rect& r1, const Rect& r2) { return r1.area() > r2.area(); }
            );
        }

        auto board_contour = contours.front();
        auto board_bbox    = bboxes.front();

        // GETTING CORNER POINTS OF CONTOUR
        assert(board_contour.size() == 4); // the contour should have left only 4 points after approximating

        // Rectangle Order for warping: 
        // 0--------1
        // |        |
        // |        |
        // 2--------3

        auto center = Point(board_bbox.x + board_bbox.width/2, board_bbox.y + board_bbox.height/2);

        // splitting points in upper and lower half
        vector<Point> uppers, lowers;
        for (auto& point : board_contour) {
            if (point.y < center.y)
                uppers.emplace_back(point);
            else
                lowers.emplace_back(point);
        }

        // deciding which point is left/right
        assert(uppers.size() == 2);
        assert(lowers.size() == 2);

        // upper side
        p0 = uppers[0];
        p1 = uppers[1];
        if (p0.x > p1.x)
            swap(p0, p1);

        // lower side
        p2 = lowers[0];
        p3 = lowers[1];
        if (p2.x < p3.x)
            swap(p2, p3);
    }


//Manual detection

    /*

    Rectangle Order: 
    0-------1
    |        |
    |        |
    3-------2

    */

    int point=-1;            //currently selected point
    int nop=4;               //number of points


    void mouseHandler(int event, int x, int y, int flags, void *param)
    {

        switch(event) 
        {
        case CV_EVENT_LBUTTONDOWN:        
            selectedImg = holdImg(x, y);
            break;

        case CV_EVENT_LBUTTONUP:    
            if((selectedImg.empty()!= true)&& point!=-1)
            {
                releaseImg(selectedImg,x,y);
                selectedImg=Mat();
            }
            break;

        //Draws the lines while navigating with the mouse
        case CV_EVENT_MOUSEMOVE:
            /* draw a rectangle*/
            if(point!=-1)
            {
                if(selectedImg.empty()!= true)
                {
                    temp = selectedImg.clone();
                    rectangle(temp, 
                        Point(x - 10, y - 10), 
                        Point(x + 10, y + 10), 
                        //BGR not RGB!!
                        Scalar(0, 255, 0, 0), 2, 8, 0);

                    //adjust the lines
                    for(int i=0;i<nop;i++)
                    {
                        if(i!=point)
                        {
                            line(temp,
                                Point(x, y), 
                                Point(boardCornerX[i] , boardCornerY[i] ), 
                                Scalar(0, 255, 0 ,0), 1,8,0);
                        }
                    }
                    imshow(windowName, temp); 
                }
                break;
            }
            temp = Mat();
        }
    }

    //draws the lines and points while holding left mouse button down
    Mat holdImg(int x, int y)
    {
        Mat img = img0;

        int radius = 4;
        //find what point is selected
        for(int i=0;i<nop;i++){
            if((x>=(boardCornerX[i]-radius)) && (x<=(boardCornerX[i]+radius ))&& (y<=(boardCornerY[i]+radius ))&& (y<=(boardCornerY[i]+radius ))){
                point=i;
                break;
            }

        }
        //draw points
        for(int j=0;j<nop;j++)
        {
            //if this is not the selected point
            if(j!=point)
            {
                img = img.clone();
                rectangle(img, 
                    Point(boardCornerX[j] - 1, boardCornerY[j] - 1), 
                    Point(boardCornerX[j] + 1, boardCornerY[j] + 1), 
                    Scalar(255, 0,  0, 0), 2, 8, 0);
            }
        }

        //draw lines
        for(int i=0;i<nop;i++)
        {
            if(i!=point)
            {
                for(int k=i+1;k<nop;k++)
                {
                    if(k!=point)
                    {
                        img = img.clone();
                        line(img,
                            Point(boardCornerX[i] , boardCornerY[i] ), 
                            Point(boardCornerX[k] , boardCornerY[k] ), 
                            Scalar(255, 0, 0, 0), 1,8,0);

                    }
                }
            }
        }
        return img;
    }

    //set new coordinates and redraw the scene if the left mouse button is released
    void releaseImg(Mat a, int x, int y)
    {
        boardCornerX[point]=x;
        boardCornerY[point]=y;
        showImage();
    }

    /**
     * @brief   Shows the selected points (through manual or automatic board detection) on a clone of the img0 in a new window.
     */
    void showImage()
    {
        Mat img1 = img0.clone();

        //draw the points
        for(int j=0;j<nop;j++)
        {        
            rectangle(img1, 
                Point(boardCornerX[j] - 1, boardCornerY[j] - 1), 
                Point(boardCornerX[j] + 1, boardCornerY[j] + 1), 
                Scalar(255, 0,  0, 0), 2, 8, 0);


            //draw the lines
            for(int k=j+1;k<nop;k++)
            {
                line(img1,
                    Point(boardCornerX[j] , boardCornerY[j] ), 
                    Point(boardCornerX[k] , boardCornerY[k] ), 
                    Scalar(255, 0,  0, 0), 1,8,0);
            }
        }
        imshow(windowName, img1);
    }


    bool getWarpedImg(Mat& warpedImg)
    {
        img0 = warpedImg.clone();

        // only process the image if the user  selected the board with "ask_for_board_contour" or "do_auto_board_detection" once.
        // this is triggered through the GUI (and the Scanners selectBoardManually() and selectBoardAutomatically() methods)
        if (!asked_for_board_contour) {
            return false;
        }
    
            warpedImg = warpImage(img0,
            Point2i(boardCornerX[0], boardCornerY[0]),
            Point2i(boardCornerX[1], boardCornerY[1]),
            Point2i(boardCornerX[3], boardCornerY[3]),
            Point2i(boardCornerX[2], boardCornerY[2]));       

        return true;

    }
}