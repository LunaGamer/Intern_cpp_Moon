cmake_minimum_required(VERSION 3.12)
project(Dlib_Face3D)
add_subdirectory(D:/Dlib/dlib dlib_build)
set("OpenCV_DIR" "D:/opencv/build")
add_definitions("-DPATH_TO_FACE_LANDMARKS=\"D:/Dlib/dlib/shape_predictor_68_face_landmarks.dat\"")
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    add_executable(Dlib_Face3D main.cpp)
    target_link_libraries(Dlib_Face3D dlib::dlib ${OpenCV_LIBS})
else()
    message("OpenCV not found")
    endif()