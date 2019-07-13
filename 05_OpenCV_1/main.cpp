#include <iostream>
#include "opencv/cv.h"
#include <opencv2/opencv.hpp>

int main()
{
	std::string text = "My cat";
	int baseline = 0;
	cv::Size text_size = cv::getTextSize(text, cv::FONT_HERSHEY_TRIPLEX, 1.7, 2, &baseline);
	cv::Point p;
	cv::Scalar clr(0, 0, 0);
	cv::Mat frame, image_res;
	cv::VideoCapture cap(0);
	if (!cap.isOpened())
	{
		std::cout << "ERROR!" << std::endl;
	}
	else
	{
		while (true)
		{
			cap.read(frame);
			cv::cvtColor(frame, image_res, cv::COLOR_BGR2GRAY);
			p.x = 640 - text_size.width - 5;
			p.y = text_size.height + 5;
			cv::putText(image_res, text, p, cv::FONT_HERSHEY_TRIPLEX, 1.7, clr, 2, cv::LINE_8);
			cv::imshow("camera", image_res);
			int x = cv::waitKey(1);
			if (x == 32)
			{
				cv::imwrite(PATH_TO_IMG, image_res);
			}
			else if (x == 27)
			{
				break;
			}
		}
	}
	return 0;
}