#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <string>
#include <math.h>

cv::Mat img0, selectedImg, temp;
void releaseImg(cv::Mat a,int x,int y);
void showImage();
cv::Mat holdImg(int x,int y);

std::string windowName = "Augmented Go Cam";

bool debug = true;
bool showall = false;

/*

Rectangle Order: 
0-------1
|		|
|		|
3-------2

*/

//point coordinates
int boardCornerX[]={180, 643, 660, 157};	
int boardCornerY[]={80, 87, 530, 525};
int imagewidth = 768;
int imageheight = 576;


int point=-1;			//currently selected point
int nop=4;				//number of points

enum lineType{HORIZONTAL, VERTICAL};
enum stoneColor{BLACK, WHITE};
enum lineheading{LEFT, RIGHT};

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

		float boardfield = ((1.0f/18.0f) * sidesize)/3.5f;

		return distance < boardfield;
	}
};

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
			selectedImg=cv::Mat();
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
				cv::rectangle(temp, 
					cv::Point(x - 10, y - 10), 
					cv::Point(x + 10, y + 10), 
					//BGR not RGB!!
					cv::Scalar(0, 255, 0, 0), 2, 8, 0);

				//adjust the lines
				for(int i=0;i<nop;i++)
				{
					if(i!=point)
					{
						cv::line(temp,
							cv::Point(x, y), 
							cv::Point(boardCornerX[i] , boardCornerY[i] ), 
							cv::Scalar(0, 255, 0 ,0), 1,8,0);
					}
				}
				cv::imshow(windowName, temp); 
			}
			break;
		}
		temp = cv::Mat();
	}
}

//draws the lines and points while holding left mouse button down
cv::Mat holdImg(int x, int y)
{
	cv::Mat img = img0;

	//find what point is selected
	for(int i=0;i<nop;i++){
		if((x>=(boardCornerX[i]-2)) && (x<=(boardCornerX[i]+2 ))&& (y<=(boardCornerY[i]+2 ))&& (y<=(boardCornerY[i]+2 ))){
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
			cv::rectangle(img, 
				cv::Point(boardCornerX[j] - 1, boardCornerY[j] - 1), 
				cv::Point(boardCornerX[j] + 1, boardCornerY[j] + 1), 
				cv::Scalar(255, 0,  0, 0), 2, 8, 0);
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
					cv::line(img,
						cv::Point(boardCornerX[i] , boardCornerY[i] ), 
						cv::Point(boardCornerX[k] , boardCornerY[k] ), 
						cv::Scalar(255, 0, 0, 0), 1,8,0);

				}
			}
		}
	}
	return img;
}

//set new coordinates and redraw the scene if the left mouse button is released
void releaseImg(cv::Mat a, int x, int y)
{
	boardCornerX[point]=x;
	boardCornerY[point]=y;
	showImage();
}

//draw the scene components
void showImage()
{
	cv::Mat img1 = img0;

	//draw the points
	for(int j=0;j<nop;j++)
	{		
		img1 = img1.clone();
		cv::rectangle(img1, 
			cv::Point(boardCornerX[j] - 1, boardCornerY[j] - 1), 
			cv::Point(boardCornerX[j] + 1, boardCornerY[j] + 1), 
			cv::Scalar(255, 0,  0, 0), 2, 8, 0);


		//draw the lines
		for(int k=j+1;k<nop;k++)
		{
			img1 = img1.clone();
			cv::line(img1,
				cv::Point(boardCornerX[j] , boardCornerY[j] ), 
				cv::Point(boardCornerX[k] , boardCornerY[k] ), 
				cv::Scalar(255, 0,  0, 0), 1,8,0);
		}
	}
	cv::imshow(windowName, img1);
	img1 = cv::Mat();
}

cv::Mat warpImage()
{
	/*
	Rectangle Order: 
	0-------1
	|		|
	|		|
	2-------3
	*/
	cv::Point2f selCorners[4];		
	selCorners[0] = cv::Point2f(boardCornerX[0], boardCornerY[0]);
	selCorners[1] = cv::Point2f(boardCornerX[1], boardCornerY[1]);
	selCorners[2] = cv::Point2f(boardCornerX[3], boardCornerY[3]);
	selCorners[3] = cv::Point2f(boardCornerX[2], boardCornerY[2]);

	for (int i=0; i < 4; i ++)
		std::cout << boardCornerX[i] << " " << boardCornerY[i] << std::endl;

	cv::Point2f dstCorners[4]; 
	dstCorners[0] = cv::Point2f(0.0, 0.0);
	dstCorners[1] = cv::Point2f(imagewidth-1, 0.0);
	dstCorners[2] = cv::Point2f(0.0, imageheight-1);
	dstCorners[3] = cv::Point2f(imagewidth-1, imageheight-1);


	cv::Mat transformationMatrix;
	transformationMatrix = cv::getPerspectiveTransform(selCorners, dstCorners);

	cv::Mat warpedImg;
	cv::warpPerspective(img0, warpedImg, transformationMatrix, warpedImg.size(), 1, 0 ,0);

	return warpedImg;
}

