#include <iostream>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

using namespace std;
using namespace cv;

//全局变量
String src_windowName = "原图像";
String warp_windowName = "仿射变换";
String warp_rotate_windowName = "仿射旋转变换";
String rotate_windowName = "图像旋转";

double time() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (ts.tv_sec * 1e9 + ts.tv_nsec);
}

int main(int argv, char *args[])
{
    int src_h = 300, src_w = 256, dst_h = 300, dst_w = 256, num_channels = 4;
    if (argv == 6) {
      src_h = std::atoi(args[1]);
      src_w = std::atoi(args[2]);
      dst_h = std::atoi(args[3]);
      dst_w = std::atoi(args[4]);
      num_channels = std::atoi(args[5]);
    }
    std::cout << "src[" << src_h << ", " << src_w << "] to ";
    std::cout << "dst[" << dst_h << ", " << dst_w << "], channel = " << num_channels << std::endl;

    Point2f srcTri[3];
    Point2f dstTri[3];

    Mat warp_mat(2, 3, CV_32FC1);
    Mat srcImage, warp_dstImage;

    srcImage = imread("src.jpg");
    if (num_channels == 1) {
      cvtColor(srcImage, srcImage, COLOR_RGBA2GRAY);
    } else if (num_channels == 4) {
      cvtColor(srcImage, srcImage, COLOR_RGBA2BGRA);
    }
    resize(srcImage, srcImage, Size(src_w, src_h));

    warp_dstImage = Mat::zeros(dst_h, dst_w, srcImage.type());

    srcTri[0] = Point2f(40, 56);
    srcTri[1] = Point2f(66, 48);
    srcTri[2] = Point2f(60, 80);

    dstTri[0] = Point2f(dst_w / 3,    dst_h / 3);
    dstTri[1] = Point2f(dst_w * 2/3,  dst_h / 3);
    dstTri[2] = Point2f(dst_w / 2,    dst_h * 2/3);

    warp_mat = getAffineTransform(srcTri, dstTri);
    int loopN = 10;
    double avg_time = 0;
    // int gpu_num = cv::gpu::getCudaEnabledDeviceCount();
    // assert(gpu_num > 0);
    // gpu::setDevice(0);
    gpu::GpuMat src_img_gpu(srcImage);
    gpu::GpuMat dst_img_gpu(warp_dstImage);
    for (int i = 0; i < loopN; i++) {
      auto t1 = time();
      // warpAffine(srcImage, warp_dstImage, warp_mat, warp_dstImage.size());
      warpAffine(src_img_gpu, dst_img_gpu, warp_mat, warp_dstImage.size());
      auto t2 = time();
      std::cout << "cv time [" << i <<"]: " << (t2 - t1) / 1e3 << "us" << std::endl;
      avg_time += (t2 - t1) / 1e3;
    } 
    std::cout <<"avg cv time: " << avg_time / loopN << "us" << std::endl;
    waitKey(0);
    return 0;
}
