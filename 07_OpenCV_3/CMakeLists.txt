cmake_minimum_required(VERSION 3.12)
project(07_opencv_3)
set("OpenCV_DIR" "D:/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("images")
add_executable(07_opencv_3 main.cpp)
add_definitions("-DPATH_TO_IMG_OBJ=\"${CMAKE_SOURCE_DIR}/images/object.jpg\"")
add_definitions("-DPATH_TO_IMG_SCN=\"${CMAKE_SOURCE_DIR}/images/scene.jpg\"")
add_definitions("-DPATH_TO_IMG_RES=\"${CMAKE_SOURCE_DIR}/results/res_\"")
target_link_libraries(07_opencv_3 ${OpenCV_LIBS})