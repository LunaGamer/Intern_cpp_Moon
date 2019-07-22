#include <iostream>
#include <ctime>
#include "opencv2/core.hpp"
#ifdef HAVE_OPENCV_XFEATURES2D
#include "opencv2/calib3d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
using namespace cv;
using namespace cv::xfeatures2d;
using std::cout;
using std::endl;


std::string drawResult(Mat img_object, Mat img_scene, std::vector<KeyPoint> keypoints_object, std::vector<KeyPoint>  keypoints_scene, std::vector< std::vector<DMatch> > knn_matches, std::vector<DMatch> good_matches, std::string name, time_t t)
{
	int match = 0;
	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene, good_matches, img_matches, Scalar::all(-1),
		Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}
	Mat H = findHomography(obj, scene, RANSAC);
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point2f(0, 0);
	obj_corners[1] = Point2f((float)img_object.cols, 0);
	obj_corners[2] = Point2f((float)img_object.cols, (float)img_object.rows);
	obj_corners[3] = Point2f(0, (float)img_object.rows);
	std::vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);
	line(img_matches, scene_corners[0] + Point2f((float)img_object.cols, 0),
		scene_corners[1] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f((float)img_object.cols, 0),
		scene_corners[2] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f((float)img_object.cols, 0),
		scene_corners[3] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f((float)img_object.cols, 0),
		scene_corners[0] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4);
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		if (keypoints_scene[good_matches[i].trainIdx].pt.x < scene_corners[1].x && keypoints_scene[good_matches[i].trainIdx].pt.x > scene_corners[3].x && keypoints_scene[good_matches[i].trainIdx].pt.y > scene_corners[0].y && keypoints_scene[good_matches[i].trainIdx].pt.y < scene_corners[2].y)
		{
			match++;
		}
	}
	imwrite(PATH_TO_IMG_RES+name+".jpg", img_matches);
	imshow(name, img_matches);
	waitKey();
	return "report for "+name+":\nall ponints: "+std::to_string(good_matches.size())+"\nperfect matches: " + std::to_string(match)+"\nin %: " + std::to_string(((float)match/ (float)good_matches.size())*100)+"\ntime: "+std::to_string(t)+"s";
}