cv::Mat sobelFiltering(cv::Mat warpedImgGray)
{
	//Sobel Filtering
	cv::Mat grad;
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	/// Gradient X
	cv::Sobel( warpedImgGray, grad_x, ddepth, 1, 0, 3, scale, delta, 0);
	/// Gradient Y
	cv::Sobel( warpedImgGray, grad_y, ddepth, 0, 1, 3, scale, delta, 0);

	cv::convertScaleAbs( grad_x, abs_grad_x );
	cv::convertScaleAbs( grad_y, abs_grad_y );
	cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

	return grad;
}

cv::Mat laplaceFiltering(cv::Mat warpedImgGray)
{
	cv::Mat abs_dst;
	cv::Mat dst;
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	cv::Laplacian( warpedImgGray, dst, ddepth, kernel_size, scale, delta, 0);
	convertScaleAbs(dst, abs_dst);

	return abs_dst;
}

float calcBetweenAngle(cv::Vec2f v1, cv::Vec2f v2)
{
	float angle;

	//calculate angle in radians between the 2 vectors
	angle = acosf((v1[0]*v2[0] + v1[1]*v2[1])/(sqrtf((v1[0]*v1[0])+(v1[1]*v1[1]))*sqrtf((v2[0]*v2[0])+(v2[1]*v2[1]))));
	
	//to degree
	angle = angle * (180.0f/3.14159f);

	return angle;
}

void getIntersectionLines(cv::vector<cv::Vec4i>& lines, cv::vector<cv::Vec4i>& horizontalLines, cv::vector<cv::Vec4i>& verticalLines)
{
	cv::Vec2f baseVector, lineVector;

	//baseVector
	baseVector[0] = imagewidth;
	baseVector[1] = 0;

	for (int i = 0; i < lines.size(); i++)
	{
		lineVector[0] = lines[i][2] - lines[i][0];
		lineVector[1] = lines[i][3] - lines[i][1];

		float angle = calcBetweenAngle(baseVector, lineVector);
		if(angle != 0.0f && angle != 90.0f)
			std::cout << angle << std::endl;

		//horizontal lines
		if(angle <= 1.0f && angle >= -1.0f)
		{
			cv::Vec4i v = lines[i];
			lines[i][0] = 0;
			lines[i][1] = ((float)v[1] - v[3]) / (v[0] - v[2]) * -v[0] + v[1]; 
			lines[i][2] = imagewidth; 
			lines[i][3] = ((float)v[1] - v[3]) / (v[0] - v[2]) * (imagewidth - v[2]) + v[3];
			
			horizontalLines.push_back(lines[i]);
		}
		//vertical lines
		else if(angle >= 89.7f && angle <= 90.2f)
		{
			cv::Vec4i v = lines[i];

			// y = m*x+n
			float m = ((float)v[1] - v[3]) / (v[0] - v[2] + 1);
			float n = v[1] - m * v[0];

			lines[i][0] = (-n)/m;
			lines[i][1] = 0; 
			lines[i][2] = (imageheight-n)/m;
			lines[i][3] = imageheight;

			verticalLines.push_back(lines[i]);
		}
		//other lines
		else
		{
			lines.erase(lines.begin()+i);
			std::cout << "This Line is deleted. Muahaha" << std::endl;
			//Delete that Line. Its a false line :)
		}
	}

}

