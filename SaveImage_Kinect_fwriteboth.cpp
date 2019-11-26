/* This file is part of SaveImagesfromKinectV2 program.
* Program description : This program acquires frames from Kinect V2,
* send them to skeleton extractor, receives skeleton coordinates from
* skeleton extractor, crops hand bounding box, pass it through our
* hand gestures detector and outputs the gesture class.

* libfreenet2 code to stream RGB and depth from Kinect V2 is refered from
* sample code provided from libfreenet2: Protonect.cpp
* https://github.com/OpenKinect/libfreenect2

* For more description and citation:

* Copyright (C) 2019 -  Osama Mazhar (osamazhar@yahoo.com). All Right reserved.
*
* SaveImagesfromKinectV2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* Foobar is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SaveImagesfromKinectV2.  If not, see <https://www.gnu.org/licenses/>.
*/

//! [headers]
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <time.h>
#include <signal.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/core.hpp"
// #include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
//! [headers]
#include <chrono>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <errno.h>

bool protonect_shutdown = false; // Whether the running application should shut down.

void sigint_handler(int s)
{
  protonect_shutdown = true;
}

const char* keys =
{
  "{savedepth |1| whether to save depth image or not, 1 yes, 0 no}"
};

int main(int argc, const char** argv)
{

    cv::CommandLineParser parser(argc, argv, keys);

    int depthsave = parser.get<int>("savedepth");

    std::string folder_rgb, name_rgb;
    std::string folder_d, name_d;

    std::cout << "Enter the name of folder you want to save your images in: ";
    std::cin >> folder_rgb;

    const char * folder_name = folder_rgb.c_str();

    DIR* dir = opendir(folder_name);
    while(dir)
    {
      closedir(dir);
      std::cout << "Folder already exists; re-enter the name of the folder: ";
      std::cin >> folder_rgb;
      dir = opendir(folder_name);
    }

    if (ENOENT == errno)
    {
      const int dir_err = mkdir(folder_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if(-1 == dir_err)
        {
          std::cout << "Error creating folder!" << std::endl;
          exit(1);
        }
    }
    else
      {
        std::cout << "Cannot check if folder exists!" << std::endl;
        std::cout << "Closing Program!" << std::endl;
        exit(1);
      }

    std::cout << "Streaming from Kinect One sensor!" << std::endl;

    //! [context]
    libfreenect2::Freenect2 freenect2;
    libfreenect2::Freenect2Device *dev = 0;
    libfreenect2::PacketPipeline *pipeline = 0;
    //! [context]

    //! [discovery]
    if(freenect2.enumerateDevices() == 0)
    {
        std::cout << "no device connected!" << std::endl;
        return -1;
    }

    std::string serial = freenect2.getDefaultDeviceSerialNumber();

    std::cout << "SERIAL: " << serial << std::endl;

    if(pipeline)
    {
        //! [open]
        dev = freenect2.openDevice(serial, pipeline);
        //! [open]
    } else {
        dev = freenect2.openDevice(serial);
    }

    if(dev == 0)
    {
        std::cout << "failure opening device!" << std::endl;
        return -1;
    }

    signal(SIGINT, sigint_handler);
    protonect_shutdown = false;

    //! [listeners]
    libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color |
                                                  libfreenect2::Frame::Depth |
                                                  libfreenect2::Frame::Ir);
    libfreenect2::FrameMap frames;

    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);
    //! [listeners]

    //! [start]
    dev->start();

    std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
    std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;
    //! [start]

    //! [registration setup]
    libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
    libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4), depth2rgb(1920, 1080 + 2, 4); // check here (https://github.com/OpenKinect/libfreenect2/issues/337) and here (https://github.com/OpenKinect/libfreenect2/issues/464) why depth2rgb image should be bigger
    //! [registration setup]

    cv::Mat rgbmat, depthmatUndistorted, irmat, rgbd, depth_fullscale;
    // String imagename;
    unsigned int i = 1;
    //! [loop start]

    while(!protonect_shutdown)
    {
        listener.waitForNewFrame(frames);
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
        libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
        libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];
        //! [loop start]

        // cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data).copyTo(rgbmat);
        // cv::Mat(ir->height, ir->width, CV_32FC1, ir->data).copyTo(irmat);
        // cv::Mat(depth->height, depth->width, CV_32FC1, depth->data).copyTo(depthmat);
        auto tinit = std::chrono::high_resolution_clock::now();

        cv::Mat rgbmat(rgb->height, rgb->width, CV_8UC4, rgb->data);
        cv::Mat depthmat(depth->height, depth->width, CV_32FC1, depth->data);
	      cv::namedWindow("RGB Image", cv::WINDOW_NORMAL);
        cv::imshow("RGB Image", rgbmat);

        //! [registration]
        registration->apply(rgb, depth, &undistorted, &registered, true, &depth2rgb);
        //! [registration]

        cv::Mat(undistorted.height, undistorted.width, CV_32FC1, undistorted.data).copyTo(depthmatUndistorted);
        cv::Mat(registered.height, registered.width, CV_8UC4, registered.data).copyTo(rgbd);
        cv::Mat(depth2rgb.height, depth2rgb.width, CV_32FC1, depth2rgb.data).copyTo(depth_fullscale);

        cv::namedWindow("Depth Map", cv::WINDOW_NORMAL);
        cv::imshow("Depth Map", depth_fullscale / 4096.0f);
        // IMPORTANT TO PUT THIS PATH CORRECT //
        // imagename = "/home/osama/Programs/SaveImagesfromKinect/" + folder_rgb + "/" + folder_rgb + "_%d.bin";
        std::stringstream imagename;
        imagename << "/home/osama/Programs/SaveImagesfromKinect/" << folder_rgb << "/" << folder_rgb << "_" << i << ".bin";

        // char filename_rgb[80];
        // sprintf(filename_rgb,imagename.c_str(),i);

        int sizeofdepthimage[2] = {depth_fullscale.rows, depth_fullscale.cols};

        // FILE *FP=fopen(imagename.str().c_str(), "wb");
        std::fstream file(imagename.str(), std::ios::binary | std::ios::out);
        file.write((char*)sizeofdepthimage, 2*sizeof(int));
        file.write((char*)rgbmat.data, rgbmat.channels() * rgbmat.rows * rgbmat.cols*sizeof(uchar));
        file.write((char*)depth_fullscale.data, depth_fullscale.rows * depth_fullscale.cols*sizeof(float));

        // fwrite(sizeofdepthimage, sizeof(int), 2, FP);
        // fwrite(rgbmat.data, sizeof(uchar), rgbmat.channels() * rgbmat.rows * rgbmat.cols, FP);
        // fwrite(depth_fullscale.data, sizeof(float), depth_fullscale.rows * depth_fullscale.cols, FP);
        // fclose(FP);
        auto twriteimage = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> fp_ms = twriteimage-tinit;
        std::cout << "duration write depth="<< fp_ms.count() << std::endl;
        std::cout << "--------------------------------------------------------"<< std::endl;

        i++;
         // imwrite("osama.exr", depth_full_scale);

        int key = cv::waitKey(1);
        protonect_shutdown = protonect_shutdown || (key > 0 && ((key & 0xFF) == 27)); // shutdown on escape

    //! [loop end]
        listener.release(frames);
    }
    //! [loop end]

    //! [stop]
    dev->stop();
    dev->close();
    //! [stop]

    delete registration;

    std::cout << "Streaming Ends!" << std::endl;
    return 0;
}
