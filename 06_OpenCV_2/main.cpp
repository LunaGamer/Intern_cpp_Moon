#include <iostream>
#include "opencv/cv.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>


int main()
{
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::Mat image = cv::imread(PATH_TO_IMG_SRC);
	cv::Mat img_buf, image_res, warp;
	cv::Point p;
	float angle = 0;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	p.x = image.cols / 2;
	p.y = image.rows / 2;
	cv::imshow("Source_image", image);
	cv::waitKey(0);
	cv::cvtColor(image, img_buf, cv::COLOR_BGR2GRAY);
	cv::blur(img_buf, img_buf, cv::Size(3, 3));
	cv::threshold(img_buf, img_buf, 240, 255, 0);
	cv::morphologyEx(img_buf, img_buf, cv::MORPH_CLOSE, kernel);
	cv::findContours(img_buf, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	cv::RotatedRect rotateR = cv::minAreaRect(contours[1]);
	cv::Point2f rectP[4];
	rotateR.points(rectP);
	angle = rotateR.angle+90;
	warp = cv::getRotationMatrix2D(p, angle, 1);
	warpAffine(image, image_res, warp, image.size());
	cv::imshow("Result", image_res);
	std::cout << "Save Result? y - yes" << std::endl;
	int x = cv::waitKey(0);
	if (x == 121)
	{
		cv::imwrite(PATH_TO_IMG_DST, image_res);
	}
	return 0;
}