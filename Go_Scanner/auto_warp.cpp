#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include <numeric>

namespace Go_Scanner {
    cv::Mat warpImage(cv::Mat img, cv::Point2f p0, cv::Point2f p1, cv::Point2f p2, cv::Point2f p3);

    const char* thresh_window = "Thresh";
    const char* morph_window = "Morph";

    using namespace cv;
    using namespace std;

    RNG rng(12345);

    Mat automatic_warp(const Mat& input)
    {
        // CONVERT TO HSV
        Mat imgHSV;
        cvtColor( input, imgHSV, CV_BGR2HSV );

        // SPLITTING CHANNELS
        vector<cv::Mat> v_channel;
        split(imgHSV, v_channel);          //split into three channels

        // SELECT CHANNEL FOR FURTHER PROCEEDING
        auto& source_channel = v_channel[1];

        // SMOOTHING IMAGE
        medianBlur(source_channel, source_channel, 3);

        // CANNY
        Mat edges;
        const int threshold = 70;
        Canny(source_channel, edges, threshold, threshold*2, 3);
        imshow( thresh_window, edges );

        // MORPHING FOR BETTER AREAS AND THUS CONTOURS
        const int morph_size = 4;
        Mat element = getStructuringElement(MORPH_RECT, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ));

        // Apply the specified morphology operation
        Mat morph;
        morphologyEx(edges, morph, MORPH_GRADIENT, element);
        imshow( morph_window, morph );

        // FINDING CONTOURS
        Mat morph_clone = morph.clone();
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(morph_clone, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));

        // APPROX. CONTOURS
        for (auto& contour : contours) {
            // approximating each contour to get a rectangle with 4 points!
            approxPolyDP(Mat(contour), contour, arcLength(Mat(contour), true)*0.02, true);
        }

        // CALCULATING BBOXES
        vector<Rect> bboxes;
        for (const auto& contour : contours)
            bboxes.push_back(boundingRect(contour));

        // CALCULATING MEAN BBOX AREA
        int bbox_area_sum = std::accumulate(begin(bboxes), end(bboxes), 0, [](int base, const Rect& r) { return base + r.area(); });
        int mean_area = bbox_area_sum/contours.size();

        // DELETING IMPROPER CONTOURS/BBOXES
        // that are smaller than the mean
        // or almost as big as the whole image
        // or don't have exactly 4 corner points after approximation ( == rectangle)
        assert(bboxes.size() == contours.size());

        const float edge_factor = .95f;
        const float max_width   = input.cols * edge_factor;
        const float max_height  = input.rows * edge_factor;

        auto cont_it = begin(contours);
        auto bbox_it = begin(bboxes);
        while (cont_it != end(contours) && bbox_it != end(bboxes)) {
            auto& contour = *cont_it;
            auto& bbox = *bbox_it;

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

        //assert(contours.size() > 0);
        if (contours.size() > 0) {
            // SELECTING SUITABLE CONTOUR

            // sort contours and bboxes by area
            // that means we're selecting biggest contour for further processing
            if (contours.size() > 1) {
                std::sort(begin(contours), end(contours),
                    [](const vector<Point>& cont1, const vector<Point>& cont2) { return contourArea(cont1) > contourArea(cont2); }
                );
                std::sort(begin(bboxes), end(bboxes),
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
            Point p0, p1, p2, p3;

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
                std::swap(p0, p1);

            // lower side
            p2 = lowers[0];
            p3 = lowers[1];
            if (p2.x > p3.x)
                std::swap(p2, p3);

            // WARPING THE IMAGE
            auto warped = warpImage(input, p0, p1, p2, p3);
            imshow("Warped", warped);
            return warped;
        }

        // DRAWING ALL LEFTOVER CONTOURS
        Mat drawing = input.clone();
        for (size_t i = 0; i < contours.size(); ++i) {
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
            drawContours(drawing, contours, (int)i, color, 2, 8, hierarchy, 0, Point());
        }

        // DRAWING BBOXES FOR ALL CONTOURS
        for(auto& rect : bboxes) {
            rectangle(drawing, rect.tl(), rect.br(), Scalar(255, 255, 255), 2, 8);
        }

        imshow("Contours", drawing);
        return Mat();
    }

}