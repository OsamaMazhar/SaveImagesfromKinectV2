cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0012 NEW)
PROJECT("KinectOpenCV")

set(freenect2_DIR /home/osama/Programs/freenect2/lib/cmake/freenect2)
ADD_DEFINITIONS(-std=c++11)

find_package(OpenCV 3.3.1 EXACT PATHS /home/osama/Programs/opencv-3.3.1/build QUIET)
MESSAGE( "OpenCV version: ${OpenCV_VERSION}")

find_package(freenect2 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIR})
link_directories(${OpenCV_LIB_DIR})

include_directories("/usr/include/libusb-1.0/")

INCLUDE_DIRECTORIES(${freenect2_INCLUDE_DIR})

ADD_EXECUTABLE(SaveImage_Kinect_fwriteboth SaveImage_Kinect_fwriteboth.cpp)
ADD_EXECUTABLE(bin_to_png_and_videos bin_to_png_and_videos.cpp)


message("freenect2_LIBRARIES = ${freenect2_LIBRARIES}")
TARGET_LINK_LIBRARIES(SaveImage_Kinect_fwriteboth ${OpenCV_LIBS} ${freenect2_LIBRARIES})
TARGET_LINK_LIBRARIES(bin_to_png_and_videos ${OpenCV_LIBS} ${freenect2_LIBRARIES})
