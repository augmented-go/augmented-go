#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <math.h>

cv::Mat img0, selectedImg, temp;
void releaseImg(cv::Mat a,int x,int y);
void showImage();
cv::Mat holdImg(int x,int y);

std::string windowName = "Augmented Go Cam";

bool debug = false;

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
int imagewidth, imageheight;

int point=-1;			//currently selected point
int nop=4;				//number of points


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

	//Draws the lines while simple navigating with the mouse
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

//set new cordinates and redraw the scene if the left mouse button is released
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
	/*
		TODO: some crooked lines will be painted... dont know why.
	*/

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
		else if(angle >= 89.5f && angle <= 90.5f)
		{
			cv::Vec4i v = lines[i];

			// y = m*x+n
			float m = ((float)v[1] - v[3]) / (v[0] - v[2] + 1);
			float n = v[1] - m * v[0];

			lines[i][0] = -n/m;
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

cv::Mat getBoardIntersections(cv::Mat warpedImg, int thresholdValue)
{
	/*TODO: Find the best results
		Canny + HoughLinesP gives the best results.

		Dont know if the filtering methods need to be used. 

		threshold will be implemented for bad images
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

	cv::vector<cv::Vec4i> lines, horizontal_lines, vertival_lines;
	cv::HoughLinesP(threshedImg, lines, 1, CV_PI/180, 80, 10, 10 );

	getIntersectionLines(lines, horizontal_lines, vertival_lines);

	//Draw the lines
    for( size_t i = 0; i < lines.size(); i++ )
    {
        cv::line(warpedImg, cv::Point(lines[i][0], lines[i][1]),
            cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0,0,255), 1, 8 );
    }

	cv::imshow("Hough Lines", warpedImg);
	
	return threshedImg;
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

	sobel = sobelFiltering(Thresh);

	cv::threshold(sobel, Thresh_res, threshold_value, max_BINARY_value,threshold_type );

	cv::imshow( window_name_thre, Thresh_res );
}

int main(int argc, char** argv)
{
	//TODO: Get imagewidth and size automatically
	img0 = cv::imread("go_bilder/01.jpg");

	imagewidth = 768;
	imageheight = 576;

	cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	cv::setMouseCallback(windowName, mouseHandler, NULL);
	cv::putText(img0, "Mark the Go board with the blue rectangle. Press any Key when finished.", 
				cvPoint(20,20), 1, 0.8, cv::Scalar(64, 64, 255), 1, CV_AA);
	showImage();
	cv::waitKey(0);
	
	cv::Mat warpedImg = warpImage();
	cv::imshow("Warped Image", warpedImg);


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
		cv::Mat threshedImg = getBoardIntersections(warpedImg, 255);
		cv::imshow("Threshed Image", threshedImg);
	}

	cv::waitKey(0);
	cv::destroyWindow(windowName);

	img0 = cv::Mat();

	return 0;
}