cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0012 NEW)
PROJECT("KinectOpenCV")

set(freenect2_DIR /home/osama/Programs/freenect2/lib/cmake/freenect2)

# Requires OpenCV
#FIND_PACKAGE( OpenCV 3.0.0 REQUIRED )
find_package(OpenCV 3.3.1 EXACT PATHS /home/osama/Programs/opencv-3.3.1/build QUIET)
MESSAGE( "OpenCV version: ${OpenCV_VERSION}")

find_package(freenect2 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIR})
link_directories(${OpenCV_LIB_DIR})

include_directories("/usr/include/libusb-1.0/")

INCLUDE_DIRECTORIES(${freenect2_INCLUDE_DIR})

#ADD_EXECUTABLE(KinectOpenCV KinectOpenCV.cpp)
ADD_EXECUTABLE(SaveImage_Kinect_fwriteboth SaveImage_Kinect_fwriteboth.cpp)
# ADD_EXECUTABLE(Matrixoperation Matrixoperation.cpp)
# ADD_EXECUTABLE(KinectOpenCV_readwrite KinectOpenCV_readwrite.cpp)
# ADD_EXECUTABLE(bintopng bintopng.cpp)
ADD_EXECUTABLE(bintopng_png_bin bintopng_png_bin.cpp)
# ADD_EXECUTABLE(readbindataset readbindataset.cpp)

ADD_DEFINITIONS(-std=c++11)

message("freenect2_LIBRARIES = ${freenect2_LIBRARIES}")
TARGET_LINK_LIBRARIES(SaveImage_Kinect_fwriteboth ${OpenCV_LIBS} ${freenect2_LIBRARIES})
#TARGET_LINK_LIBRARIES(KinectOpenCV ${OpenCV_LIBS} ${freenect2_LIBRARIES})
# TARGET_LINK_LIBRARIES(Matrixoperation ${OpenCV_LIBS} ${freenect2_LIBRARIES})
# TARGET_LINK_LIBRARIES(KinectOpenCV_readwrite ${OpenCV_LIBS} ${freenect2_LIBRARIES})
# TARGET_LINK_LIBRARIES(bintopng ${OpenCV_LIBS} ${freenect2_LIBRARIES})
# TARGET_LINK_LIBRARIES(readbindataset ${OpenCV_LIBS} ${freenect2_LIBRARIES})
TARGET_LINK_LIBRARIES(bintopng_png_bin ${OpenCV_LIBS} ${freenect2_LIBRARIES})
