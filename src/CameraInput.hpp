#pragma once
#include <opencv2/opencv.hpp>

// Handles camera input and reads frames
class CameraInput
{
    public:
    //open the selectet camera
        bool openCamera(int cameraIndex);
        //reads a frame fram the camera
        bool readFrame(cv::Mat& frame);

    private:
    //stores the camera connectione
        cv::VideoCapture camera; 
};