int main( int argc, char* argv[] )
{
	time_t t,t1, t2;
    Mat img_object = imread(PATH_TO_IMG_OBJ);
    Mat img_scene = imread(PATH_TO_IMG_SCN);
    if ( img_object.empty() || img_scene.empty() )
    {
        cout << "Could not open or find the image!\n" << endl;
        return -1;
    }
	int minHessian = 1000;
	std::string name, res1, res2, res3, res4;
	const float ratio_thresh = 0.75f;
	name = "SURF"; //SURF method
	t1 = time(NULL);
	Ptr<SURF> detector1 = SURF::create( minHessian );
    std::vector<KeyPoint> keypoints_object1, keypoints_scene1;
    Mat descriptors_object1, descriptors_scene1;
    detector1->detectAndCompute( img_object, noArray(), keypoints_object1, descriptors_object1 );
    detector1->detectAndCompute( img_scene, noArray(), keypoints_scene1, descriptors_scene1 );
    Ptr<DescriptorMatcher> matcher1 = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    std::vector< std::vector<DMatch> > knn_matches1;
    matcher1->knnMatch( descriptors_object1, descriptors_scene1, knn_matches1, 2 );
    std::vector<DMatch> good_matches1;
    for (size_t i = 0; i < knn_matches1.size(); i++)
    {
        if (knn_matches1[i][0].distance < ratio_thresh * knn_matches1[i][1].distance)
        {
            good_matches1.push_back(knn_matches1[i][0]);
        }
    }
	t2 = time(NULL);
	t = t2 - t1;
	res1 = drawResult(img_object, img_scene, keypoints_object1, keypoints_scene1, knn_matches1, good_matches1, name,t );

	name = "ORB"; //ORB method
	t1 = time(NULL);
	Ptr<ORB> detector2 = ORB::create(minHessian);
	std::vector<KeyPoint> keypoints_object2, keypoints_scene2;
	Mat descriptors_object2, descriptors_scene2;
	detector2->detectAndCompute(img_object, noArray(), keypoints_object2, descriptors_object2);
	detector2->detectAndCompute(img_scene, noArray(), keypoints_scene2, descriptors_scene2);
	Ptr<DescriptorMatcher> matcher2 = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE_HAMMING);
	std::vector< std::vector<DMatch> > knn_matches2;
	matcher2->knnMatch(descriptors_object2, descriptors_scene2, knn_matches2, 2);
	std::vector<DMatch> good_matches2;
	for (size_t i = 0; i < knn_matches2.size(); i++)
	{
		if (knn_matches2[i][0].distance < ratio_thresh * knn_matches2[i][1].distance)
		{
			good_matches2.push_back(knn_matches2[i][0]);
		}
	}
	t2 = time(NULL);
	t = t2 - t1;
	res2 = drawResult(img_object, img_scene, keypoints_object2, keypoints_scene2, knn_matches2, good_matches2, name, t);

	name = "AKAZE"; //AKAZE method
	t1 = time(NULL);
	Ptr<AKAZE> detector3 = AKAZE::create();
	std::vector<KeyPoint> keypoints_object3, keypoints_scene3;
	Mat descriptors_object3, descriptors_scene3;
	detector3->detectAndCompute(img_object, noArray(), keypoints_object3, descriptors_object3);
	detector3->detectAndCompute(img_scene, noArray(), keypoints_scene3, descriptors_scene3);
	Ptr<DescriptorMatcher> matcher3 = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE_HAMMING);
	std::vector< std::vector<DMatch> > knn_matches3;
	matcher3->knnMatch(descriptors_object3, descriptors_scene3, knn_matches3, 2);
	std::vector<DMatch> good_matches3;
	for (size_t i = 0; i < knn_matches3.size(); i++)
	{
		if (knn_matches3[i][0].distance < ratio_thresh * knn_matches3[i][1].distance)
		{
			good_matches3.push_back(knn_matches3[i][0]);
		}
	}
	t2 = time(NULL);
	t = t2 - t1;
	res3 = drawResult(img_object, img_scene, keypoints_object3, keypoints_scene3, knn_matches3, good_matches3, name, t);

	name = "KAZE"; //GFTT method
	t1 = time(NULL);
	Ptr<KAZE> detector4 = KAZE::create();
	std::vector<KeyPoint> keypoints_object4, keypoints_scene4;
	Mat descriptors_object4, descriptors_scene4;
	detector4->detectAndCompute(img_object, noArray(), keypoints_object4, descriptors_object4);
	detector4->detectAndCompute(img_scene, noArray(), keypoints_scene4, descriptors_scene4);
	Ptr<DescriptorMatcher> matcher4 = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
	std::vector< std::vector<DMatch> > knn_matches4;
	matcher4->knnMatch(descriptors_object4, descriptors_scene4, knn_matches4, 2);
	std::vector<DMatch> good_matches4;
	for (size_t i = 0; i < knn_matches4.size(); i++)
	{
		if (knn_matches4[i][0].distance < ratio_thresh * knn_matches4[i][1].distance)
		{
			good_matches4.push_back(knn_matches4[i][0]);
		}
	}
	t2 = time(NULL);
	t = t2 - t1;
	res4 = drawResult(img_object, img_scene, keypoints_object4, keypoints_scene4, knn_matches4, good_matches4, name, t);
   
	cv::destroyAllWindows();
	cout << res1 << endl << endl << res2 << endl << endl << res3 << endl << endl << res4 << endl;
    return 0;
}
#else
int main()
{
    std::cout << "This tutorial code needs the xfeatures2d contrib module to be run." << std::endl;
    return 0;
}
#endif