cv::vector<cv::Vec4i> getBoardLines(cv::vector<cv::Vec4i>& lines, lineType type)
{
	int valueIndex, imagesizeIndex, zeroIndex, imagesize;

	if(type == VERTICAL)
	{
		valueIndex = 0;
		imagesizeIndex = 3;
		zeroIndex = 1;
		imagesize = imageheight;
	}
	else if (type == HORIZONTAL)
	{
		valueIndex = 1;
		imagesizeIndex = 2;
		zeroIndex = 0;
		imagesize = imagewidth;
	}

	//Horizontal Lines. X is 0 or the width of the picture. 
	cv::vector<int> lineStarts(lines.size());
	cv::vector<int> resultStarts(lines.size());

	for(size_t i=0; i<lines.size(); i++)
	{
		lineStarts[i] = lines[i][valueIndex];
	}

	// clustering of linedata
	PartitionOperator Oper(imagesize);
	int clusterNum = cv::partition<int, PartitionOperator>(lineStarts, resultStarts, Oper);


	// put the lines in there cluster
	cv::vector<cv::vector<int>> clusteredLineStarts(clusterNum);

	for(size_t i = 0; i < resultStarts.size(); i++)
	{
		int j = resultStarts[i];

		clusteredLineStarts[j].push_back(lineStarts[i]);
	}

	// middle the lines
	cv::vector<cv::Vec4i> middledLines;
	cv::Vec4i middle;

	for(int i=0; i < clusterNum; i++)
	{
		int numLines = clusteredLineStarts[i].size();
		int sum=0;

		for(int j=0; j < numLines; j++)
		{
			sum += clusteredLineStarts[i][j];
		}

		int mid = sum/numLines;

		middle[zeroIndex] = 0;
		middle[valueIndex] = mid;
		middle[imagesizeIndex] = imagesize;
		middle[valueIndex+2] = mid;

		middledLines.push_back(middle);
	}

	return middledLines;
}

bool intersection(cv::Vec4i horizontalLine, cv::Vec4i verticalLine, cv::Point2f &r)
{
	cv::Point2f o1;
	o1.x = horizontalLine[0];
	o1.y = horizontalLine[1];
	cv::Point2f p1;
	p1.x = horizontalLine[2];
	p1.y = horizontalLine[3];
	cv::Point2f o2;
	o2.x = verticalLine[0];
	o2.y = verticalLine[1];
	cv::Point2f p2;
	p2.x = verticalLine[2];
	p2.y = verticalLine[3];

    cv::Point2f x = o2 - o1;
    cv::Point2f d1 = p1 - o1;
    cv::Point2f d2 = p2 - o2;

    float cross = d1.x*d2.y - d1.y*d2.x;
    if (abs(cross) < /*EPS*/1e-8)
        return false;

    double t1 = (x.x * d2.y - x.y * d2.x)/cross;
    r = o1 + d1 * t1;
    return true;
}

bool getBoardIntersections(cv::Mat warpedImg, int thresholdValue, cv::vector<cv::Point2f> &intersectionPoints)
{
	/*TODO: Find the best results
		Canny + HoughLinesP gives the best results.

		Dont know if the filtering methods need to be used. 

		threshold will be implemented for bad images

		sort the lines! 

		what if there are not enough intersections points? 
		what if there are to many intersections points? 
	*/

	//Set the right Threshold for the image
	cv::Mat warpedImgGray, threshedImg, sobelImg, cannyImg;
	int const maxValue = 255;
	int thresholdType = 4;

	//reduce the noise
	cv::GaussianBlur(warpedImg, warpedImg , cv::Size(3,3), 0, 0, 0);
	cv::cvtColor(warpedImg, warpedImgGray, CV_RGB2GRAY);

	cv::Canny(warpedImgGray, cannyImg, 100, 200, 3);
	//sobelImg = sobelFiltering(cannyImg);
	cv::threshold(cannyImg, threshedImg, 255, maxValue, thresholdType );

	if(showall == true)
		cv::imshow("Threshed Image", threshedImg);

	cv::vector<cv::Vec4i> lines, horizontalLines, verticalLines;
	cv::HoughLinesP(threshedImg, lines, 1, CV_PI/180, 80, 60, 10 );

	getIntersectionLines(lines, horizontalLines, verticalLines);

	cv::vector<cv::Vec4i> newhorizontalLines = getBoardLines(horizontalLines, HORIZONTAL);
	cv::vector<cv::Vec4i> newverticalLines = getBoardLines(verticalLines, VERTICAL);

	//get the intersections
	for(size_t i=0; i < newhorizontalLines.size();  i++)
	{
		for(size_t j=0; j < newverticalLines.size(); j++)
		{
			cv::Point2f intersectedPoint;
			bool result = intersection(newhorizontalLines[i], newverticalLines[j], intersectedPoint);

			if(result == true)
				intersectionPoints.push_back(intersectedPoint);
		}
	}


	cv::vector<cv::Vec4i> newLines; 

	newLines.insert(newLines.begin(), newhorizontalLines.begin(), newhorizontalLines.end());
	newLines.insert(newLines.end(), newverticalLines.begin(), newverticalLines.end());

	//Draw the lines
    for( size_t i = 0; i < newLines.size(); i++ )
    {
        cv::line(warpedImg, cv::Point(newLines[i][0], newLines[i][1]),
            cv::Point(newLines[i][2], newLines[i][3]), cv::Scalar(0,0,255), 1, 8 );
    }

	//Draw the intersections
	for(size_t i= 0; i < intersectionPoints.size(); i++)
	{
		cv::rectangle( warpedImg, 
		cv::Point(intersectionPoints[i].x-1, intersectionPoints[i].y-1),
		cv::Point(intersectionPoints[i].x+1, intersectionPoints[i].y+1), 
		cv::Scalar(255, 0,  0, 0), 2, 8, 0);
	}


	cv::imshow("Intersections", warpedImg);
	
	return true;
}

