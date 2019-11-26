#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <time.h>
#include <signal.h>
#include <opencv2/opencv.hpp>

#include <chrono>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

int main(int argc, const char** argv)
{
  std::string folder_input;
  std::string save_folder_name;
  std::cout << "Enter the Person name you want to read bin files of: ";
  std::cin >> folder_input;
  const char * Image_base_name = folder_input.c_str();

  std::cout << "Enter the generic name of folder you want to save your images in: ";
  std::cin >> save_folder_name;

  const char * saveimagebasename = save_folder_name.c_str();

  // // // >>> String Initialization for Reading >>> Single .bin file
  cv::String base_directory = "/home/osama/Programs/SaveImagesfromKinect/";
  // cv::String base_directory = "/media/osama/Elements/GestureRecordings/";
  cv::String mainfolder = base_directory + Image_base_name;
  cv::String Image_name = mainfolder + "/" + Image_base_name + "_%d.bin";
  // // // <<< String Initialization for Reading >>> Single .bin file

  // // // // >>> String Initialization for Reading >>> .bin and png file
  // // cv::String base_directory = "/media/osama/Elements/GestureRecordings/";
  // // cv::String base_directory = "/media/osama/Elements1/GestureRecordings/Full_bin/";
  // cv::String folder_location =  base_directory + Image_base_name;
  // std::cout << folder_location << std::endl;
  // cv::String folder = folder_location + "/" + "*.png"; // to be used in glob
  // cv::String rgb_Image_name = folder_location + "/" + Image_base_name + "_%d.png";
  // cv::String depth_folder = folder_location + "_d";
  // cv::String depth_Image_name = depth_folder + "/" + Image_base_name + "_d" + "_%d.bin";
  // // // // >>> String Initialization for Reading <<< .bin and png file


  // // >>> String Initialization for Writing
  // Saving in Second Harddrive External
  cv::String base_directory_write = "/home/osama/Programs/SaveImagesfromKinect/";
  // cv::String base_directory_write = "/media/osama/Elements1/GestureRecordings/";
  cv::String mainfolder_write = base_directory_write + Image_base_name;

  cv::String depth_foldername = mainfolder_write + "/depth_images";
  cv::String depth_Image_name_png = depth_foldername + "/" + saveimagebasename + "_d_%d.png";
  char filename_depth[200];

  cv::String videofolder = mainfolder_write + "/videos";
  cv::String rgbvideoname = videofolder + "/rgb_video.avi";
  cv::String depthvideoname = videofolder + "/depth_video.avi";  // // // >>> String Initialization for Reading >>> Single .bin file
  // cv::String base_directory = "/home/osama/Programs/SaveImagesfromKinect/";
  // cv::String base_directory = "/media/osama/Elements/GestureRecordings/";
  // cv::String mainfolder = base_directory + Image_base_name;
  // cv::String Image_name = mainfolder + "/" + Image_base_name + "_%d.bin";
  // // // <<< String Initialization for Reading >>> Single .bin file
  // // <<< String Initialization for Writing

  // DIR* dir = opendir(rgb_foldername.c_str());
  // if(!dir)
  // {
  //   const int dir_err = mkdir(rgb_foldername.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  //   if(-1 == dir_err)
  //     {
  //       std::cout << "Error creating rgb folder!" << std::endl;
  //       exit(1);
  //     }
  // }
  // else closedir(dir);
  //
  // DIR* dir = opendir(depth_foldername.c_str());
  // if(!dir)
  // {
  //   const int dir_err_d = mkdir(depth_foldername.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  //   if(-1 == dir_err_d)
  //     {
  //       std::cout << "Error creating depth folder!" << std::endl;
  //       exit(1);
  //     }
  // }
  // else closedir(dir);
  //
  // dir = opendir(depth_folder.c_str());
  // if(!dir)
  // {
  //   closedir(dir);
  //   std::cout << "Depth images folder do not exists" << std::endl;
  //   exit(1);
  // }

  // char filename[80];
  // FILE * pFile;
  // float * buffer;
  // int * sizeofimage;
  //
  // size_t result;
  // long bytesacquiredbysizeofimage;
  // long bytesofmatrixcontent, totalbytesoffile;
  // <<< Two Folders: RGB.png and Depth.bin Initialization

  // dir = opendir(depth_bin_foldername.c_str());
  // if(!dir)
  // {
  //   const int dir_err_d_b = mkdir(depth_bin_foldername.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  //   if(-1 == dir_err_d_b)
  //     {
  //       std::cout << "Error creating depth bin folder!" << std::endl;
  //       exit(1);
  //     }
  // }
  // else closedir(dir);

  DIR* dir = opendir(videofolder.c_str());
  if(!dir)
  {
    const int dir_err_video = mkdir(videofolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(-1 == dir_err_video)
      {
        std::cout << "Error creating video folder!" << std::endl;
        exit(1);
      }
  }
  else closedir(dir);

  char filename[80];
  FILE * pFile;
  uchar * rgbbuffer;
  float * depthbuffer;
  int * sizeofimage;

  size_t result;
  long totalbytesread;
  long bytesofsizeofdepthimage;

  // // <<< Only .bin with both RGB and Depth Initialization

  int initialframe = 1;
  std::string input;

  std::vector<cv::String> filenames;

  cv::glob(mainfolder, filenames); // new function that does the job ;-)
  std::cout << "Total number of frames in this folder: " << filenames.size() << std::endl;
  std::cout << "Start from frame number [default=1]: ";
  std::getline( std::cin, input );
  if ( !input.empty() ) {
      std::istringstream stream( input );
      stream >> initialframe;
  }
  std::cout << "Value of Initial Frame selected: " << initialframe << std::endl;

  cv::VideoWriter rgbVideo, depthVideo;

  int fourcc = CV_FOURCC('D','I','V','X');
  int fps = 18;

  rgbVideo.open(rgbvideoname, fourcc, fps, cv::Size(1920, 1080), true);
  if (!rgbVideo.isOpened())
    {
      std::cout  << "Could not open the rgb video for write: " << std::endl;
      return -1;
    }
  depthVideo.open(depthvideoname, fourcc, fps, cv::Size(1920, 1082), true);
  if (!depthVideo.isOpened())
    {
      std::cout  << "Could not open the depth video for write: " << std::endl;
      return -1;
    }

  cv::Mat intdepth, depth_int_onechannel, depth_floatto255;
  cv::Mat depth_multichannel;

  cv::namedWindow("RGB Image", cv::WINDOW_NORMAL);
  cv::namedWindow("Depth Image", cv::WINDOW_NORMAL);

  for(int i = initialframe; i < filenames.size(); i++)
  // for(int i = initialframe; i < 100; i++)
    {
      // // >>> filenames for writing
      sprintf(filename_depth,depth_Image_name_png.c_str(),i);
      // // <<< filenames for writing

      // >>> Only .bin File with RGB and Depth reading
      sprintf(filename, Image_name.c_str(), i);
      // <<< Only .bin File with RGB and Depth reading

      std::cout << "Processing file: " << filename << std::endl;
      pFile = fopen ( filename , "rb" );

      fseek (pFile , 0 , SEEK_END);
      totalbytesread = ftell (pFile);

      bytesofsizeofdepthimage = sizeof(int)*2;
      rewind (pFile);

      sizeofimage = (int*) malloc (2);

      result = fread(sizeofimage, 1, bytesofsizeofdepthimage, pFile);
      if (result != bytesofsizeofdepthimage){
        fputs ("Reading error",stderr); exit (3);
      }
      long bytesofdepthimage = sizeof(float)*sizeofimage[0]*sizeofimage[1];
      long bytesofrgbimage = totalbytesread - bytesofsizeofdepthimage - bytesofdepthimage;

      rgbbuffer = (uchar*) malloc (bytesofrgbimage);
      depthbuffer = (float *) malloc (bytesofdepthimage); // malloc assigns memory block in bytes

      result = fread(rgbbuffer, 1, bytesofrgbimage, pFile);
      if (result != bytesofrgbimage){
        fputs ("Reading error",stderr); exit (3);
      }

      result = fread(depthbuffer, 1, bytesofdepthimage, pFile);
      if (result != bytesofdepthimage){
        fputs ("Reading error",stderr); exit (3);
      }
      fclose(pFile);

      cv::Mat readdata = cv::Mat(1,bytesofrgbimage, CV_8UC1, rgbbuffer);
      cv::Mat src = readdata.reshape(4, 1080);
      cv::flip(src, src, 1);

      cv::Mat readdepthdata = cv::Mat(1,bytesofdepthimage / sizeof(float), CV_32FC1, depthbuffer);
      cv::Mat src_d = readdepthdata.reshape(0, sizeofimage[0]);
      cv::flip(src_d, src_d, 1);
      // <<< Only .bin File with RGB and Depth reading

      // // >>> Two Folders: RGB.png and Depth.bin Reading
      // sprintf(filename, rgb_Image_name.c_str(), i);
      // std::cout << filename << std::endl;
      // cv::Mat src = cv::imread(filename);
      // if(!src.data)
      //   {
      //     std::cerr << "Problem loading rgb image!!!" << std::endl;
      //     exit(1);
      //   }
      //
      // sprintf(filename, depth_Image_name.c_str(), i);
      // pFile = fopen ( filename , "rb" );
      //
      // fseek (pFile , 0 , SEEK_END);
      // totalbytesoffile = ftell (pFile);
      // bytesacquiredbysizeofimage = sizeof(int)*2;
      // bytesofmatrixcontent = totalbytesoffile - bytesacquiredbysizeofimage;
      //
      // rewind (pFile);
      //
      // int matrix_members = bytesofmatrixcontent / sizeof(float);
      //
      // buffer = (float*) malloc (bytesofmatrixcontent);
      // sizeofimage = (int*) malloc (bytesacquiredbysizeofimage);
      //
      // result = fread (buffer,1, bytesofmatrixcontent,pFile);
      //  if (result != bytesofmatrixcontent){
      //    fputs ("Reading error",stderr); exit (3);
      //  }
      //
      // result = fread (sizeofimage,1, bytesacquiredbysizeofimage,pFile);
      //   if (result != bytesacquiredbysizeofimage){
      //     fputs ("Reading error",stderr); exit (3);
      //   }
      //
      // fclose (pFile);
      //
      // cv::Mat readdata = cv::Mat(1,matrix_members, CV_32FC1, buffer);
      // //std::cout << "Size of readdata: " << "(" << readdata.rows << ", " << readdata.cols << ")" << std::endl;
      //
      // cv::Mat src_d = readdata.reshape(1, sizeofimage[0]); // giving rows
      // if(!src_d.data)
      //   {
      //     std::cerr << "Problem loading depth image!!!" << std::endl;
      //     exit(1);
      //   }
      // // // <<< Two Folders: RGB.png and Depth.bin Reading


      // For saving rgb images and videos we need to convert BGRA to BGR
      cv::cvtColor(src, src, cv::COLOR_BGRA2BGR);

      // For display only we need to convert the float depth grayscale into
      // CV_8U 3 channels. We will replicate the single channel data into
      // three channels and will merge the array of matrices to form an image.
      depth_floatto255 = src_d / 4096.0f * 255;
      depth_floatto255.convertTo(depth_int_onechannel, CV_8UC1);
      cv::Mat depth_temp[] = {depth_int_onechannel, depth_int_onechannel, depth_int_onechannel};
      cv::merge(depth_temp, 3, depth_multichannel);
      cv::imshow("Depth Image", depth_multichannel);
      // cv::waitKey(0);
      // >>> Transferring the rgb (BGR) and depth images to video writer
      rgbVideo << src;
      depthVideo << depth_multichannel;
      // <<< Transferring the rgb (BGR) and depth images to video writer
      std::cout << filename_depth << std::endl;
      // >>> Writing Color and depth images (both in png)
      cv::imwrite(filename_depth, depth_multichannel);
      // >>> Writing Color and depth images (both in png)

      cv::imshow("RGB Image", src);
      cv::imshow("Depth Image", depth_multichannel);

      cv::waitKey(1);

      free (rgbbuffer);
      free (depthbuffer);
      free (sizeofimage);

      // free (buffer);
      // free (sizeofimage);
    }
    rgbVideo.release();
    depthVideo.release();

}
// char filename[80];
//
// FILE * pFile;
//
// float * buffer;
// int * sizeofimage;
//
// size_t result;
// long bytesacquiredbysizeofimage;
// long bytesofmatrixcontent, totalbytesoffile;

// // In the Loop after Glob (See FromImages_old.cpp)
// sprintf(filename, depth_Image_name.c_str(), i);
// pFile = fopen ( filename , "rb" );
//
// fseek (pFile , 0 , SEEK_END);
// totalbytesoffile = ftell (pFile);
// bytesacquiredbysizeofimage = sizeof(int)*2;
// bytesofmatrixcontent = totalbytesoffile - bytesacquiredbysizeofimage;
//
// rewind (pFile);
//
// int matrix_members = bytesofmatrixcontent / sizeof(float);
//
// buffer = (float*) malloc (bytesofmatrixcontent);
// sizeofimage = (int*) malloc (bytesacquiredbysizeofimage);
//
// result = fread (buffer,1, bytesofmatrixcontent,pFile);
//  if (result != bytesofmatrixcontent){
//    fputs ("Reading error",stderr); exit (3);
//  }
//
// result = fread (sizeofimage,1, bytesacquiredbysizeofimage,pFile);
//   if (result != bytesacquiredbysizeofimage){
//     fputs ("Reading error",stderr); exit (3);
//   }
//
// fclose (pFile);
//
// cv::Mat readdata = cv::Mat(1,matrix_members, CV_32FC1, buffer);
// //std::cout << "Size of readdata: " << "(" << readdata.rows << ", " << readdata.cols << ")" << std::endl;
//
// cv::Mat src_d = readdata.reshape(1, sizeofimage[0]); // giving rows
//
// //std::cout << "fread sizeofimage: " << "(" << sizeofimage[0] << ", " << sizeofimage[1] << ")" << std::endl;
// //std::cout << "Size of src_d: " << "(" << src_d.rows << ", " << src_d.cols << ")" << std::endl;
// //cv::namedWindow("Depth", cv::WINDOW_NORMAL);
// //cv::imshow("Depth", src_d / 4096.0f);
//
// if(!src_d.data)
//   {
//     std::cerr << "Problem loading depth image!!!" << std::endl;
//     exit(1);
//   }
