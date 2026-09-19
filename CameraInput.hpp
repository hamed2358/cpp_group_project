#pragma once
#include <opencv2/opencv.hpp>


class CameraInput
{
    public:
        bool openCamera(int cameraIndex);
        bool readFrame(cv::Mat& frame);

    private:
        cv::VideoCapture camera; 
};