bool detectBlobs(cv::Mat warpedImg)
{
	// not working!!! alternative: detectStones function


	cv::Mat warpedImgGray;
	//cv::cvtColor(warpedImg, warpedImgGray, CV_RGB2GRAY);

	// set up the parameters (check the defaults in opencv's code in blobdetector.cpp)
	cv::SimpleBlobDetector::Params params;
	//params.minDistBetweenBlobs = 50.0f;
	params.filterByInertia = false;
	params.filterByConvexity = false;
	//params.filterByColor = true;
	params.filterByCircularity = false;
	params.minCircularity = 15;
	params.maxCircularity = 80;
	//params.filterByArea = false;
	params.minThreshold = 50;
	params.maxThreshold = 90; 
	params.blobColor = 0;
	// ... any other params you don't want default value

	// set up and create the detector using the parameters
	cv::SimpleBlobDetector blob_detector(params);
	//blob_detector.create("SimpleBlob");

	// detect!
	cv::vector<cv::KeyPoint> keypoints;
	blob_detector.detect(warpedImg, keypoints);

	// extract the x y coordinates of the keypoints: 

	for (int i=0; i<keypoints.size(); i++){
		float X=keypoints[i].pt.x; 
		float Y=keypoints[i].pt.y;

		cv::rectangle( warpedImg, 
		cv::Point(X-1, Y-1),
		cv::Point(X+1, Y+1), 
		cv::Scalar(0, 0,  255, 0), 2, 8, 0);

	}

	cv::imshow("Wir sind so toll!", warpedImg);

	return true;
}

int getStoneDistanceAndMidpoint(cv::Mat& warpedImgGray, int x, int y, lineheading heading, cv::Point2f& midpointLine)
{
	/**
	*	Returns the size of a 45°(left headed) or 125°(right headed) line within a circle, 
	*	starting from any point within that circle 
	*	mainly used to get the diameters of the stones. 
	*	This functin also provides the Midpoint of that line. 
	*/

	int d1, d2, distance, xTmp1, yTmp1, xTmp2, yTmp2;

	if(heading == RIGHT)
	{	
		xTmp1 = x;
		yTmp1 = y;
		for(int j=1; warpedImgGray.at<uchar>(xTmp1,yTmp1) > 50 ; j++)
		{
			xTmp1 -= j;	
			yTmp1 += j; 
			d1 = j;
		}

		xTmp2 = x;
		yTmp2 = y;
		for(int j=1; warpedImgGray.at<uchar>(xTmp2,yTmp2) > 50 ; j++)
		{
			xTmp2 += j;
			xTmp2 -= j; 
			d2 = j;
		}
	}
	else //heading is left
	{

		xTmp1 = x;
		yTmp1 = y;
		for(int j=1; warpedImgGray.at<uchar>(xTmp1,yTmp1) > 50 ; j++)
		{
			xTmp1 -= j;	
			yTmp1 -= j; 
			d1 = j;
		}

		xTmp2 = x;
		yTmp2 = y;
		for(int j=1; warpedImgGray.at<uchar>(xTmp2,yTmp2) > 50 ; j++)
		{
			xTmp2 += j;
			xTmp2 += j; 
			d2 = j;
		}
	}


	//distance of our line
	distance = d1 + d2;

	// midpoint of our first help line
	midpointLine.x = (xTmp1 + xTmp2) / 2.0f;
	midpointLine.y = (yTmp1 + yTmp2) / 2.0f;

	return distance;
}

