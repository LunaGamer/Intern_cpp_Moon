#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

using namespace dlib;
using namespace std;

/*void d(int i)
{
	std::cout << i << std::endl;
}*/

int main()
{
	cv::VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cerr << "Unable to connect to camera" << endl;
		return 1;
	}

	// Load face detection and pose estimation models.
	frontal_face_detector detector = get_frontal_face_detector();
	shape_predictor pose_model;
	deserialize(PATH_TO_FACE_LANDMARKS) >> pose_model;


	// 3D model points.
	std::vector<cv::Point3d> model_points;
	model_points.push_back(cv::Point3d(0.0f, 0.0f, 0.0f));               // Nose tip
	model_points.push_back(cv::Point3d(0.0f, -330.0f, -65.0f));          // Chin
	model_points.push_back(cv::Point3d(-225.0f, 170.0f, -135.0f));       // Left eye left corner
	model_points.push_back(cv::Point3d(225.0f, 170.0f, -135.0f));        // Right eye right corner
	model_points.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f));      // Left Mouth corner
	model_points.push_back(cv::Point3d(150.0f, -150.0f, -125.0f));       // Right mouth corner

	cv::Mat im;
	image_window win;
	// Grab and process frames until the main window is closed by the user.
	while (!win.is_closed())
	{
		// Grab a frame
		cap.read(im);

		// Turn OpenCV's Mat into something dlib can deal with.  Note that this just
		// wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
		// long as temp is valid.  Also don't do anything to temp that would cause it
		// to reallocate the memory which stores the image as that will make cimg
		// contain dangling pointers.  This basically means you shouldn't modify temp
		// while using cimg.
		cv_image<bgr_pixel> cimg(im);

		// Detect faces 
		std::vector<rectangle> faces = detector(cimg);
		// Find the pose of each face.
		std::vector<full_object_detection> shapes;
		for (unsigned long i = 0; i < faces.size(); ++i)
			shapes.push_back(pose_model(cimg, faces[i]));
		
		if (shapes.size() > 0)
		{
			// 2D image points. If you change the image, you need to change vector
			cv::Point2d nose_tip = cv::Point(shapes[0].part(30).x(), shapes[0].part(30).y());
			cv::Point2d chin = cv::Point(shapes[0].part(8).x(), shapes[0].part(8).y());
			cv::Point2d left_eye = cv::Point(shapes[0].part(45).x(), shapes[0].part(45).y());
			cv::Point2d right_eye = cv::Point(shapes[0].part(36).x(), shapes[0].part(36).y());
			cv::Point2d left_mouse = cv::Point(shapes[0].part(54).x(), shapes[0].part(54).y());
			cv::Point2d right_mouse = cv::Point(shapes[0].part(48).x(), shapes[0].part(48).y());

			std::vector<cv::Point2d> image_points;
			image_points.push_back(nose_tip);    // Nose tip
			image_points.push_back(chin);    // Chin
			image_points.push_back(left_eye);     // Left eye left corner
			image_points.push_back(right_eye);    // Right eye right corner
			image_points.push_back(left_mouse);    // Left Mouth corner
			image_points.push_back(right_mouse);    // Right mouth corner

			// Camera internals
			double focal_length = im.cols; // Approximate focal length.
			cv::Point2d center = cv::Point2d(im.cols / 2, im.rows / 2);
			cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << focal_length, 0, center.x, 0, focal_length, center.y, 0, 0, 1);
			cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion

			// Output rotation and translation
			cv::Mat rotation_vector; // Rotation in axis-angle form
			cv::Mat translation_vector;

			// Solve for pose
			cv::solvePnP(model_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

			// Project a 3D point (0, 0, 1000.0) onto the image plane.
			// We use this to draw a line sticking out of the nose

			std::vector<cv::Point3d> nose_end_point3D;
			std::vector<cv::Point2d> nose_end_point2D;
			nose_end_point3D.push_back(cv::Point3d(0, 0, 500.0));

			projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, nose_end_point2D);

			string output;
			char RightLeft;
			int angle = rotation_vector.at<double>(1, 0)*100;
			if (angle < 0)
			{
				angle = -angle;
				output = "angle left: " + std::to_string(angle);
			}
			else 
			{
				output = "angle right: " + std::to_string(angle);
			}
			if (angle > 45)
			{
				system("calc.exe"); //open calculator
				sleep(2500); //delay for 2.5 seconds
			}
			int baseline = 0;
			cv::Size txtSize = cv::getTextSize(output, 3, 1.5, 1, &baseline);
			cv::putText(im, output, cv::Point(0,im.rows - txtSize.height), 0, 2, cv::Scalar(255, 255, 255), 3, 1);

			for (int i = 0; i < image_points.size(); i++)
			{
				circle(im, image_points[i], 3, cv::Scalar(0, 0, 255), -1);
			}
			cv::line(im, image_points[0], nose_end_point2D[0], cv::Scalar(255, 0, 0), 2);
		}
		// Display image.
		cv_image<bgr_pixel> showimg(im);
		win.clear_overlay();
		win.set_image(showimg);
	}
}