bool detectStones(cv::Mat warpedImg, cv::vector<cv::Point2f>& intersectionPoints)
{
	//TODO: use black/white image. write function for it. 

	cv::Mat warpedImgGray;
	cv::cvtColor(warpedImg, warpedImgGray, CV_RGB2GRAY);
	cv::threshold(warpedImgGray, warpedImgGray, 85, 255, 0);


	for(int i=0; i < intersectionPoints.size(); i++)
	{
		int x = intersectionPoints[i].x;
		int y = intersectionPoints[i].y;
		int distance, diameter45, diameter125;

		/**
		* Let's check if this is a stone :). We'll read out the diameters at 45° and 125° if they 
		* are similar -> it's a stone. 
		*/

		if (warpedImgGray.at<uchar>(x,y) < 50)
		{

			/**	
			*	first we produce a help line to get the diameters at 45° and 125°.
			*	therefore we use the intersectionpoints. 
			*/ 
			cv::Point2f midpointLine;
			distance = getStoneDistanceAndMidpoint(warpedImgGray, x, y, LEFT, midpointLine);

			// Get the Diameter for 125 degree
			cv::Point2f midpoint125;
			diameter125 = getStoneDistanceAndMidpoint(warpedImgGray, midpointLine.x, midpointLine.y, RIGHT, midpoint125);

			// Get the Diameter for 45 degree
			cv::Point2f midpoint45;
			diameter45 = getStoneDistanceAndMidpoint(warpedImgGray, midpoint125.x, midpoint125.y, RIGHT, midpoint45);

			if(diameter125+5 >= diameter45 && diameter125-5 <= diameter45)
			{
				std::cout << "a stone was detected" << std::endl;

				//save stone in data structure. this will be the returntype of this function
			}
			else
				std::cout << "no stone could be detected" << std::endl;

		}
		else
			std::cout << "No Stone on this position found" << std::endl;

	}
}


	cv::Mat Thresh;
	cv::Mat Thresh_res;
	int threshold_value = 100;
	int threshold_type = 0;
	int const max_value = 255;
	int const max_type = 4;
	int const max_BINARY_value = 255;
	
	char* window_name_thre = "Threshold Demo";
	char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
	char* trackbar_value = "Value";

void Threshold_Debug( int, void* )
{
  /* 0: Binary
     1: Binary Inverted
     2: Threshold Truncated
     3: Threshold to Zero
     4: Threshold to Zero Inverted
   */
	cv::Mat sobel;

	//sobel = sobelFiltering(Thresh);

	cv::Mat sub_mat = cv::Mat::ones(Thresh.size(), Thresh.type())*255;
	//subtract the original matrix by sub_mat to give the negative output new_image

	cv::subtract(sub_mat, Thresh, sobel);

	cv::threshold(sobel, Thresh_res, threshold_value, max_BINARY_value,threshold_type );

	cv::imshow( window_name_thre, Thresh_res );
}

int main(int argc, char** argv)
{
	//TODO: Get imagewidth and size automatically
	// convert the warped image just once to greyscale! 

	img0 = cv::imread("go_bilder/01.jpg");

	cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	cv::setMouseCallback(windowName, mouseHandler, NULL);
	cv::putText(img0, "Mark the Go board with the blue rectangle. Press any Key when finished.", 
				cvPoint(20,20), 1, 0.8, cv::Scalar(64, 64, 255), 1, CV_AA);
	showImage();
	cv::waitKey(0);
	
	cv::Mat warpedImg = warpImage();
	if(showall == true)
		cv::imshow("Warped Image", warpedImg);

	cv::Mat srcWarpedImg = warpedImg.clone();
	cv::vector<cv::Point2f> intersectionPoints;

	if (debug == true)
	{
		Thresh = warpedImg.clone();
		cv::cvtColor(Thresh, Thresh, CV_RGB2GRAY);

		cv::namedWindow( window_name_thre, CV_WINDOW_AUTOSIZE );

		cv::createTrackbar( trackbar_type,
                  window_name_thre, &threshold_type,
                  max_type, Threshold_Debug );


		cv::createTrackbar( trackbar_value,
                  window_name_thre, &threshold_value,
                  max_value, Threshold_Debug );
	}

	else
	{

		bool intersectionResult = getBoardIntersections(warpedImg, 255, intersectionPoints);

	}

	bool detectResult = detectStones(srcWarpedImg, intersectionPoints);
	//bool blobResult = detectBlobs(srcWarpedImg);

	cv::waitKey(0);
	cv::destroyWindow(windowName);

	img0 = cv::Mat();

	return 0;
}